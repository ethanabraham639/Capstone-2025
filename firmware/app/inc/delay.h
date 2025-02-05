#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>
#include <stdbool.h>

typedef int64_t Timer_t;

int64_t TIMER_restart(void);
int64_t TIMER_get_ms(Timer_t timer);
int64_t TIMER_get_us(Timer_t timer);

#endif