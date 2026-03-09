#ifndef LCD_MANAGER_H
#define LCD_MANAGER_H

#include <stdint.h>

void lcd_manager_init(void *pvParameters);
void lcd_manager_write(uint8_t col, uint8_t row, const char *str);
void lcd_manager_clear(void);


#endif