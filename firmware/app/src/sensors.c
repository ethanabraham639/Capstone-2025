#include "sensors.h"
#include "gpio.h"
#include "delay.h"

#define DEBOUNCE_DELAY_MS       10

typedef struct{
    gpio_num_t gpio;
    int confirmed_level;

    bool detected;
    bool confirmed;
    Timer_t timer;
} GpioSensor_t;

typedef struct {
    GpioSensor_t BIH;
    GpioSensor_t BIG;
    GpioSensor_t BD;
    GpioSensor_t BQ;
} Sensors_t;

volatile Sensors_t sensors = {.BIH.gpio = BIH_GPIO_IN, .BIH.confirmed_level = GPIO_LOW, .BIH.detected = false, .BIH.confirmed = false, .BIH.timer = 0,
                              .BIG.gpio = BIG_GPIO_IN, .BIG.confirmed_level = GPIO_LOW, .BIG.detected = false, .BIG.confirmed = false, .BIG.timer = 0,
                              .BD.gpio  = BD_GPIO_IN , .BD.confirmed_level  = GPIO_LOW, .BD.detected  = false, .BD.confirmed  = false, .BD.timer  = 0,
                              .BQ.gpio  = BQ_GPIO_IN, .BQ.confirmed_level   = GPIO_LOW, .BQ.detected  = false, .BQ.confirmed  = false, .BQ.timer  = 0};


// Common ISR
static void sensor_gpio_isr(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;

    switch (gpio_num)
    {
        case BIH_GPIO_IN:
            sensors.BIH.detected = true;
            TIMER_restart(&sensors.BIH.timer);
            break;

        case BIG_GPIO_IN:
            sensors.BIG.detected = true;
            TIMER_restart(&sensors.BIG.timer);
            break;

        case BD_GPIO_IN:
            sensors.BD.detected = true;
            TIMER_restart(&sensors.BD.timer);
            break;

        case BQ_GPIO_IN:
            sensors.BQ.detected = true;
            TIMER_restart(&sensors.BQ.timer);
            break;
    }
}

void generic_gpio_debounce_read(GpioSensor_t* sensor)
{
    if (sensor->detected && !sensor->confirmed)
    {
        int gpio_level = gpio_get_level(sensor->gpio);

        if (gpio_level == sensor->confirmed_level)
        {
            if (TIMER_get_ms(sensor->timer) > DEBOUNCE_DELAY_MS)
            {
                sensor->confirmed = true;
            }
        }
        else
        {
            sensor->detected = false;
        }
    }
}

void check_ball_in_hole(void)
{
    generic_gpio_debounce_read(&sensors.BIH);
}

void check_ball_in_gutter(void)
{
    generic_gpio_debounce_read(&sensors.BIG);
}

void check_ball_dep(void)
{
    generic_gpio_debounce_read(&sensors.BD);
}

void check_ball_queue(void)
{
    generic_gpio_debounce_read(&sensors.BQ);
}

void SNS_init(void)
{
    // initially read all gpios and make sure its accurate

}

bool SNS_get_ball_in_hole(void)
{
    return sensors.BIH_confirmed;
}

void SNS_clear_ball_in_hole(void)
{
    sensors.BIH_confirmed = false;
    sensors.BIH_detected = false;
}

bool SNS_get_ball_in_gutter(void)
{
    return sensors.BIG_confirmed;
}

void SNS_clear_ball_in_gutter(void)
{
    sensors.BIG_confirmed = false;
    sensors.BIG_detected = false;
}

bool SNS_get_ball_dep(void)
{
    return sensors.BD_confirmed;
}

void SNS_clear_ball_dep(void)
{
    sensors.BD_confirmed = false;
    sensors.BD_detected = false;
    }

bool SNS_get_ball_queue(void)
{
    return sensors.BQ_confirmed;
}

void SNS_clear_ball_queue(void)
{
    sensors.BQ_confirmed = false;
    sensors.BQ_detected = false;
}

void SNS_run_task(void)
{
    check_ball_in_hole();
    check_ball_in_gutter();
    check_ball_dep();
    check_ball_queue();
    //DELAY
}