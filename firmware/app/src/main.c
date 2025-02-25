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
#include "gpio.h"
#include "delay.h"
#include "user_nvs.h"

#define LED_BLINK_TIMER_MS      500

static void task_1ms(void* arg);
static void task_10ms(void* arg);
static void task_100ms(void* arg);

void app_main()
{
    WIFI_init_and_start_server();
    NVS_init(); // NVS_init must come before any other init that uses it

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
    TickType_t xLastWakeTime = xTaskGetTickCount(); // Get current tick count
    const TickType_t xFrequency = pdMS_TO_TICKS(1); // Convert 1ms to ticks
    
    Timer_t blink_timer;
    blink_timer = TIMER_restart();

    for (;;)
    {
        SNS_run_task();

        if (TIMER_get_ms(blink_timer) > LED_BLINK_TIMER_MS)
        {
            gpio_set_level(LED_GPIO_OUT, !gpio_get_level(LED_GPIO_OUT)); // Toggle GPIO15
            blink_timer = TIMER_restart();
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

static void task_10ms(void* arg)
{
    TickType_t xLastWakeTime = xTaskGetTickCount(); // Get current tick count
    const TickType_t xFrequency = pdMS_TO_TICKS(10); // Convert 10ms to ticks

    for (;;)
    {
        AC_run_task();
        BE_run_task();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

static void task_100ms(void* arg)
{
    TickType_t xLastWakeTime = xTaskGetTickCount(); // Get current tick count
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // Convert 100ms to ticks

    for (;;)
    {
        BQ_run_task();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}