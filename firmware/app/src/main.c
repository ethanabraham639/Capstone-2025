#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "pca9685.h"
#include "i2c.h"
#include "wifi_init.h"
#include "adc.h"
#include "gpio.h"

#define BLINKY_GPIO 5
PCA9685_t led_pca9685 = {.addr = 0x64, .isLed = true, .osc_freq = 25000000.0};
PCA9685_t servo_pca9685 = {.addr = 0x61, .isLed = false, .osc_freq = 25000000.0};

void init_peripherals(void)
{
    GPIO_init();
    ADC_init();
}

void app_main()
{
    I2C_master_init();
    init_peripherals();
    
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP8266 chip with %d CPU cores, WiFi, ",
            chip_info.cores);

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    WIFI_init_and_start_server();

    bool isOn = false;
    while(1) {
        gpio_set_level(BLINKY_GPIO, isOn);
        isOn = !isOn;
        uint8_t lv = (((float)ADC_getLvADCVal())/1023.0f) * 100;
        uint8_t bd = (((float)ADC_getBDADCVal())/1023.0f) * 100;
        printf("LV: %d\n", lv);
        printf("Test: %d\n", bd);
        vTaskDelay(250 / portTICK_PERIOD_MS); // Delay for smooth motion
    }

}
