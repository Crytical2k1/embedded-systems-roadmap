#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "twi.h"
#include "gy.h"
#include "uart.h"

int main(void) {
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
  sei();

  //test WHO_AM_I verify sensor communication
  uint8_t id = gy_read_reg(GY_WHO_AM_I);
  uart_write_string("WHO_AM_I: ");
  uart_write_hex(id);
  uart_write_char('\n');

  //Check sensor identity (MPU-6500 -> 0x70)
  if (id != 0x70)
  {
      uart_write_string("Sensor not detected!\n");
  }

  uint8_t buffer[6];

  while (1)
  {
      gy_read_bytes(GY_ACCEL_XOUT_H, buffer, 6);

      int16_t ax = (buffer[0] << 8) | buffer[1];
      int16_t ay = (buffer[2] << 8) | buffer[3];
      int16_t az = (buffer[4] << 8) | buffer[5];

      uart_write_int(ax);
      uart_write_char(',');
      uart_write_int(ay);
      uart_write_char(',');
      uart_write_int(az);
      uart_write_char('\n');
      _delay_ms(1000);
  }
}