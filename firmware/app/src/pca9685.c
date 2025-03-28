#include "pca9685.h"
#include "i2c.h"
#include "math.h"

#define ON_L_OFFSET             0
#define ON_H_OFFSET             1
#define OFF_L_OFFSET            2
#define OFF_H_OFFSET            3
#define ON_OFF_L_MASK           0x00FF
#define ON_OFF_H_MASK           0x0F00
#define SET_PWM_SIZE            4
#define GET_PWM_SIZE            2

#define MIN_FREQ                24.0F
#define MAX_FREQ                1526.0F

#define MIN_POS                 0.0F   // 0% duty
#define MAX_POS                 127.0F // 100% duty

#define MIN_OFF_POS_SERVO       85.176F // 2.08% duty found experimentally
#define MAX_OFF_POS_SERVO       542.5875F // 13.5% duty found experimentally
#define GAIN_SERVO              ((MAX_OFF_POS_SERVO - MIN_OFF_POS_SERVO)/(MAX_POS - MIN_POS))

#define MIN_OFF_POS_LED         0.0F        // 0% duty
#define MAX_OFF_POS_LED         4095.0F     // 100% duty
#define GAIN_LED                ((MAX_OFF_POS_LED - MIN_OFF_POS_LED)/(MAX_POS - MIN_POS))

#define TOTAL_NUM_SERVO 15

void        pca9685_setPrescaler(const PCA9685_t* pca9685, uint8_t prescaler);
uint8_t     pca9685_getPrescaler(const PCA9685_t* pca9685);
esp_err_t   pca9685_setPWM(const PCA9685_t* pca9685, uint8_t outputPin, uint16_t onPos, uint16_t offPos);
uint16_t    pca9685_getPWM(const PCA9685_t* pca9685, uint8_t outputPin, bool getOff);

void pca9685_setPrescaler(const PCA9685_t* pca9685, uint8_t prescaler)
{
    const uint8_t ADDR = pca9685->addr;

    uint8_t oldMode = 0;
    I2C_readReg8(ADDR, PCA9685_MODE1, &oldMode);

    uint8_t newMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
    I2C_writeReg8(ADDR, PCA9685_MODE1, newMode);
    I2C_writeReg8(ADDR, PCA9685_PRESCALE, prescaler);
    I2C_writeReg8(ADDR, PCA9685_MODE1, oldMode);
}

uint8_t pca9685_getPrescaler(const PCA9685_t* pca9685)
{
    uint8_t prescaler = 0;
    I2C_writeReg8(pca9685->addr, PCA9685_PRESCALE, prescaler);

    return prescaler;
}

esp_err_t pca9685_setPWM(const PCA9685_t* pca9685, uint8_t outputPin, uint16_t onPos, uint16_t offPos)
{
    uint8_t regAddr = PCA9685_LED0_ON_L + PCA9685_LEDX_OFFSET * outputPin;
    
    uint8_t data[SET_PWM_SIZE] = {0};

    data[ON_L_OFFSET] = (uint8_t)( onPos & ON_OFF_L_MASK);
    data[ON_H_OFFSET] = (uint8_t)((onPos & ON_OFF_H_MASK) >> 8);

    data[OFF_L_OFFSET] = (uint8_t)( offPos & ON_OFF_L_MASK);
    data[OFF_H_OFFSET] = (uint8_t)((offPos & ON_OFF_H_MASK) >> 8);

    return I2C_writeReg(pca9685->addr, regAddr, data, SET_PWM_SIZE);
}

uint16_t pca9685_getPWM(const PCA9685_t* pca9685, uint8_t outputPin, bool getOff)
{
    uint8_t regAddr = PCA9685_LED0_ON_L + PCA9685_LEDX_OFFSET * outputPin + (getOff ? OFF_L_OFFSET : ON_L_OFFSET);

    uint8_t data[GET_PWM_SIZE] = {0};

    I2C_readReg(pca9685->addr, regAddr, data, GET_PWM_SIZE);

    return (uint16_t)(data[0]) | ((uint16_t)(data[1]) << 8);
}


// init
void PCA9685_init(const PCA9685_t* pca9685)
{
    //configure the mode 1 and 2
    const uint8_t ADDR = pca9685->addr;
    uint8_t mode1 = MODE1_AI;

    uint8_t mode2 = MODE2_OUTDRV;
    if (pca9685->isLed)
    {
        mode2 = MODE2_INVRT; // may have to change between LED and Servos
    }
    
    PCA9685_setFreq(pca9685, DEFAULT_SERVO_FREQ);
    
    I2C_writeReg8(ADDR, PCA9685_MODE1, mode1);
    I2C_writeReg8(ADDR, PCA9685_MODE2, mode2);
}

// set frequency between 24Hz and 1526Hz
void PCA9685_setFreq(const PCA9685_t* pca9685, float freq)
{
    if (freq < MIN_FREQ){freq = MIN_FREQ;}
    if (freq > MAX_FREQ){freq = MAX_FREQ;}

    uint8_t prescaler = (uint16_t)(round(pca9685->osc_freq / (4096.0 * freq)) - 1.0);

    pca9685_setPrescaler(pca9685, prescaler);
}

// set one servo position
void PCA9685_setServoPos(const PCA9685_t* pca9685, uint8_t outputPin, uint8_t servoPos)
{
    const uint16_t ON_POS = 0;
    uint16_t OFF_POS = (uint16_t)round(GAIN_SERVO * servoPos + MIN_OFF_POS_SERVO);

    if (pca9685->isLed)
    {
        OFF_POS = (uint16_t)round(GAIN_LED * servoPos + MIN_OFF_POS_LED);
    } 

    pca9685_setPWM(pca9685, outputPin, ON_POS, OFF_POS);
}

// set all servo position to the same
void PCA9685_setAllServoPos(const PCA9685_t* pca9685, uint8_t servoPos)
{
    const uint16_t ON_POS = 0;
    uint16_t OFF_POS = (uint16_t)round(GAIN_SERVO * servoPos + MIN_OFF_POS_SERVO);

    if (pca9685->isLed)
    {
        OFF_POS = (uint16_t)round(GAIN_LED * servoPos + MIN_OFF_POS_LED);
    } 
    
    for (uint8_t i = 0; i < TOTAL_NUM_SERVO; i++)
    {
        pca9685_setPWM(pca9685, i, ON_POS, OFF_POS);
    }
}
