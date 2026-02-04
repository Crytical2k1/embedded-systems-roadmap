#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init(uint16_t baudrate);
void serial_write_char(char c);
void serial_write_uint(uint16_t value);
void serial_write_string(const char *str);

#endif 