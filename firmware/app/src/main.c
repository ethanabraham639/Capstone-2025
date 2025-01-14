#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "pca9685.h"
#include "i2c.h"
#include "wifi_init.h"
#include "actuator_control.h"

void app_main()
{
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    I2C_master_init();
    AC_init();
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP8266 chip with %d CPU cores, WiFi, ",
            chip_info.cores);

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    WIFI_init_and_start_server();

    while(1) {
        AC_run_task();
    }

}
