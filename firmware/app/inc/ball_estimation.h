#ifndef BALL_ESTIMATION_H
#define BALL_ESTIMATION_H

typedef enum {
    IDLE = 0,
    READY_TO_HIT_on_enter,
    READY_TO_HIT,
    IN_TRANSIT,
    IN_HOLE,
    STUCK,
    IN_GUTTER,
    NO_ESTIMATION_TRACKING,
} BallEstState_e;

void BE_reset_stats(void);
uint8_t BE_get_balls_hit(void);
uint8_t BE_get_balls_in_hole(void);
void BE_set_auto_dispense(bool autoDispense);

void BE_run_task(void);

#endif