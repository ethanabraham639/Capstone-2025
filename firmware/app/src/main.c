#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "pca9685.h"
#include "i2c.h"
#include "wifi_init.h"

#define BLINKY_GPIO 5
PCA9685_t led_pca9685 = {.addr = 0x64, .isLed = true, .osc_freq = 25000000.0};
PCA9685_t servo_pca9685 = {.addr = 0x61, .isLed = false, .osc_freq = 25000000.0};

void init_peripherals(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = 1<<BLINKY_GPIO;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    PCA9685_init(&led_pca9685);
    PCA9685_init(&servo_pca9685);

    for (uint8_t i = 1; i < 16; i++)
    {
        PCA9685_setServoPos(&led_pca9685, i, 0);
    }

    for (uint8_t i = 1; i < 16; i++)
    {
        PCA9685_setServoPos(&servo_pca9685, i, 0);
    }
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

        // Gradually move from 0 to 255
        for (int pos = 0; pos <= 255; pos++)
        {
            for (uint8_t i = 6; i < 11; i++)
            {
                PCA9685_setServoPos(&servo_pca9685, i, pos); // Set servo position
                PCA9685_setServoPos(&led_pca9685, i, pos); // Set servo position
            }
            vTaskDelay(20 / portTICK_PERIOD_MS); // Delay for smooth motion
        }

        // Gradually move from 255 back to 0
        for (int pos1 = 255; pos1 > 0; pos1--)
        {
            for (uint8_t i = 6; i < 11; i++)
            {
                PCA9685_setServoPos(&servo_pca9685, i, pos1); // Set servo position
                PCA9685_setServoPos(&led_pca9685, i, pos1); // Set servo position
            }
            vTaskDelay(20 / portTICK_PERIOD_MS); // Delay for smooth motion
        }

    }

}
