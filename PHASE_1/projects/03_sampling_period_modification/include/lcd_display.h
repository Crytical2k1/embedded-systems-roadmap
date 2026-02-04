#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H
#include <stdint.h>

void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_write_string(const char *str);
void lcd_write_int(uint16_t value);

#endif