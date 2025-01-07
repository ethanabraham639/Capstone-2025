#include "adc.h"
#include "gpio.h"

#include "driver/adc.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MUX_SOURCE_MASK     0x07
#define T_ON_WAIT           (2/portTICK_RATE_MS) //ms

static const char *TAG = "ADC";

typedef enum {
    BD_SENSE = 0,
    LV_SENSE = 7,
} ADCSource_e;

void setMuxInput(ADCSource_e source);
uint16_t genericADCRead(void);


void setMuxInput(ADCSource_e source)
{
    const uint8_t muxGpioMask = ((uint8_t)source) & MUX_SOURCE_MASK;
    
    const uint8_t muxA0GpioLevel = (muxGpioMask & 0x01) >> 0;
    const uint8_t muxA1GpioLevel = (muxGpioMask & 0x02) >> 1;
    const uint8_t muxA2GpioLevel = (muxGpioMask & 0x04) >> 2;

    gpio_set_level(ADC_MUX_A0_GPIO, muxA0GpioLevel);
    gpio_set_level(ADC_MUX_A1_GPIO, muxA1GpioLevel);
    gpio_set_level(ADC_MUX_A2_GPIO, muxA2GpioLevel);

    vTaskDelay(T_ON_WAIT); // wait for input to switch, just in case
}

uint16_t genericADCRead(void)
{
    uint16_t ret = 0;
    
    uint16_t adc_data = 0;
    if (adc_read(&adc_data) == ESP_OK)
    {
        ret = adc_data;
    }
    else
    {
        ESP_LOGI(TAG, "ADC read failed\r\n");
        ret = 0;
    }

    return ret;
}


void ADC_init(void)
{
    adc_config_t adc_config;

    adc_config.mode = ADC_READ_TOUT_MODE;
    adc_config.clk_div = 8; // ADC sample collection clock = 80MHz/clk_div = 10MHz
    
    setMuxInput(LV_SENSE); // set default input
    
    ESP_ERROR_CHECK(adc_init(&adc_config));
}

/**
 * There is only 1 ADC, we must mux in the input first then read
 */
uint16_t ADC_getLvADCVal(void)
{
    setMuxInput(LV_SENSE);

    return genericADCRead();
}

uint16_t ADC_getBDADCVal(void)
{
    setMuxInput(BD_SENSE);
    
    return genericADCRead();

}