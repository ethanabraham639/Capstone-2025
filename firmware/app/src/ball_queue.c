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

            //if prox sensor activated
            state = DISPENSING;
            break;
        
        case DISPENSING:
            //dispensing sensor activated
            state = WAITING;

            //5 second timeout
            state = FAILED;
            break;
        
        case FAILED:
            break;
    }
}