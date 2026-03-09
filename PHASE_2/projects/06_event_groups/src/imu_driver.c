#include "imu_driver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

//helpers
static const char *TAG = "imu_driver";
static i2c_master_bus_handle_t i2c_bus;
static i2c_master_dev_handle_t i2c_dev;

esp_err_t imu_init(void){
    //configure i2c bus
    i2c_master_bus_config_t i2c_config = {
        .i2c_port = i2c_port,
        .sda_io_num = i2c_sda_pin,
        .scl_io_num = i2c_scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    //init i2c bus
    esp_err_t esp_ret = i2c_new_master_bus(&i2c_config, &i2c_bus);
    if(esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing the i2c bus");
        return ESP_FAIL;
    }
    //configure i2c device
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = GY_ADDR,
        .scl_speed_hz = i2c_spd_clk_hz,
    };
    //init i2c device
    esp_ret = i2c_master_bus_add_device(i2c_bus, &dev_config, &i2c_dev);
    if(esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing the i2c device on the bus");
        return ESP_FAIL;
    }
    //wake up device
    const uint8_t wake_cmd[2] = {GY_PWR_MGNT_1, 0x00};
    esp_ret = i2c_master_transmit(i2c_dev, wake_cmd, 2, portMAX_DELAY);
    if(esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "IMU wake up failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void imu_read(int16_t values[3]){
    const uint8_t reg = GY_ACCEL_XOUT_H;
    uint8_t read_buffer[6];

    i2c_master_transmit_receive(i2c_dev, &reg, 1, read_buffer, 6, pdMS_TO_TICKS(100));

    for (uint8_t axis = 0; axis < 3; axis++) {
        values[axis] = (int16_t)((read_buffer[axis * 2] << 8) | read_buffer[axis * 2 + 1]);
    }
}