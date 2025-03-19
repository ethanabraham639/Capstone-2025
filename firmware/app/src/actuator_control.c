#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>

#include "actuator_control.h"
#include "pca9685.h"
#include "user_nvs.h"
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
#define ROLLOUT_GROUP_DELAY_MS      20

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

typedef enum {
    IDLE = 0,
    MODE_SELECT,
    CLEAR_SEQUENCE_MODE_on_enter,
    CLEAR_SEQUENCE_MODE,
    STATIC_CONTROL_MODE,
    MOVE_ACTUATORS
} ACState_e;

void AC_idle_state(void);
void AC_mode_select_state(void);
void AC_clear_sequence_mode_on_enter_state(void);
void AC_clear_sequence_mode_state(void);
void AC_static_control_mode_state(void);
void AC_move_actuators_state(void);

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
    .columnPositions = {90, 70, 0, 70, 90}
};

// struct describing the actuator control task
typedef struct {
    uint8_t currentPos[NUM_ACTUATORS];      // Actual current position
    uint8_t desiredPos[NUM_ACTUATORS];      // Actual desired position
    uint8_t reqDesiredPos[NUM_ACTUATORS];   // Requested desired position

    ACMode_e mode;                          // Actual current mode

    bool newCourseStateReq;                 // Flag to indicate a new course state is requested

    bool reqClearSequence;                  // Flag to indicate a clear sequence is requested

    bool saveCourseState;                   // Flag to indicate the course state should be saved

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
    { .addr = 0x43, .isLed = false, .osc_freq = 26434765.0 },
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
    AC.reqClearSequence = false;
    AC.saveCourseState = false;
    AC.state = IDLE;
    
    esp_err_t nvs_err = NVS_read_course_state(AC.currentPos);

    if (nvs_err == ESP_OK)
    {
        for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
        {
            AC.desiredPos[i] = AC.currentPos[i];
            AC.reqDesiredPos[i] = AC.currentPos[i];
        }       
    }
    else
    {
        //set the defaults
        for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
        {
            AC.currentPos[i] = 0;
            AC.desiredPos[i] = 0;
            AC.reqDesiredPos[i] = 0;
        }
    }

    // set desired and current positions to known state

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

            PCA9685_setServoPos(&hwGroups[hwGroup], relativeServoId, AC.currentPos[absoluteServoId]);
        }
    }
}

void AC_idle_state(void)
{
    // Do nothing in IDLE state
    AC.state = MODE_SELECT;
}

void AC_mode_select_state(void)
{
    switch (AC.mode)
    {
        case STATIC:
            AC.state = STATIC_CONTROL_MODE;
            break;
    }

    if (AC.reqClearSequence || CS.isActive)
    {
        AC.state = CLEAR_SEQUENCE_MODE_on_enter;
        AC.reqClearSequence = false;
    }
}

void AC_clear_sequence_mode_on_enter_state(void)
{
    if (!CS.isActive)
    {
        printf("Entering clear sequence for the first time\n'");
        CS.isActive = true;
        CS.timer = INT64_MAX;
        CS.currentColumn = 0;
        
        memcpy(AC.reqDesiredPos, AC.desiredPos, NUM_ACTUATORS);
        AC.newCourseStateReq = true; 
    }
    
    AC.state = CLEAR_SEQUENCE_MODE;
    // printf("Clear Sequence On Enter\n");
}

void AC_clear_sequence_mode_state(void)
{
    const uint8_t currentColumn = CS.currentColumn;

    if ((TIMER_get_ms(CS.timer) > CS_COLUMN_DELAY_MS) || (currentColumn == 0))
    {
        if (currentColumn >= NUM_COLUMNS)
        {
            printf("Clear sequence done\n");
            CS.isActive = false;
            AC.state = MODE_SELECT;
            return;
        }
        else
        {
            const uint8_t columnPos = CS.columnPositions[currentColumn];

            for (uint8_t row = 0; row < NUM_ROWS; row++)
            {
                AC.desiredPos[row * NUM_COLUMNS + currentColumn] = columnPos;
            }

            CS.currentColumn++;
            CS.timer = TIMER_restart();
            printf("Moving next column\n");
        }
    }

    AC.state = MOVE_ACTUATORS;

    // printf("Clear Sequence\n");
}

void AC_static_control_mode_state(void)
{
    if (AC.newCourseStateReq)
    {
        printf("new course state detected\n");
        memcpy(AC.desiredPos, AC.reqDesiredPos, NUM_ACTUATORS);
        AC.newCourseStateReq = false;
    }

    AC.state = MOVE_ACTUATORS;

    // printf("Static Control Mode\n");
}

void AC_move_actuators_state(void)
{
    bool didPositionsChange = calculate_next_position();

    if (didPositionsChange)
    {
        rollout_actuator_positions();
    }

    vTaskDelay(AC_TASK_DELAY / portTICK_PERIOD_MS);

    AC.state = MODE_SELECT;
    // printf("Move Actuators\n");
}

void AC_run_task(void)
{
    switch (AC.state)
    {
        case IDLE:
            AC_idle_state();
            break;
        case MODE_SELECT:
            AC_mode_select_state();
            break;
        case CLEAR_SEQUENCE_MODE_on_enter:
            AC_clear_sequence_mode_on_enter_state();
            break;
        case CLEAR_SEQUENCE_MODE:
            AC_clear_sequence_mode_state();
            break;
        case STATIC_CONTROL_MODE:
            AC_static_control_mode_state();
            break;
        case MOVE_ACTUATORS:
            AC_move_actuators_state();
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
    
    AC.newCourseStateReq = true;
    AC.saveCourseState = true;

    printf("Desired positions updated\n");
}

void AC_update_mode(ACMode_e mode)
{
    AC.mode = mode;
}

void AC_req_clear_sequence(void)
{
    AC.reqClearSequence = true;
    printf("Clear sequence flag set!!\n");
}