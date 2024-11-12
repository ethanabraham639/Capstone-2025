#include "gpio.h"
#include "driver/gpio.h"

#define PIN_MASK(PIN_NUM) (1 << (PIN_NUM))

#define ADC_MUX_A0      GPIO_NUM_16
#define ADC_MUX_A1      GPIO_NUM_12
#define ADC_MUX_A2      GPIO_NUM_13

gpio_config_t gpios[] = 
{
    {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_0),  .pull_down_en = 0, .pull_up_en = 0},

    {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_16), .pull_down_en = 0, .pull_up_en = 0},
    {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_12), .pull_down_en = 0, .pull_up_en = 0},
    {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_13), .pull_down_en = 0, .pull_up_en = 0},

};

void GPIO_init(void)
{
    for (uint8_t i = 0; i < sizeof(gpios)/sizeof(gpio_config_t); i++)
    {
        gpio_config(&gpios[i]);
    }
}