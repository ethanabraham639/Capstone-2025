#include "delay.h"

int64_t TIMER_start(Timer_t* timer)
{
    return esp_timer_get_time();
}

int64_t TIMER_get_ms(Timer_t* timer)
{
    return round((esp_timer_get_time() - &timer) / 1000.0f);
}

int64_t TIMER_get_us(Timer_t* timer)
{
    return (esp_timer_get_time() - &timer);
}

void TIMER_restart(Timer_t* timer)
{
    &timer = esp_timer_get_time();
}