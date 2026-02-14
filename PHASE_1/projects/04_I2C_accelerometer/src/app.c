#include "app.h"
#include "twi.h"
#include "gy.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>

static uint8_t buffer[6];
static int16_t refactor_x = 0;
static int16_t refactor_y = 0;
static int16_t refactor_z = 0;
#define GRAV_FACTOR 16384
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define ALPHA 128 // 0.1 ~ 26/256
static int32_t filter_value[3] = {0, 0, 0};

static void sensor_calibration(void) {
    int32_t buffer_ax = 0;
    int32_t buffer_ay = 0;
    int32_t buffer_az = 0;

    for (uint8_t i = 0; i < 20; i++) {
        gy_read_bytes(GY_ACCEL_XOUT_H, buffer, 6);

        buffer_ax += (buffer[0] << 8) | buffer[1];
        buffer_ay += (buffer[2] << 8) | buffer[3];
        buffer_az += (buffer[4] << 8) | buffer[5];

        _delay_ms(50);
    }

    refactor_x = buffer_ax / 20;
    refactor_y = buffer_ay / 20;
    refactor_z =(buffer_az / 20) - GRAV_FACTOR;
}

void app_init(void) {
    //enable interal pull-up resistors
    PORTC |= (1 << PC4) | (1 << PC5);
    //Initialize serial communication
    uart_init(9600);
    uart_write_string("=== MPU Sensor Test ===\n");
    //Initialize TWI (I2C)
    twi_init();
    uart_write_string("TWI Initialized\n");
    //delay to let sensor power up
    _delay_ms(100);
    //Initialize sensor
    gy_init();
    uart_write_string("Sensor Initialized\n");
    _delay_ms(100);
    //test sensor communication
    gy_who_am_i();

    //calibrate sensor
    sensor_calibration();
}

static int16_t refactor_calibrate(int16_t value, uint8_t axis) {
    switch (axis) {
        case 0:
            value -= refactor_x;
            break;
        case 1:
            value -= refactor_y;
            break;
        case 2:
            value -= refactor_z;
            break;
        default:
            break;
    }
    return value;
}

static int16_t EMA_filter(int16_t raw_value, uint8_t axis) {
    filter_value[axis] += (ALPHA * (raw_value - filter_value[axis])) >> 8; //restore scale factor from ALPHA
    return (int16_t)filter_value[axis];
}

void app_orientation(void) {

    gy_read_bytes(GY_ACCEL_XOUT_H, buffer, 6);

    int16_t ax = refactor_calibrate(((buffer[0] << 8) | buffer[1]), AXIS_X);
    int16_t ay = refactor_calibrate(((buffer[2] << 8) | buffer[3]), AXIS_Y);
    int16_t az = refactor_calibrate(((buffer[4] << 8) | buffer[5]), AXIS_Z);

    ax = EMA_filter(ax, AXIS_X);
    ay = EMA_filter(ay, AXIS_Y);
    az = EMA_filter(az, AXIS_Z);

    uart_write_int(ax);
    uart_write_char(',');
    uart_write_int(ay);
    uart_write_char(',');
    uart_write_int(az);
    uart_write_char('\n');
    _delay_ms(50);
}