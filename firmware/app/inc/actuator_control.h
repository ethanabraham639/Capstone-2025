#ifndef ACTUATOR_CONTROL_H
#define ACTUATOR_CONTROL_H

#include "stdint.h"

#define NUM_ACTUATORS 45

void updateActuatorPositions(uint8_t* positionArray);
void resetAcuatorPositions(void);
#endif