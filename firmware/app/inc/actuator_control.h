#ifndef ACTUATOR_CONTROL_H
#define ACTUATOR_CONTROL_H

#include "stdint.h"

#define NUM_ACTUATORS   45
#define MODES_SIZE      1

void updateActPositions(uint8_t* positionArray);
void resetActPositions(void);
#endif