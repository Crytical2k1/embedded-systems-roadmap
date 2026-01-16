#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init(uint32_t baudrate);
void serial_write_uint16(uint16_t value);
void serial_write_char(char c);

#endif