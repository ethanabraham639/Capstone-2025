#ifndef GPIO_H
#define GPIO_H

#include "driver/gpio.h"

#define ADC_MUX_A0_GPIO      GPIO_NUM_16
#define ADC_MUX_A1_GPIO      GPIO_NUM_12
#define ADC_MUX_A2_GPIO      GPIO_NUM_13

#define LED0_GPIO           GPIO_NUM_5
#define LED1_GPIO           GPIO_NUM_4
#define LED2_GPIO           GPIO_NUM_9

#define BIH_LS_GPIO         GPIO_NUM_8
#define BD_LS_GPIO          GPIO_NUM_6
#define BQ_LS_GPIO          GPIO_NUM_1

#define GUTTER1_GPIO        GPIO_NUM_7
#define GUTTER2_GPIO        GPIO_NUM_10

#define GPIO_ON             0
#define GPIO_OFF            1

void GPIO_init(void);

#endif