#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init(uint16_t baudrate);
void serial_write_char(char c);
void serial_write_uint16(uint16_t value);
void serial_write_string(const char *str);
uint8_t serial_rx_available(void);
char serial_read_char(void);


#endif