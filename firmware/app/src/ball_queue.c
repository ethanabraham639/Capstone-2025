#include "ball_queue.h"

#include <bool.h>

#include "delay.h"
#include "sensors.h"

#define BIH_FEEDFORWARD_DELAY_MS 5000

typedef enum {
    IDLE = 0,
    WAITING,
    DISPENSING,
    FAILED
} BallQueueState_e;

typedef struct {
    BallQueueState_e BIH_return_state;
    bool BIH_request;
    Timer_t BIH_timer;
    int64_t BIH_current_delay;

    BallQueueState_e player_return_state;
    bool player_request;
    uint8_t player_ball_count;
} BallQueue_t;

BallQueue_t BQ = {.BIH_return_state    = IDLE, .BIH_request    = false, .BIH_timer = 0, .BIH_current_delay = 0;
                  .player_return_state = IDLE, .player_request = false, .player_ball_count = 0};


/**
 * Possible issues with this as we test:
 * - The entire sequence takes X amount of time to complete in a feedforward fasion
 * - What happens if another ball goes in the hole before that timer runs out?
 *      - We could just add the time required the machine has to be on for.
 *      - We could add a laser sensor to detect how many balls are backlogged.
 */
void run_ball_in_hole_return_task(void)
{
    switch (BQ.BIH_return_state)
    {
        case IDLE:
            // do nothing, immediately enter WAITING
            BQ.BIH_return_state = WAITING;
            break;
        
        case WAITING:

            if (BQ.BIH_request)
            {
                BQ.BIH_request = false;

                //START THE CONTINUOUS MOTOR
                BQ.BIH_timer = TIMER_restart();
                BQ.BIH_current_delay = BIH_FEEDFORWARD_DELAY_MS;
                
                BQ.BIH_return_state = DISPENSING
            }
            break;
        
        case DISPENSING:

            // In the case that we get another request while dispensing, increase the delay by the feedforward amount
            if (BQ.BIH_request)
            {
                BQ.BIH_current_delay += BIH_FEEDFORWARD_DELAY_MS;
                BQ.BIH_request = false;
            }

            if (TIMER_get_ms(BQ.BIH_timer) > BQ.BIH_current_delay)
            {
                // assumed we have dispensed a ball by this time
                //STOP THE CONTINUOUS MOTOR
                BQ.BIH_return_state = WAITING;
            }

            break;
    }
}

void run_ball_queue_task(void)
{
    switch (BQ.player_return_state)
    {
        case IDLE:
            // do nothing, immediately enter WAITING
            BQ.player_return_state = WAITING;
            break;
        
        case WAITING:
            
            if (BQ.player_request)
            {
                BQ.player_request = false;

                //START THE CONTINUOUS MOTOR
                BQ.player_return_state = DISPENSING;
            }

            break;
        
        case DISPENSING:
            
            if (SNS_get_ball_queue())
            {
                // confirms that we have read the signal and are ready for the next
                SNS_clear_ball_queue();
                BQ.player_ball_count--;
            }

            if (BQ.player_ball_count == 0)
            {
                //STOP THE CONTINOUS MOTOR
                BQ.player_return_state = WAITING;
            }
            
            break;
        
        case FAILED:
            break;
    }
}



















// void run_ball_queue_task(void)
// {
//     switch (state)
//     {
//         case IDLE:
//             //do nothing
//             state = WAITING;
//             break;
        
//         case WAITING:

//             //if prox sensor activated or ready to hit and auto dispense is on
//             //or manual dispense is activated
//             state = DISPENSING;
//             break;
        
//         case DISPENSING:

//             //dispense multiple balls if it is a manual dispense
//             state = DISPENSING;

//             //dispensing sensor activated
//             state = WAITING;

//             //5 second timeout
//             state = FAILED;
//             break;
        
//         case FAILED:
//             break;
//     }
// }








