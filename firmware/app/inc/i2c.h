#ifndef I2C_H
#define I2C_H

#include "driver/i2c.h"

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_example_master_init(void);

esp_err_t i2c_readReg(uint8_t addr, uint8_t regAddr, uint8_t* data, size_t dataLen);
esp_err_t i2c_writeReg(uint8_t addr, uint8_t regAddr, uint8_t* data, size_t dataLen);

esp_err_t i2c_readReg8(uint8_t addr, uint8_t regAddr, uint8_t* data);
esp_err_t i2c_writeReg8(uint8_t addr, uint8_t regAddr, uint8_t* data);

#endif