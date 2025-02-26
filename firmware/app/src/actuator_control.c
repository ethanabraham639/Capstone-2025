#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>

#include "actuator_control.h"
#include "pca9685.h"
#include "delay.h"

#define STEP_MAGNITUDE              1   // the step increase of the current servo position towards its desired position
#define AC_TASK_DELAY               20

#define CS_COLUMN_DELAY_MS          2000

/**
 * We are using "roll-out" groups to roll out servo motor changes incrementally to reduce
 * current draw at a given time.
 * 
 * Since the number of actuators are fixed, the number of roll out groups determines the
 * number of members (servo motors) per group.
 * 
 * NOTE: make sure NUM_ACTUATORS is evenly divisible by NUM_ROLLOUT_GROUPS 
 */
#define NUM_ROLLOUT_GROUPS          3
#define NUM_MEMBERS_PER_RO_GROUP   (NUM_ACTUATORS/NUM_ROLLOUT_GROUPS)
#define ROLLOUT_GROUP_DELAY_MS      10

/**
 * Every 15 servos are being controlled by a different board, we are calling each board a HW group.
 * We need to keep track of this becuase each board has a different I2C address that controls a different
 * group of servos.
 * 
 * We need a way to convert an "Absolute Servo ID", from 0-44 to a "Relative Servo ID", from 1-15, along with
 * knowing which board needs to be communicated with to move the specfic servo.
 * 
 * The reason "Relative Servo ID" starts at 1, is becuase each board can actually control 16 total servos, but
 * servo 0 is used for a different purpose, not related to actuator control, leaving us with 15 servos.
 */
#define NUM_HW_GROUPS               3
#define NUM_SERVOS_PER_HW_GROUP     15
#define REL_SERVO_ID_OFFSET         1

#define MAX_SERVO_POSITION          90
#define STARTING_SERVO_POSITION     0

#define INIT_SERVOS_DELAY_MS        1500

typedef enum {
    IDLE = 0,
    MODE_SELECT,
    CLEAR_SEQUENCE_MODE_on_enter,
    CLEAR_SEQUENCE_MODE,
    STATIC_CONTROL_MODE,
    MOVE_ACTUATORS
} ACState_e;

#define NUM_COLUMNS 5 
typedef struct {
    bool isActive;
    uint8_t currentColumn;
    Timer_t timer;

    uint8_t columnPositions[NUM_COLUMNS];
} ClearSequence_t;

ClearSequence_t CS = {
    .isActive = false,
    .currentColumn = 0,
    .timer = 0,
    .columnPositions = {0, 10, 30, 60, 90}
};

// struct describing the actuator control task
typedef struct {
    uint8_t currentPos[NUM_ACTUATORS];
    uint8_t desiredPos[NUM_ACTUATORS];
    uint8_t reqDesiredPos[NUM_ACTUATORS];
    bool newDesiredPosReq;
    ACMode_e mode;
    ACMode_e prevMode;
    ACState_e state;
} ActControl_t;
ActControl_t AC;

/**
 * HW group array holding the PCA9685 instances.
 * 
 * hwGroups[0] -> 0-14  Absolute Servo ID
 * hwGroups[1] -> 15-29 Absolute Servo ID
 * hwGroups[2] -> 20-44 Absolute Servo ID
 * 
 */ 
const PCA9685_t hwGroups[NUM_HW_GROUPS] = {
    { .addr = 0x60, .isLed = false, .osc_freq = 26984448.0 },
    { .addr = 0x61, .isLed = false, .osc_freq = 26484736.0 },
    { .addr = 0x62, .isLed = false, .osc_freq = 26484736.0 },
};

/**
 * Array that holds the Absolute Servo IDs arranged in the rollout order.
 * 
 * Members in the same group are moved at the same time (without blocking delay).
 */
uint8_t rolloutGroups[NUM_ROLLOUT_GROUPS][NUM_MEMBERS_PER_RO_GROUP];

// Use this function to implement the rollout order
void init_rollout_groups(void)
{
    // Currently the groups are the same as the HW groups. This can change.
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        uint8_t group = i / NUM_MEMBERS_PER_RO_GROUP;
        uint8_t member = i % NUM_MEMBERS_PER_RO_GROUP;
        rolloutGroups[group][member] = i;
    }
}

// Based on the current and desired positions, the new current position is updated.
bool calculate_next_position(void)
{
    bool didPositionChange = false;

    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        int deltaTotal = AC.desiredPos[i] - AC.currentPos[i];
        
        if (deltaTotal != 0)
        {
            didPositionChange = true;
            int step = (abs(deltaTotal) < STEP_MAGNITUDE) ? deltaTotal : (deltaTotal / abs(deltaTotal)) * STEP_MAGNITUDE;
            
            AC.currentPos[i] += step;
        }
    }

    return didPositionChange;
}

// Based on the rollout groups, physically rollout the changes
void rollout_actuator_positions(void)
{
    // iterate through each member of each rollout group
    for (uint8_t group = 0; group < NUM_ROLLOUT_GROUPS; group++)
    {
        for (uint8_t member = 0; member < NUM_MEMBERS_PER_RO_GROUP; member++)
        {
            // first, find the absolute servo ID
            uint8_t absoluteServoId = rolloutGroups[group][member];
            
            // calculate the relative servo ID and apply the offset
            uint8_t relativeServoId = (absoluteServoId % NUM_SERVOS_PER_HW_GROUP) + REL_SERVO_ID_OFFSET;
            
            // calculate the hw group
            uint8_t hwGroup = absoluteServoId / NUM_SERVOS_PER_HW_GROUP;

            // rollout!
            PCA9685_setServoPos(&hwGroups[hwGroup], relativeServoId, AC.currentPos[absoluteServoId]);
        }

        vTaskDelay(ROLLOUT_GROUP_DELAY_MS / portTICK_PERIOD_MS);
    }
}


// Init everything to do with actuator control
void AC_init(void)
{
    init_rollout_groups();

    AC.mode = STATIC;
    AC.prevMode = STATIC;
    AC.state = IDLE;
    
    // set desired and current positions to known state
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        AC.currentPos[i] = 0;
        AC.desiredPos[i] = 0;
    }

    // init the PCA9685 chips for each hw group
    for (uint8_t i = 0; i < NUM_HW_GROUPS; i++)
    {
        PCA9685_init(&hwGroups[i]);
    }

    // force all motors to a default, known position, respecting the rollout groups
    for (uint8_t group = 0; group < NUM_ROLLOUT_GROUPS; group++)
    {
        for (uint8_t member = 0; member < NUM_MEMBERS_PER_RO_GROUP; member++)
        {
            // between 0-44, actual position of the motor on the course
            uint8_t absoluteServoId = rolloutGroups[group][member];
            
            uint8_t relativeServoId = (absoluteServoId % NUM_SERVOS_PER_HW_GROUP) + REL_SERVO_ID_OFFSET;
            uint8_t hwGroup = absoluteServoId / NUM_SERVOS_PER_HW_GROUP;

            PCA9685_setServoPos(&hwGroups[hwGroup], relativeServoId, STARTING_SERVO_POSITION);
        }
        
        vTaskDelay(INIT_SERVOS_DELAY_MS / portTICK_PERIOD_MS);
    }
}

void AC_run_task(void)
{
    switch (AC.state)
    {
        case IDLE:
            // Do nothing in IDLE state
            break;

        case MODE_SELECT:
            switch (AC.mode)
            {
                case CLEAR_SEQUENCE:
                    AC.state = CLEAR_SEQUENCE_MODE_on_enter;
                    break;

                case STATIC:
                    AC.state = STATIC_CONTROL_MODE;
                    break;
            }
            break;

        case CLEAR_SEQUENCE_MODE_on_enter:

            CS.timer = INT64_MAX;
            CS.isActive = true;
            CS.currentColumn = 0;

            //MAYBE: MOVE EVERYTHING TO 0 POSITION FIRST

            AC.state = CLEAR_SEQUENCE_MODE;
            break;

        case CLEAR_SEQUENCE_MODE:

            const uint8_t currentColumn = CS.currentColumn;

            if (TIMER_get_ms( CS.timer) > CS_COLUMN_DELAY_MS)
            {
                if (currentColumn >= TOTAL_COLUMNS)
                {
                    CS.isActive = false;
                    AC.mode = AC.prevMode;  // reinstate the previous mode
                    AC.state = MODE_SELECT;
                }
                else
                {
                    const uint8_t columnPos = CS.columnPositions[currentColumn];
        
                    for (uint8_t row = 0; row < TOTAL_ROWS; row++)
                    {
                        AC.desiredPos[row * TOTAL_COLUMNS + currentColumn] = columnPos;
                    }
        
                    CS.currentColumn++;
                    CS.timer = TIMER_restart();
                }
            }

            AC.state = MOVE_ACTUATORS;

            break;

        case STATIC_CONTROL_MODE:

            if (AC.newDesiredPosReq)
            {
                memcpy(AC.desiredPos, AC.reqDesiredPos, NUM_ACTUATORS);
                AC.newDesiredPosReq = false;
            }

            AC.state = MOVE_ACTUATORS;
            break;

        case MOVE_ACTUATORS:
            // calculate next positions based on current and desired position
            bool didPositionsChange = calculate_next_position();

            // don't run anything else if the new positions are the same as the old positions
            if (didPositionsChange)
            {
                //execute rollout of the new positions
                rollout_actuator_positions();
            }

            // delay
            vTaskDelay(AC_TASK_DELAY / portTICK_PERIOD_MS);
            break;
    }
}

void AC_update_desired_positions(uint8_t desiredPos[NUM_ACTUATORS])
{
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        if (desiredPos[i] > MAX_SERVO_POSITION)
        {
            AC.reqDesiredPos[i] = MAX_SERVO_POSITION;    
        }
        else
        {
            AC.reqDesiredPos[i] = desiredPos[i];
        }
    }

    AC.newDesiredPosReq = true;
}

void AC_update_mode(ACMode_e mode)
{
    if (CS.isActive == false)
    {
        if (mode == CLEAR_SEQUENCE)
        {
            AC.prevMode = AC.mode; //storing the old mode to switch back too if its a clearing sequence
        }

        AC.mode = mode;

    }
    //else ignore the mode update
     
}