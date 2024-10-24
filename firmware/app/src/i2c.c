#include "i2c.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#define I2C_MASTER_SCL_IO           2
#define I2C_MASTER_SDA_IO           14
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_ENABLE_SDA_PULLUP       0
#define I2C_ENABLE_SCL_PULLUP       0
#define WRITE_BIT                   I2C_MASTER_WRITE
#define READ_BIT                    I2C_MASTER_READ
#define ACK_CHECK_EN                0x1
#define ACK_CHECK_DIS               0x0
#define LAST_NACK_VAL               0x2
#define I2C_CLK_STRETCH_TICK        300 // 300 ticks, Clock stretch is about 210us, you can make changes according to the actual situation.

esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = I2C_ENABLE_SDA_PULLUP;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = I2C_ENABLE_SCL_PULLUP;
    conf.clk_stretch_tick = I2C_CLK_STRETCH_TICK;
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    return ESP_OK;
}

esp_err_t i2c_readReg(uint8_t addr, uint8_t regAddr, uint8_t* data, size_t dataLen)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regAddr, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, data, dataLen, LAST_NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_writeReg(uint8_t addr, uint8_t regAddr, uint8_t* data, size_t dataLen)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regAddr, ACK_CHECK_EN);
    i2c_master_write(cmd, data, dataLen, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_readReg8(uint8_t addr, uint8_t regAddr, uint8_t* data)
{
    return i2c_readReg(addr, regAddr, data, 1);
}


esp_err_t i2c_writeReg8(uint8_t addr, uint8_t regAddr, uint8_t* data)
{
    return i2c_writeReg(addr, regAddr, data, 1);
}