#ifndef I2C_COMM_H
#define I2C_COMM_H
#include "app_main.h"
#include "gpio.h"
#include "driver/i2c.h"

#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 1000
#define SLAVE_ADDRESS 0xC1
#define ATECC508A_CRC_POLYNOM 0x8005
static const uint16_t   I2C_7BIT_ADDRESS        = 0x60;
#define ATECC508A_ADDR 0x60


esp_err_t init_I2C();
void I2C_read(void *pvParameters);


#endif