#ifndef USER_NVS_H
#define USER_NVS_H

#include "actuator_control.h"
#include "esp_err.h"

void NVS_init(void);
esp_err_t NVS_write_course_state(uint8_t courseState[NUM_ACTUATORS]);
esp_err_t NVS_read_course_state(uint8_t output[NUM_ACTUATORS]);

#endif