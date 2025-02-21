#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>

#include "actuator_control.h"
#include "pca9685.h"

#define STEP_MAGNITUDE              1   // the step increase of the current servo position towards its desired position
#define AC_TASK_DELAY               20

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


// struct describing the actuator control task
typedef struct {
    uint8_t currentPos[NUM_ACTUATORS];
    uint8_t desiredPos[NUM_ACTUATORS];
    ACMode_e mode;
} ActControl_t;
ActControl_t actControl;

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
        int deltaTotal = actControl.desiredPos[i] - actControl.currentPos[i];
        
        if (deltaTotal != 0)
        {
            didPositionChange = true;
            int step = (abs(deltaTotal) < STEP_MAGNITUDE) ? deltaTotal : (deltaTotal / abs(deltaTotal)) * STEP_MAGNITUDE;
            
            actControl.currentPos[i] += step;
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
            PCA9685_setServoPos(&hwGroups[hwGroup], relativeServoId, actControl.currentPos[absoluteServoId]);
        }

        vTaskDelay(ROLLOUT_GROUP_DELAY_MS / portTICK_PERIOD_MS);
    }
}


// Init everything to do with actuator control
void AC_init(void)
{
    init_rollout_groups();

    actControl.mode = STATIC;
    
    // set desired and current positions to known state
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        actControl.currentPos[i] = 0;
        actControl.desiredPos[i] = 0;
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
}

void AC_update_desired_positions(uint8_t desiredPos[NUM_ACTUATORS])
{
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        if (desiredPos[i] > MAX_SERVO_POSITION)
        {
            actControl.desiredPos[i] = MAX_SERVO_POSITION;    
        }
        else
        {
            actControl.desiredPos[i] = desiredPos[i];
        }
    }
}

void AC_update_mode(ACMode_e mode)
{
    actControl.mode = mode;
}