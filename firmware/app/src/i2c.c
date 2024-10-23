#include "i2c.h"
#include "esp_err.h"

#define I2C_EXAMPLE_MASTER_SCL_IO           2
#define I2C_EXAMPLE_MASTER_SDA_IO           14
#define I2C_EXAMPLE_MASTER_NUM              I2C_NUM_0
#define I2C_ENABLE_SDA_PULLUP               0
#define I2C_ENABLE_SCL_PULLUP               0
#define I2C_CLK_STRETCH_TICK                300 // 300 ticks, Clock stretch is about 210us, you can make changes according to the actual situation.

esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = I2C_ENABLE_SDA_PULLUP;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = I2C_ENABLE_SCL_PULLUP;
    conf.clk_stretch_tick = I2C_CLK_STRETCH_TICK;
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    return ESP_OK;
}