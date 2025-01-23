#include "ball_estimation.h"

#include <stdint.h>
#include <stdbool.h>

#include "sensors.h"
#include "delay.h"
#include "ball_queue.h"

#define IN_TRANSIT_TIMEOUT_MS 5000
#define FEED_ERROR_TIMEOUT_MS 7000

typedef struct{
    uint8_t ballsHit;
    uint8_t ballsInHole;

    Timer_t inTransitTimer;
    Timer_t feedErrorTimer;

    bool autoDispense;

    BallEstState_e state;
} BallEst_t;

BallEst_t BE = { .ballsHit = 0, .ballsInHole = 0, .inTransitTimer = 0, .feedErrorTimer = 0, .autoDispense = false, .state = IDLE };

void idle_state(void);
void no_estimation_tracking_state(void);
void ready_to_hit_on_enter_state(void);
void ready_to_hit_state(void);
void in_transit_state(void);
void in_hole_state(void);
void in_gutter_state(void);
void stuck_state(void);


void idle_state(void)
{
    if (BE.autoDispense)
    {
        BE.state = READY_TO_HIT_on_enter;
    }
    else
    {
        BE.state = NO_ESTIMATION_TRACKING;
    }
}

void no_estimation_tracking_state(void)
{
    if (BE.autoDispense)
    {
        BE.state = READY_TO_HIT_on_enter;
        return;
    }

    if (SNS_get_ball_dep())
    {
        BE.ballsHit++;
        SNS_clear_ball_dep();
    }

    if (SNS_get_ball_in_hole())
    {
        BE.ballsInHole++;
        SNS_clear_ball_in_hole();

        BQ_request_ball_in_hole_return();

        if (BE.ballsInHole > BE.ballsHit)
        {
            BE.ballsInHole = BE.ballsHit;
        }
    }
}

void ready_to_hit_on_enter_state(void)
{
    BQ_request_player_return(1);
    BE.state = READY_TO_HIT;
}

void ready_to_hit_state(void)
{
    if (SNS_get_ball_dep())
    {
        BE.ballsHit++;
        SNS_clear_ball_dep();
        BE.inTransitTimer = TIMER_restart();

        BE.state = IN_TRANSIT;
    }
}

void in_transit_state(void)
{
    if (SNS_get_ball_in_hole())
    {
        BE.ballsInHole++;
        SNS_clear_ball_in_hole();
        BE.state = IN_HOLE;
        return;
    }

    if (SNS_get_ball_in_gutter())
    {
        BE.feedErrorTimer = TIMER_restart();
        BE.state = IN_GUTTER;
        return;
    }

    if (TIMER_get_ms(BE.inTransitTimer) > IN_TRANSIT_TIMEOUT_MS)
    {
        BE.state = STUCK;
    }
}

void in_hole_state(void)
{
    BQ_request_ball_in_hole_return();
    BE.state = IN_GUTTER;
}

void in_gutter_state(void)
{
    if (SNS_get_ball_in_gutter())
    {
        SNS_clear_ball_in_gutter();
        BE.state = READY_TO_HIT;
        return;
    }

    if (TIMER_get_ms(BE.feedErrorTimer) > FEED_ERROR_TIMEOUT_MS)
    {
        BE.state = READY_TO_HIT;
    }
}

void stuck_state(void)
{
    BE.state = READY_TO_HIT;
}


void BE_run_task(void)
{
    switch (BE.state)
    {
        case IDLE:
            idle_state();
            break;
        case NO_ESTIMATION_TRACKING:
            no_estimation_tracking_state();
            break;
        case READY_TO_HIT_on_enter:
            ready_to_hit_on_enter_state();
            break;
        case READY_TO_HIT:
            ready_to_hit_state();
            break;
        case IN_TRANSIT:
            in_transit_state();
            break;
        case IN_HOLE:
            in_hole_state();
            break;
        case IN_GUTTER:
            in_gutter_state();
            break;
        case STUCK:
            stuck_state();
            break;
    }
}

void BE_reset_stats(void)
{
    BE.ballsHit = 0;
    BE.ballsInHole = 0;
}

uint8_t BE_get_balls_hit(void)
{
    return BE.ballsHit;
}

uint8_t BE_get_balls_in_hole(void)
{
    return BE.ballsInHole;
}

void BE_set_auto_dispense(bool autoDispense)
{
    BE.autoDispense = autoDispense;
}