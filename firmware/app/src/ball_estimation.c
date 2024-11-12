#include "ball_estimation.h"

BallEstimationState_e state = IDLE;

void run_ball_estimation_task(void)
{
    switch(state)
    {
        case IDLE:
            // Do nothing, go in to next state
            state = READY_TO_HIT;
            break;
        
        case READY_TO_HIT:
            //if sensor detects ball broke
            state = IN_TRANSIT;
            break;
        
        case IN_TRANSIT:
            //if ball goes in the hole
            state = IN_HOLE;

            //if ball goes in the gutter
            state = IN_GUTTER;

            //if ball is stuck (10 second timer of inactivity)
            state = STUCK;

            break;
        
        case IN_HOLE:
            //notify app

            //if the ball goes into the gutter (it should)
            state = IN_GUTTER;

            //if the ball does not make it to the gutter (10 second timer of inactivity)
            state = FEED_ERROR;
            
            break;

        case STUCK:
            //notify app

            //signal for the ball return sequence

            //if the ball goes into the gutter (it should)
            state = IN_GUTTER;

            //if the ball does not make it to the gutter (10 second timer of inactivity)
            state = FEED_ERROR;

            break;

        case IN_GUTTER:
            //notify app

            state = READY_TO_HIT;

            break;

        case FEED_ERROR:
            //do something... not sure what
            break;
    }
}