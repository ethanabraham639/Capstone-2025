#ifndef BALL_QUEUE_H
#define BALL_QUEUE_H

BQ_init(void);

BQ_request_ball_in_hole_return(void);
BQ_request_player_return(uint8_t ball_count);

BQ_run_task(void);

#endif