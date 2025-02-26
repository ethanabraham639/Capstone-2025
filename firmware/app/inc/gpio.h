#ifndef GPIO_H
#define GPIO_H

#include "driver/gpio.h"

#define UNUSED 999

#define ADC_MUX_A0_GPIO     UNUSED
#define ADC_MUX_A1_GPIO     UNUSED
#define ADC_MUX_A2_GPIO     UNUSED

#define BIH_GPIO_IN         GPIO_NUM_4
#define BIG_GPIO_IN         GPIO_NUM_5

#define BD_GPIO_IN          GPIO_NUM_12
#define BQ_GPIO_IN          GPIO_NUM_13
#define LED_GPIO_OUT        GPIO_NUM_15
#define BALL_DQ_GPIO_OUT    GPIO_NUM_16

#define GPIO_HIGH           1
#define GPIO_LOW            0

void GPIO_init(void);

#endif