#ifndef ACTUATOR_CONTROL_H
#define ACTUATOR_CONTROL_H

#include "stdint.h"

#define NUM_ACTUATORS   45
#define MODES_SIZE      1

typedef enum {
    RESET = 0,
    STATIC,
} ACMode_e;

void AC_init(void);
void AC_run_task(void);
void AC_update_desired_positions(uint8_t desiredPos[NUM_ACTUATORS]);
void AC_update_mode(ACMode_e mode);

#endif