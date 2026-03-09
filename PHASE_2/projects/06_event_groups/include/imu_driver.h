#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H

#include <stdint.h>
#include "driver/i2c_master.h"

//config
static const i2c_port_num_t i2c_port = 0;
static const gpio_num_t i2c_sda_pin = GPIO_NUM_21;       //gpio pin for sda
static const gpio_num_t i2c_scl_pin = GPIO_NUM_22;       //gpio pin for scl
static const uint32_t i2c_spd_clk_hz = 100000;           //100kHz

//registers
static const uint8_t GY_ADDR = 0x68;
static const uint8_t GY_PWR_MGNT_1 = 0x6B;      //wake up register
static const uint8_t GY_ACCEL_XOUT_H = 0x3B;    //data register


esp_err_t imu_init(void);
void imu_read(int16_t values[3]);

#endif