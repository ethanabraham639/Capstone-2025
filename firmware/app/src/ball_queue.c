#include "ball_queue.h"

#include <stdbool.h>

#include "delay.h"
#include "sensors.h"
#include "pca9685.h"
#include "esp_log.h"
#include "error_codes.h"

#define TAG "BALL_QUEUE.C"

#define BIH_FEEDFORWARD_DELAY_MS 5000
#define PBR_TIMEOUT_MS           5000


#define CW_SPEED                 68
#define STOP_SPEED               64
#define CCW_SPEED                60
#define CONT_SERVO_ID            0

typedef enum {
    CW,
    CCW
} Dir_e;

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
    Timer_t PBR_timer; // player ball return timer

} BallQueue_t;

BallQueue_t BQ = {.BIH_return_state    = IDLE, .BIH_request    = false, .BIH_timer = 0, .BIH_current_delay = 0,
                  .player_return_state = IDLE, .player_request = false, .player_ball_count = 0, .PBR_timer = 0};

const PCA9685_t BIH_SERVO    = { .addr = 0x62, .isLed = false, .osc_freq = 26484736.0 };
const PCA9685_t PLAYER_SERVO = { .addr = 0x43, .isLed = false, .osc_freq = 26434765.0 };


void start_cont_servo(const PCA9685_t* pca9685, Dir_e dir)
{
    uint8_t speed = (dir == CW ? CW_SPEED : CCW_SPEED);

    PCA9685_setServoPos(pca9685, CONT_SERVO_ID, speed);
}

void stop_cont_servo(const PCA9685_t* pca9685)
{
    PCA9685_setServoPos(pca9685, CONT_SERVO_ID, STOP_SPEED);
}

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

                start_cont_servo(&BIH_SERVO, CW);
                BQ.BIH_timer = TIMER_restart();
                BQ.BIH_current_delay = BIH_FEEDFORWARD_DELAY_MS;
                
                ESP_LOGI(TAG, "Ball in hole ball return started");

                BQ.BIH_return_state = DISPENSING;
            }
            break;
        
        case DISPENSING:

            // In the case that we get another request while dispensing, increase the delay by the feedforward amount
            if (BQ.BIH_request)
            {
                BQ.BIH_current_delay += BIH_FEEDFORWARD_DELAY_MS;

                ESP_LOGI(TAG, "Ball in hole ball return already started, adding more time");

                BQ.BIH_request = false;
            }

            if (TIMER_get_ms(BQ.BIH_timer) > BQ.BIH_current_delay)
            {
                // assumed we have dispensed a ball by this time
                stop_cont_servo(&BIH_SERVO);

                ESP_LOGI(TAG, "Ball in hole ball return assumed to be completed");

                BQ.BIH_return_state = WAITING;
            }

            break;
        
        case FAILED:
            break;
    }
}

void run_player_ball_queue_task(void)
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

                start_cont_servo(&PLAYER_SERVO, CCW);
                
                BQ.PBR_timer = TIMER_restart();
                
                ESP_LOGI(TAG, "Player ball return dispensing started");
                BQ.player_return_state = DISPENSING;
            }


            break;
        
        case DISPENSING:
            
            if (SNS_get_ball_queue())
            {
                // confirms that we have read the signal and are ready for the next
                SNS_clear_ball_queue();
                BQ.player_ball_count--;

                BQ.PBR_timer = TIMER_restart();
            }

            if (TIMER_get_ms(BQ.PBR_timer) > PBR_TIMEOUT_MS)
            {
                BQ.player_return_state = FAILED;
            }

            if (BQ.player_ball_count == 0)
            {
                stop_cont_servo(&PLAYER_SERVO);
                
                ESP_LOGI(TAG, "Player ball return dispensing complete");

                BQ.player_return_state = WAITING;
            }

            break;
        
        case FAILED:

            ESP_LOGE(TAG, "Player ball return timed out! Going back to WAITING. Balls remaining before clearing count: %d", BQ.player_ball_count);
            ERRORCODE_set(PLAYER_BALL_RETURN_ERROR);
            stop_cont_servo(&PLAYER_SERVO);

            BQ.player_ball_count = 0;
            BQ.player_return_state = WAITING;
            
            break;
    }
}

void BQ_init(void)
{
    stop_cont_servo(&BIH_SERVO);
    stop_cont_servo(&PLAYER_SERVO);
}

void BQ_request_ball_in_hole_return(void)
{
    BQ.BIH_request = true;
}

void BQ_request_player_return(uint8_t ball_count)
{
    BQ.player_ball_count = ball_count;
    BQ.player_request = true;
}

void BQ_run_task(void)
{
    run_ball_in_hole_return_task();
    run_player_ball_queue_task();
}
