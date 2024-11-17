#include "ball_queue.h"

typedef enum {
    IDLE = 0,
    WAITING,
    DISPENSING,
    FAILED
} BallQueueState_e;

BallQueueState_e state = IDLE;

void run_ball_queue_task(void)
{
    switch (state)
    {
        case IDLE:
            //do nothing
            state = WAITING;
            break;
        
        case WAITING:

            //if prox sensor activated or ready to hit and auto dispense is on
            //or manual dispense is activated
            state = DISPENSING;
            break;
        
        case DISPENSING:

            //dispense multiple balls if it is a manual dispense
            state = DISPENSING;

            //dispensing sensor activated
            state = WAITING;

            //5 second timeout
            state = FAILED;
            break;
        
        case FAILED:
            break;
    }
}