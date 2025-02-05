#ifndef BALL_QUEUE_H
#define BALL_QUEUE_H

#include <stdint.h>

void BQ_init(void);

void BQ_request_ball_in_hole_return(void);
void BQ_request_player_return(uint8_t ball_count);

void BQ_run_task(void);

#endif