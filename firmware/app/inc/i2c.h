#ifndef I2C_H
#define I2C_H

#include "driver/i2c.h"

/**
 * @brief i2c master initialization
 * @return ESP error code
 */
esp_err_t I2C_master_init(void);

/**
 * @brief Read multiple bytes
 * @param addr Slave address
 * @param regAddr Register address
 * @param data Pointer to data buffer to put read data in
 * @param dataLen Length of data to read
 * @return ESP error code
 */
esp_err_t I2C_readReg(uint8_t addr, uint8_t regAddr, uint8_t* data, size_t dataLen);

/**
 * @brief Write multiple bytes
 * @param addr Slave address
 * @param regAddr Register address
 * @param data Pointer to data buffer to transmit out of
 * @param dataLen Length of data to transmit
 * @return ESP error code
 */
esp_err_t I2C_writeReg(uint8_t addr, uint8_t regAddr, uint8_t* data, size_t dataLen);

/**
 * @brief Read one byte
 * @param addr Slave address
 * @param regAddr Register address
 * @param data Pointer to data variable to put read data in
 * @return ESP error code
 */
esp_err_t I2C_readReg8(uint8_t addr, uint8_t regAddr, uint8_t* data);

/**
 * @brief Write one byte
 * @param addr Slave address
 * @param regAddr Register address
 * @param data Pointer to data variable to transmit out of
 * @return ESP error code
 */
esp_err_t I2C_writeReg8(uint8_t addr, uint8_t regAddr, uint8_t* data);

#endif