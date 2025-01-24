#ifndef SENSORS_H
#define SENSORS_H

#include <stdbool.h>

void SNS_init(void);

bool SNS_get_ball_in_hole(void);
void SNS_clear_ball_in_hole(void);

bool SNS_get_ball_in_gutter(void);
void SNS_clear_ball_in_gutter(void);

bool SNS_get_ball_dep(void);
void SNS_clear_ball_dep(void);

bool SNS_get_ball_queue(void);
void SNS_clear_ball_queue(void);

void SNS_run_task(void);

#endif