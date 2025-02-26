#include "error_codes.h"

bool activeErrorCodes[NUM_ERROR_CODES] = {0};

void ERRORCODE_set(ERROR_CODE_e error)
{
    activeErrorCodes[error] = true;
}

void ERRORCODE_clear(ERROR_CODE_e error)
{
    activeErrorCodes[error] = false;
}

void ERRORCODE_clear_all(ERROR_CODE_e error)
{
    for (int i = 0; i < NUM_ERROR_CODES; i++)
    {
        activeErrorCodes[i] = false;
    }
}

void ERRORCODE_get_all(bool* errors)
{
    for (int i = 0; i < NUM_ERROR_CODES; i++)
    {
        errors[i] = activeErrorCodes[i];
    }
}