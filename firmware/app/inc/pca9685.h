#ifndef PCA9685_H
#define PCA9685_H

#include <stdint.h>
#include "driver/i2c.h"

// REGISTER ADDRESSES
#define PCA9685_MODE1               0x00        // Mode Register 1
#define PCA9685_MODE2               0x01        // Mode Register 2
#define PCA9685_SUBADR1             0x02        // I2C-bus subaddress 1
#define PCA9685_SUBADR2             0x03        // I2C-bus subaddress 2
#define PCA9685_SUBADR3             0x04        // I2C-bus subaddress 3
#define PCA9685_ALLCALLADR          0x05        // LED All Call I2C-bus address
#define PCA9685_LED0_ON_L           0x06        // LED0 on tick, low byte
#define PCA9685_LED0_ON_H           0x07        // LED0 on tick, high byte
#define PCA9685_LED0_OFF_L          0x08        // LED0 off tick, low byte
#define PCA9685_LED0_OFF_H          0x09        // LED0 off tick, high byte

// etc all 16:  LED15_OFF_H 0x45
#define PCA9685_LEDX_OFFSET         4           // There are 4 registers for each LED

#define PCA9685_ALLLED_ON_L         0xFA        // load all the LEDn_ON registers, low
#define PCA9685_ALLLED_ON_H         0xFB        // load all the LEDn_ON registers, high
#define PCA9685_ALLLED_OFF_L        0xFC        // load all the LEDn_OFF registers, low
#define PCA9685_ALLLED_OFF_H        0xFD        // load all the LEDn_OFF registers,high
#define PCA9685_PRESCALE            0xFE        // Prescaler for PWM output frequency
#define PCA9685_TESTMODE            0xFF        // defines the test mode to be entered

// MODE1 bits
#define MODE1_ALLCAL                0x01        // respond to LED All Call I2C-bus address
#define MODE1_SUB3                  0x02        // respond to I2C-bus subaddress 3
#define MODE1_SUB2                  0x04        // respond to I2C-bus subaddress 2
#define MODE1_SUB1                  0x08        // respond to I2C-bus subaddress 1
#define MODE1_SLEEP                 0x10        // Low power mode. Oscillator off
#define MODE1_AI                    0x20        // Auto-Increment enabled
#define MODE1_EXTCLK                0x40        // Use EXTCLK pin clock
#define MODE1_RESTART               0x80        // Restart enabled

// MODE2 bits
#define MODE2_OUTNE_0               0x01        // Active LOW output enable input
#define MODE2_OUTNE_1               0x02        // Active LOW output enable input - high impedience
#define MODE2_OUTDRV                0x04        // totem pole structure vs open-drain
#define MODE2_OCH                   0x08        // Outputs change on ACK vs STOP
#define MODE2_INVRT                 0x10        // Output logic state inverted

#define FREQUENCY_OSCILLATOR        25000000    //Int. osc. frequency in datasheet

#define DEFAULT_SERVO_FREQ          50.0F

typedef struct{
    uint8_t addr;       // I2C slave address
    float osc_freq;     // Tested true osc_freq of the chip
}PCA9685_t;

/**
 * @brief Initializes the PCA9685 pwm driver
 * @param pca9685 PCA9685 handle
 */
void PCA9685_init(PCA9685_t* pca9685);

/**
 * @brief Sets the frequency for the entire chip (24Hz - 1526Hz)
 * @param pca9685 PCA9685 handle
 * @param freq Frequency to set
 */
void PCA9685_setFreq(PCA9685_t* pca9685, float freq);

/**
 * @brief Sets the relative position of a servo via PWM duty cycle manipulation
 * @param pca9685 PCA9685 handle
 * @param outputPin Which servo position to set (0 - 15)
 * @param servoPos The position to set (0-255) linearized to full scale range
 */
void PCA9685_setServoPos(PCA9685_t* pca9685, uint8_t outputPin, uint8_t servoPos);

/**
 * @brief Sets the servo position controlled by the entire chip via PWM duty cycle manipulation
 * @param pca9685 PCA9685 handle
 * @param servoPos The position to set (0-255) linearized to full scale range
 */
void PCA9685_setAllServoPos(PCA9685_t* pca9685, uint8_t servoPos);

#endif