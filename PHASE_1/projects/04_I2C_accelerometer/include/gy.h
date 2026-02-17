#ifndef GY_H
#define GY_H

#include <stdint.h>

//I2C address
#define GY_ADDR 0x68
// Registers
#define GY_WHO_AM_I 0x75
#define GY_PWR_MGMT_1 0x6B
#define GY_ACCEL_XOUT_H 0x3B //accelerometer x values
#define GY_GYRO_XOUT_H 0x43 //gyroscope x values

void gy_init(void);
void gy_write_reg(uint8_t reg, uint8_t data);
uint8_t gy_read_reg(uint8_t reg);
void gy_read_bytes(uint8_t reg, uint8_t *buffer, uint8_t length);
void gy_who_am_i(void);

#endif