#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "pca9685.h"
#include "i2c.h"
#include "wifi_init.h"
#include "actuator_control.h"
#include "sensors.h"
#include "ball_estimation.h"
#include "ball_queue.h"

static void task_1ms(void* arg);
static void task_10ms(void* arg);
static void task_100ms(void* arg);

void app_main()
{
    WIFI_init_and_start_server();
    I2C_master_init();
    GPIO_init();
    
    AC_init();
    BQ_init();
    SNS_init();

    xTaskCreate(task_1ms,   "task_1ms",   2048, NULL, 10, NULL);
    xTaskCreate(task_10ms,  "task_10ms",  2048, NULL, 10, NULL);
    xTaskCreate(task_100ms, "task_100ms", 2048, NULL, 10, NULL);
}

static void task_1ms(void* arg)
{
    for (;;)
    {
        SNS_run_task();

        vTaskDelay(1 / portTICK_RATE_MS);
    }
}

static void task_10ms(void* arg)
{
    for (;;)
    {
        AC_run_task();
        BE_run_task();

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

static void task_100ms(void* arg)
{
    for (;;)
    {
        BQ_run_task();

        vTaskDelay(100 / portTICK_RATE_MS);
    }
}