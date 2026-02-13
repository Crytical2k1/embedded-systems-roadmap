#include "gy.h"
#include "twi.h"

void gy_init(void) {
    //wake sensor
    gy_write_reg(GY_PWR_MGMT_1, 0x00);
}

void gy_write_reg(uint8_t reg, uint8_t data) {
    /*
    START
    SLA+W (set to write)
    REGISTER_ADDRESS
    DATA
    STOP*/
    twi_start();
    twi_write((GY_ADDR << 1) | 0); // write mode
    //GY_ADDR is shifted left as I2C address is 7 bit. The last bit is R/W.
    twi_write(reg);
    twi_write(data);
    twi_stop();
}
uint8_t gy_read_reg(uint8_t reg) {
    /*
    START
    SLA+W (set to write)
    REGISTER_ADDRESS
    REPEATED START
    SLA+R (set to read)
    READ DATA
    STOP*/
    uint8_t data;
    twi_start();
    twi_write((GY_ADDR << 1) | 0); // write mode
    twi_write(reg);
    twi_start();
    twi_write((GY_ADDR << 1) | 1); // read mode
    data = twi_read_nack();
    twi_stop();

    return data;
}

void gy_read_bytes(uint8_t reg, uint8_t *buffer, uint8_t length) {
    twi_start();
    twi_write((GY_ADDR << 1) | 0); // write mode
    twi_write(reg);

    twi_start();
    twi_write((GY_ADDR << 1) | 1); // read mode
    //read and store each byte
    for (uint8_t i = 0; i < length -1; i++) {
        buffer[i] = twi_read_ack();
    }
    //last byte in nack to finish the transmition
    buffer[length - 1] = twi_read_nack();

    twi_stop();
}