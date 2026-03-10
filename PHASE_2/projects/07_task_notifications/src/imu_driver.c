#include "imu_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

//helpers
static i2c_master_bus_handle_t i2c_bus;
static i2c_master_dev_handle_t i2c_dev;

void imu_init(void) {
    // configure i2c bus
    i2c_master_bus_config_t bus_config = {
        .i2c_port = i2c_port,
        .sda_io_num = i2c_sda_pin,
        .scl_io_num = i2c_scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
    };
    // init i2c bus
    esp_err_t esp_ret = i2c_new_master_bus(&bus_config, &i2c_bus);
    if (esp_ret != ESP_OK) {
        printf("I2C bus: error creating the i2c bus\r\n");
        abort();
    }
    // configure i2c device
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = GY_ADDR,
        .scl_speed_hz = i2c_scl_speed_hz,
    };
    // init i2c device
    esp_ret = i2c_master_bus_add_device(i2c_bus, &dev_config, &i2c_dev);
    if (esp_ret != ESP_OK) {
        printf("I2C dev: error adding device to bus\r\n");
        abort();
    }
    // wake up imu
    uint8_t wake_cmd[2] = {GY_PWR_MGMT_1, 0x00}; //set power management register to 0x00 to reset
    esp_ret = i2c_master_transmit(i2c_dev, wake_cmd, 2, portMAX_DELAY);
    if (esp_ret != ESP_OK) {
        printf("IMU wake up failed\r\n");
        abort();
    }
}

void imu_read(int16_t value[3]) {
    uint8_t data_buffer[6];
    //store register in buffer
    uint8_t reg = GY_ACCEL_XOUT_H;
    //read data from sensor
    esp_err_t esp_ret = i2c_master_transmit_receive(i2c_dev, &reg, 1, data_buffer, 6, portMAX_DELAY);
    if (esp_ret != ESP_OK) {
        printf("IMU couldnt read data\r\n");
    }
    //rewrite data from buffer
    for (uint8_t axis = 0; axis < 3; axis++) {
        value[axis] = (int16_t)((data_buffer[axis*2] << 8) | data_buffer[axis*2 + 1]);
    }
}