#include "actuator_control.h"
#include "pca9685.h"

#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>


#define MAX_POSITION                255.0
#define MAX_DELAY_MS                2000.0
#define DELTA_CONVERSION_MS         (MAX_DELAY_MS/MAX_POSITION)

#define AC_STEP_MAGNITUDE           1
#define AC_TASK_DELAY               10

#define NUM_ROLLOUT_GROUPS          3
#define NUM_MEMBERS_PER_RO_GROUP   (NUM_ACTUATORS/NUM_ROLLOUT_GROUPS)

#define NUM_HW_GROUPS               3
#define NUM_SERVOS_PER_HW_GROUP     16

typedef struct {
    //data type describing how motors are divided by pca chip
    uint8_t currentPos[NUM_ACTUATORS];
    uint8_t desiredPos[NUM_ACTUATORS];
    ACMode_e mode;
} ActControl_t;

ActControl_t actControl;

const PCA9685_t hwGroups[NUM_HW_GROUPS] = {
    { .addr = 0x64, .isLed = true,  .osc_freq = 25000000.0 },
    { .addr = 0x61, .isLed = false, .osc_freq = 25000000.0 },
    { .addr = 0x42, .isLed = false, .osc_freq = 25000000.0 },
};

uint8_t rolloutGroups[NUM_ROLLOUT_GROUPS][NUM_MEMBERS_PER_RO_GROUP];

void init_rollout_groups(void)
{
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        uint8_t group = i / NUM_MEMBERS_PER_RO_GROUP;
        uint8_t member = i % NUM_MEMBERS_PER_RO_GROUP;
        rolloutGroups[group][member] = i;
    }
}

bool calculate_next_position(void)
{
    bool didPositionChange = false;
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        int deltaTotal = actControl.desiredPos[i] - actControl.currentPos[i];
        
        if (deltaTotal != 0)
        {
            didPositionChange = true;
            int step = (abs(deltaTotal) < AC_STEP_MAGNITUDE) ? deltaTotal : (deltaTotal / abs(deltaTotal)) * AC_STEP_MAGNITUDE;
            actControl.currentPos[i] += step;
        }
    }

    return didPositionChange;
}

void rollout_actuator_positions(void)
{
    for (uint8_t group = 0; group < NUM_ROLLOUT_GROUPS; group++)
    {
        for (uint8_t member = 0; member < NUM_MEMBERS_PER_RO_GROUP; member++)
        {
            // between 0-44, actual position of the motor on the course
            uint8_t absoluteServoId = rolloutGroups[group][member];
            
            uint8_t relativeServoId = absoluteServoId % NUM_SERVOS_PER_HW_GROUP;
            uint8_t hwGroup = absoluteServoId / NUM_SERVOS_PER_HW_GROUP;

            PCA9685_setServoPos(&hwGroups[hwGroup], relativeServoId, actControl.currentPos[absoluteServoId]);
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

}

void AC_init(void)
{
    init_rollout_groups();

    actControl.mode = STATIC;
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        actControl.currentPos[i] = 0;
        actControl.desiredPos[i] = 0;
    }

    for (uint8_t i = 0; i < NUM_HW_GROUPS; i++)
    {
        PCA9685_init(&hwGroups[i]);
    }

    // EXPLICITLY SET EACH MOTOR TO POSITION 0 ON STARTUP TO ENSURE WE ARE IN A DEFINED POSITION
}

void AC_run_task(void)
{
    //calculate next positions based on current and desired position
    bool didPositionsChange = calculate_next_position();

    //don't run anything else if the new positions are the same as the old positions
    if (didPositionsChange)
    {
        //execute rollout of the new positions
        rollout_actuator_positions();
    }

    //delay
    vTaskDelay(20 / portTICK_PERIOD_MS);

}

void AC_update_desired_positions(uint8_t desiredPos[NUM_ACTUATORS])
{
    uint8_t offset = 0;
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        // skip indices 0, 16, and 32
        if (i % 16 == 0)
        {
            offset++;
            continue;
        }

        actControl.desiredPos[i] = desiredPos[i - offset];
    }

    // print the positions
    printf("Positions: ");
    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        printf("%d ", actControl.desiredPos[i]);
    }
    printf("\n");
}

void AC_update_mode(ACMode_e mode)
{
    actControl.mode = mode;
}