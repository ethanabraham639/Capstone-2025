#include "pca9685.h"
#include "i2c.h"

#define ON_L_OFFSET     0
#define ON_H_OFFSET     1
#define OFF_L_OFFSET    2
#define OFF_H_OFFSET    3
#define ON_OFF_L_MASK   0x00FF
#define ON_OFF_H_MASK   0x0F00
#define SET_PWM_SIZE    4
#define GET_PWM_SIZE    2

void        pca9685_setPrescaler(PCA9685_t* pca9685, uint8_t prescaler);
uint8_t     pca9685_getPrescaler(PCA9685_t* pca9685);
esp_err_t   pca9685_setPWM(PCA9685_t* pca9685, uint8_t outputPin, uint16_t onPos, uint16_t offPos);
uint16_t    pca9685_getPWM(PCA9685_t* pca9685, uint8_t outputPin, bool getOff);

void pca9685_setPrescaler(PCA9685_t* pca9685, uint8_t prescaler)
{
    const uint8_t ADDR = pca9685->addr;

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

esp_err_t pca9685_setPWM(PCA9685_t* pca9685, uint8_t outputPin, uint16_t onPos, uint16_t offPos)
{
    uint8_t regAddr = PCA9685_LED0_ON_L + PCA9685_LEDX_OFFSET * outputPin;
    
    uint8_t data[SET_PWM_SIZE] = {0};

    data[ON_L_OFFSET] = (uint8_t)( onPos & ON_OFF_L_MASK);
    data[ON_H_OFFSET] = (uint8_t)((onPos & ON_OFF_H_MASK) >> 8);

    data[OFF_L_OFFSET] = (uint8_t)( offPos & ON_OFF_L_MASK);
    data[OFF_H_OFFSET] = (uint8_t)((offPos & ON_OFF_H_MASK) >> 8);

    return i2c_writeReg(pca9685->addr, regAddr, data, SET_PWM_SIZE);
}

uint16_t pca9685_getPWM(PCA9685_t* pca9685, uint8_t outputPin, bool getOff)
{
    uint8_t regAddr = PCA9685_LED0_ON_L + PCA9685_LEDX_OFFSET * outputPin + (getOff ? OFF_L_OFFSET : ON_L_OFFSET);

    uint8_t data[GET_PWM_SIZE] = {0};

    i2c_readReg(pca9685->addr, regAddr, data, GET_PWM_SIZE);

    return (uint16_t)(data[0]) | ((uint16_t)(data[1]) << 8);
}






// init
void PCA9685_init(void)
{

}

// set frequency
void PCA9685_setFreq(uint16_t freq)
{

}

// set one servo position
void PCA9685_setServoPos(uint8_t pos)
{

}

// set all servo position to the same
void PCA9685_setAllServoPos(uint8_t pos)
{
    
}
