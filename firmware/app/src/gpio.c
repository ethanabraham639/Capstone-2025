#include "gpio.h"

#define PIN_MASK(PIN_NUM) (1 << (PIN_NUM))

gpio_config_t gpios[] = 
{
    /*Flashing*///   {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_0),  .pull_down_en = 0, .pull_up_en = 0},

    /* Mux A0 */     {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_16), .pull_down_en = 0, .pull_up_en = 0},
    /* Mux A1 */     {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_12), .pull_down_en = 0, .pull_up_en = 0},
    /* Mux A2 */     {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_13), .pull_down_en = 0, .pull_up_en = 0},

    /* LED0 */       {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_5),  .pull_down_en = 0, .pull_up_en = 0},
    /* LED1 */       {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_4),  .pull_down_en = 0, .pull_up_en = 0},
    /* LED2 */       {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_9),  .pull_down_en = 0, .pull_up_en = 0},

    /* NULL */       {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = PIN_MASK(GPIO_NUM_15), .pull_down_en = 0, .pull_up_en = 0},

    /* BIH LS */     {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_8),  .pull_down_en = 0, .pull_up_en = 0},
    /* BD LS */      {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_6),  .pull_down_en = 0, .pull_up_en = 0},
    /* BQ_LS */      {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_1),  .pull_down_en = 0, .pull_up_en = 0},
    /* Gutter1 LS */ {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_7),  .pull_down_en = 0, .pull_up_en = 0},
    /* Gutter2 LS */ {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_INPUT,  .pin_bit_mask = PIN_MASK(GPIO_NUM_10), .pull_down_en = 0, .pull_up_en = 0},


};

void GPIO_init(void)
{
    for (uint8_t i = 0; i < sizeof(gpios)/sizeof(gpio_config_t); i++)
    {
        gpio_config(&gpios[i]);
    }
}