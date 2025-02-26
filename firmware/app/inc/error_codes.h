#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <stdbool.h>

typedef enum {
    BALL_MATH_ERROR = 0,
    BALL_IN_HOLE_FEED_ERROR,
    PLAYER_BALL_RETURN_ERROR,
    NVS_ERROR,

    NUM_ERROR_CODES
} ERROR_CODE_e;

void ERRORCODE_set(ERROR_CODE_e error);
void ERRORCODE_clear(ERROR_CODE_e error);
void ERRORCODE_clear_all(ERROR_CODE_e error);
void ERRORCODE_get_all(bool* errors);

#endif // ERROR_CODES_H