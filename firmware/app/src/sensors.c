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

Sensors_t sensors = {.ballInHole = false,
                     .ballInGutter = false,
                     .ballDep = false,
                     .ballQueue = false};

// All of these sensors are active LOW, so only set to true on the FALLING EDGE
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
    return sensors.ballInHole;
}

void SNS_clear_ball_in_hole(void)
{
    sensors.ballInHole = false;
}

bool SNS_get_ball_in_gutter(void)
{
    return sensors.ballInGutter;
}

void SNS_clear_ball_in_gutter(void)
{
    sensors.ballInGutter = false;
}

bool SNS_get_ball_dep(void)
{
    return sensors.ballDep;
}

void SNS_clear_ball_dep(void)
{
    sensors.ballDep = false;
}

bool SNS_get_ball_queue(void)
{
    return sensors.ballQueue;
}

void SNS_clear_ball_queue(void)
{
    sensors.ballQueue;
}

void SNS_run_task(void)
{
    check_ball_in_hole();
    check_ball_in_gutter();
    check_ball_dep();
    check_ball_queue();
    //DELAY
}