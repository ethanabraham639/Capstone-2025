#include "sensors.h"

#define BALL_HOLE_SNS_GPIO      4
#define GUTTER_SNS_GPIO         5
#define BALL_DEP_SNS_GPIO       12
#define BALL_QUEUE_SNS_GPIO     13

#define LASER_EMITTER_GPIO      16

typedef struct {
    bool ballInHole;
    bool ballInGutter;
    bool ballDep;
    bool ballQueue;
} Sensors_t;

void check_ball_in_hole(void)
{

}

void check_ball_in_gutter(void)
{

}

void check_ball_dep(void)
{

}

void check_ball_queue(void)
{

}



void SNS_init(void)
{

}

bool SNS_get_ball_in_hole(void)
{

}

void SNS_clear_ball_in_hole(void)
{

}

bool SNS_get_ball_in_gutter(void)
{

}

void SNS_clear_ball_in_gutter(void)
{

}

bool SNS_get_ball_dep(void)
{

}

void SNS_clear_ball_dep(void)
{

}

bool SNS_get_ball_queue(void)
{

}

void SNS_clear_ball_queue(void)
{

}