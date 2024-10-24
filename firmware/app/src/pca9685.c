#include "pca9685.h"
#include "i2c.h"

void     pca9685_setPrescaler(PCA9685_t* pca9685, uint8_t prescaler);
uint8_t  pca9685_getPrescaler(PCA9685_t* pca9685);
void     pca9685_setPWM(PCA9685_t* pca9685, uint8_t outputPin, uint16_t onPos, uint16_t offPos);
uint16_t pca9685_getPWM(PCA9685_t* pca9685, uint8_t outputPin, bool getOff);

void pca9685_setPrescaler(PCA9685_t* pca9685, uint8_t prescaler)
{
    const ADDR = pca9685->addr;

    uint8_t oldMode = 0;
    i2c_readReg8(ADDR, PCA9685_MODE1, &oldMode);

    uint8_t newMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
    i2c_writeReg8(ADDR, PCA9685_MODE1, &newMode);
    i2c_writeReg8(ADDR, PCA9685_PRESCALE, &prescaler);
    i2c_writeReg8(ADDR, PCA9685_MODE1, &oldMode);

}

uint8_t pca9685_getPrescaler(PCA9685_t* pca9685)
{
    uint8_t prescaler = 0;
    i2c_writeReg8(pca9685->addr, PCA9685_PRESCALE, &prescaler);

    return prescaler;
}


// I THINK WE WILL HAVE TO ENABLE AUTO-INCREMENT IN INIT FUNCTION

uint16_t pca9685_getPWM(PCA9685_t* pca9685, uint8_t outputPin, bool getOff)
{
    uint8_t regAddr = PCA9685_LED0_ON_L + PCA9685_LEDX_OFFSET * outputPin + (getOff ? 2 : 0);

    const uint8_t PWM_COUNT_SIZE = 2;
    uint8_t data[PWM_COUNT_SIZE] = {0};

    i2c_readReg(pca9685->addr, regAddr, data, PWM_COUNT_SIZE);

    return uint16_t(data[0]) | (uint16_t(data[1]) << 8);
}
