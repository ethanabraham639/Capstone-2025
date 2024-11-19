#include "actuator_control.h"
#include "pca9685.h"

#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define MAX_POSITION            255.0
#define MAX_DELAY_MS            2000.0
#define DELTA_CONVERSION_MS     (MAX_DELAY_MS/MAX_POSITION)

#define NUM_PCA9685             3
#define NUM_ACT_PER_PCA6985     15

typedef enum {
    RESET = 0,
    STATIC,
    DYNAMIC_PRESET_1,
    DYNAMIC_PRESET_2,
    DYNAMIC_PRESET_3,
    RANDOM
} ActControlScheme_e;

typedef struct {
    PCA9685_t pca9685[NUM_PCA9685];
    ActControlScheme_e controlScheme;
    bool isNewPositions;
    uint8_t actPositions[NUM_ACTUATORS];



    //static
    uint8_t staticPositions[NUM_ACTUATORS];

    //for all
    uint32_t delay; //ms

} ActControl_t;

ActControl_t actControl;


uint32_t calculate_delay_ms(uint8_t* pos1, uint8_t* pos2)
{
    //find the largest delta
    uint8_t largestDelta = 0;

    for (uint8_t i = 0; i < NUM_ACTUATORS; i++)
    {
        uint8_t cur_pos1 = pos1[i];
        uint8_t cur_pos2 = pos2[i];

        // subtract smaller number from bigger
        uint8_t delta = (cur_pos1 > cur_pos2) ? (cur_pos1 - cur_pos2) : (cur_pos2 - cur_pos1);

        if (delta > largestDelta)
        {
            largestDelta = delta;
        }
    }

    return (uint32_t)((float)largestDelta * DELTA_CONVERSION_MS);
}











void calculate_reset_positions(void)
{
    //set everything to 0 for now
    actControl.delay = 0;
    memset(actControl.actPositions, 0, NUM_ACTUATORS);
}

void calculate_static_positions(void)
{
    actControl.delay = calculate_delay_ms(actControl.actPositions, actControl.staticPositions);
    memcpy(actControl.staticPositions, actControl.actPositions, NUM_ACTUATORS);
}
















void calculate_and_set_actuator_positions(void)
{
    switch(actControl.controlScheme)
    {
        case(RESET):
            calculate_reset_positions();
            break;
    
        case(STATIC):
            calculate_static_positions();
            break;
    }
}



/**
 * - we have 45 motors
 * - each row is 5 motors
 * 
 * - each i2c pwm module supports 15 motors
 * 
 * - lets assume this is the configuration wired up
 * 
 *           
 *          35
 * 
 *          30
 * 
 *          25
 * 
 *          20
 * 
 *          15        
 *  
 *          10
 * 
 *          5   
 *           
 *          0   1   2   3   4
 * 
 */

void apply_actuator_positions(void)
{
    for (uint8_t pca9685Index = 0; pca9685Index < NUM_PCA9685; pca9685Index++)
    {
        PCA9685_t* pca9685Instance = &actControl.pca9685[pca9685Index];

        for (uint8_t actIndex = 0; actIndex < NUM_ACT_PER_PCA6985; actIndex++)
        {
            uint8_t servoPos = actControl.actPositions[pca9685Index*actIndex];
            PCA9685_setServoPos(pca9685Instance, actIndex, servoPos);
        }
    }


}

void run_actuator_control_task(void)
{
    calculate_and_set_actuator_positions();

    if (actControl.isNewPositions)
        apply_actuator_positions();

    // allow time for positions to be applied
    vTaskDelay(actControl.delay / portTICK_RATE_MS);
}
