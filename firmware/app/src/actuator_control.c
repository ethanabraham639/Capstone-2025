#include "actuator_control.h"

typedef enum {
    RESET = 0,
    STATIC,
    DYNAMIC_PRESET_1,
    DYNAMIC_PRESET_2,
    DYNAMIC_PRESET_3,
    RANDOM
} ActControlScheme_e;

typedef struct {
    
    ActControlScheme_e controlScheme;
    uint8_t actPositions[NUM_ACTUATORS];

} ActControl_t;

ActControl_t actControl;

void select_actuator_control_scheme(void)
{

}

void calculate_actuator_positions(void)
{

}

void apply_actuator_positions(void)
{

}

void run_actuator_control_task(void)
{
    /**
     * Step 1: Mode select
     *  - determines the motor control scheme
     * 
     * Step 2: Calculate actuator positions
     *  - based on the selected mode, calculate the actuator positions and populate the buffer
     *  - must consider the time it takes to most the servo motors in percentage/sec
     * 
     * Step 3: Apply actuator positions
     *  - call the pwm driver for the motors
     */

    select_actuator_control_scheme();

    calculate_actuator_positions();

    apply_actuator_positions();

    //delay for some time
}
