#include "gpio.h"

#define PIN_MASK(PIN_NUM) (1 << (PIN_NUM))

gpio_config_t gpios[] = 
{
    /* BIH LS */        {.intr_type = GPIO_INTR_NEGEDGE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_4),  .pull_down_en = 0, .pull_up_en = 0},
    /* Gutter LS */     {.intr_type = GPIO_INTR_NEGEDGE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_5),  .pull_down_en = 0, .pull_up_en = 0},

    /* BD Laser */      {.intr_type = GPIO_INTR_NEGEDGE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_12), .pull_down_en = 0, .pull_up_en = 0},
    /* BQ Laser */      {.intr_type = GPIO_INTR_NEGEDGE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_13), .pull_down_en = 0, .pull_up_en = 0},

    /* LED */           {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_15), .pull_down_en = 0, .pull_up_en = 0},

    /* Laser Emitter */ {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_16), .pull_down_en = 0, .pull_up_en = 0},
};

void GPIO_init(void)
{
    for (uint8_t i = 0; i < sizeof(gpios)/sizeof(gpio_config_t); i++)
    {
        gpio_config(&gpios[i]);
    }
}