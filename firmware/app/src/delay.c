#include "delay.h"
#include "esp_timer.h"

int64_t TIMER_restart()
{
    return esp_timer_get_time();
}

int64_t TIMER_get_ms(Timer_t timer)
{
    return round((esp_timer_get_time() - timer) / 1000.0f);
}

int64_t TIMER_get_us(Timer_t timer)
{
    return (esp_timer_get_time() - timer);
}