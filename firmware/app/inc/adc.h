#ifndef ADC_H
#define ADC_H

#include "stdint.h"

void ADC_init(void);
uint16_t ADC_getLvADCVal(void);
uint16_t ADC_getBDADCVal(void);

#endif