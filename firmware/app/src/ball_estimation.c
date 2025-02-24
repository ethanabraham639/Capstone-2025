#include "ball_estimation.h"

#include <stdint.h>
#include <stdbool.h>

#include "sensors.h"
#include "delay.h"
#include "ball_queue.h"
#include "esp_log.h"

#define TAG "BALL_ESTIMATION.C"

#define IN_TRANSIT_TIMEOUT_MS 5000 // how long we allow the ball to be in transit before we consider it stuck on the field
#define FEED_ERROR_TIMEOUT_MS 7000 // how long we give the ball to travel from the hole to the gutter via the ball in hole return mechanism

#define RETURN_ONE_BALL 1

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

void in_transit_on_enter_state(void);
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
    // check if auto dispense is turned on
    if (BE.autoDispense)
    {
        BE.state = READY_TO_HIT_on_enter;
    }
    // else, continue with no estimation tracking
    else
    {
        if (SNS_get_ball_dep())
        {
            BE.ballsHit++;
            SNS_clear_ball_dep();
    
            ESP_LOGI(TAG, "Ball departure detected");
        }
    
        if (SNS_get_ball_in_hole())
        {
            BE.ballsInHole++;
            SNS_clear_ball_in_hole();
    
            ESP_LOGI(TAG, "Ball in hole detected");
    
            BQ_request_ball_in_hole_return();
    
            if (BE.ballsInHole > BE.ballsHit)
            {
                BE.ballsInHole = BE.ballsHit;
                ESP_LOGE(TAG, "Balls in hole greater than balls hit, impossible! Equating to balls hit");
            }
        }
    }

}

void ready_to_hit_on_enter_state(void)
{
    //clear all sensors to ignore stray balls
    SNS_clear_ball_dep();
    SNS_clear_ball_queue();
    
    // check if auto dispense was turned off
    if (BE.autoDispense == false)
    {
        BE.state = NO_ESTIMATION_TRACKING;   
    }
    // else continue to ball estimation tracking
    else
    {
        BQ_request_player_return(RETURN_ONE_BALL);
        
        BE.state = READY_TO_HIT;
    }

}

void ready_to_hit_state(void)
{
    if (SNS_get_ball_dep())
    {
        BE.ballsHit++;
        SNS_clear_ball_dep();
        BE.inTransitTimer = TIMER_restart();

        ESP_LOGI(TAG, "Ball departure detected");

        BE.state = IN_TRANSIT_on_enter;
    }
}

void in_transit_on_enter_state(void)
{
    SNS_clear_ball_in_hole();
    SNS_clear_ball_in_gutter();

    BE.state = IN_TRANSIT;
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
        /**
         * Purposely don't clear the ball in gutter flag, it is how we tell the ball went straight into the gutter and did not
         * come from the ball in hole return mechanism. Therefore, there is no need to check for a feed error. The ball in hole
         * state will clear the flag.
         */

        // resetting the timer to make sure it doesn't timeout in the gutter state
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
    BE.feedErrorTimer = TIMER_restart();

    ESP_LOGI(TAG, "Ball in hole detected");

    BE.state = IN_GUTTER;
}

void in_gutter_state(void)
{
    /**
     * If the ball came directly from in transit, this will be true immediately since the flag was not cleared
     * If the ball came from in the hole, this will take some time
     */
    if (SNS_get_ball_in_gutter())
    {
        ESP_LOGI(TAG, "Ball in gutter detected");
        
        SNS_clear_ball_in_gutter();
        BE.state = READY_TO_HIT_on_enter;
        
        return;
    }

    if (TIMER_get_ms(BE.feedErrorTimer) > FEED_ERROR_TIMEOUT_MS)
    {
        ESP_LOGE(TAG, "Ball in hole feed error to gutter, continuing to READY_TO_HIT anyways"); 
        BE.state = READY_TO_HIT_on_enter;
    }
}

void stuck_state(void)
{
    ESP_LOGI(TAG, "Ball stuck on field, continuing to READY_TO_HIT");
    BE.state = READY_TO_HIT_on_enter;
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
        case IN_TRANSIT_on_enter:
            in_transit_on_enter_state();
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