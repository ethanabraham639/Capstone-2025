#ifndef BALL_ESTIMATION_H
#define BALL_ESTIMATION_H

typedef enum BallEstimationState_e{
    IDLE = 0,
    READY_TO_HIT,
    IN_TRANSIT,
    IN_HOLE,
    STUCK,
    IN_GUTTER,
    FEED_ERROR
};

#endif