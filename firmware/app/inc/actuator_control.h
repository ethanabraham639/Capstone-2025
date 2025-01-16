#ifndef ACTUATOR_CONTROL_H
#define ACTUATOR_CONTROL_H

#include "stdint.h"

#define NUM_ACTUATORS           45
#define MODES_SIZE              1

// Enum of actuator control modes
typedef enum {
    RESET = 0,
    STATIC,
} ACMode_e;

/**
 * @brief Initializes the actuator control task
 */
void AC_init(void);

/**
 * @brief Runs the actuator control task
 */
void AC_run_task(void);

/**
 * @brief Updates the desired positions of the actuators
 * @param desiredPos Array of desired actuator positions
 */
void AC_update_desired_positions(uint8_t desiredPos[NUM_ACTUATORS]);

/**
 * @brief Updates the actuator control mode
 * @param mode Actuator control mode
 */
void AC_update_mode(ACMode_e mode);

#endif