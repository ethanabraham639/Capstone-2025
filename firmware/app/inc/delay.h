#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>
#include <bool.h>

typedef Timer_t int64_t;

int64_t TIMER_start(Timer_t* timer);
int64_t TIMER_get_ms(Timer_t* timer);
int64_t TIMER_get_us(Timer_t* timer);
void TIMER_restart(Timer_t* timer);

#endif