#include "pca9685.h"
#include "i2c.h"

esp_err_t pca9685_setPrescaler(PCA9685_t* pca9685, uint8_t prescaler);
uint8_t pca9685_getPrescaler(PCA9685_t* pca9685);
void pca9685_setPWM(PCA9685_t* pca9685, uint8_t outputPin,uint16_t onPos, uint16_t offPos);
void pca9685_getPWM(PCA9685_t* pca9685, uint8_t outputPin, bool getOff);

void pca9685_setPrescaler(PCA9685_t* pca9685, uint8_t prescaler)
{
    esp_err_t ret = ESP_OK;
    const ADDR = pca9685->addr;

    uint8_t oldMode = 0;
    ret &= i2c_readReg8(ADDR, PCA9685_MODE1, &oldMode);

    uint8_t newMode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
    ret &= i2c_writeReg8(ADDR, PCA9685_MODE1, &newMode);
    ret &= i2c_writeReg8(ADDR, PCA9685_PRESCALE, &prescaler);
    ret &= i2c_writeReg8(ADDR, PCA9685_MODE1, &oldMode);
    
    return ret;
}

uint8_t pca9685_getPrescaler(PCA9685_t* pca9685)
{
    uint8_t prescaler = 0;
    i2c_writeReg8(ADDR, PCA9685_PRESCALE, &prescaler);


    return prescaler;
}