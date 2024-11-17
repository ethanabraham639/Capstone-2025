#include "ball_estimation.h"

BallEstimationState_e state = IDLE;

void run_ball_estimation_task(void)
{
    switch(state)
    {
        case IDLE:
            // only if auto dispense is on, run this state machine.
            state = READY_TO_HIT;

            // else 
            state = NO_ESTIMATION_TRACKING;
            break;

        case NO_ESTIMATION_TRACKING:
            
            //if auto dispense changes
            state = IDLE;
        
        case READY_TO_HIT:
            //if sensor detects ball broke
            state = IN_TRANSIT;
            break;
        
        case IN_TRANSIT:
            //if ball goes in the hole
            state = IN_HOLE;

            //if ball goes in the gutter
            state = IN_GUTTER;

            //if stuck after timeout
            state = STUCK;

            break;
        
        case IN_HOLE:
            //notify app

            //if the ball goes into the gutter (it should)
            state = IN_GUTTER;

            break;

        case IN_GUTTER:
            //notify app

            state = READY_TO_HIT;

            break;

        case STUCK:
            // ball is stuck, ready to hit
            state = READY_TO_HIT;
            break;
    }
}