#include "lcd_display.h"
#include <avr/io.h>
#include <util/delay.h>

//LCD display
#define LCD_REGISTER_PORT PORTD
#define LCD_REGISTER_DDR DDRD
#define LCD_ENABLE_PORT PORTB
#define LCD_ENABLE_DDR DDRB
#define LCD_DATA_PORT PORTB
#define LCD_DATA_DDR DDRB
#define LCD_RS PD7
#define LCD_E PB0
#define LCD_D4 PB1
#define LCD_D5 PB2
#define LCD_D6 PB3
#define LCD_D7 PB4

#define SET_PIN(pin,port) (port |= (1 << pin))
#define CLEAR_PIN(pin,port) (port &= ~(1 << pin))

//send nibble (4 bits)
static void lcd_send_nibble(uint8_t nibble) {
    if (nibble & 0x01) SET_PIN(LCD_D4, LCD_DATA_PORT); else CLEAR_PIN(LCD_D4, LCD_DATA_PORT);
    if (nibble & 0x02) SET_PIN(LCD_D5, LCD_DATA_PORT); else CLEAR_PIN(LCD_D5, LCD_DATA_PORT);
    if (nibble & 0x04) SET_PIN(LCD_D6, LCD_DATA_PORT); else CLEAR_PIN(LCD_D6, LCD_DATA_PORT);
    if (nibble & 0x08) SET_PIN(LCD_D7, LCD_DATA_PORT); else CLEAR_PIN(LCD_D7, LCD_DATA_PORT);

    SET_PIN(LCD_E, LCD_ENABLE_PORT);
    for(volatile uint8_t i=0;i<5;i++); //short pulse
    CLEAR_PIN(LCD_E, LCD_ENABLE_PORT);
    _delay_us(50);
}

static void lcd_send_byte(uint8_t data, uint8_t rs) {
    if (rs) SET_PIN(LCD_RS, LCD_REGISTER_PORT);
    else CLEAR_PIN(LCD_RS, LCD_REGISTER_PORT);

    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
}

void lcd_init(void) {
	//set to output every pin
    LCD_REGISTER_DDR |= (1 << LCD_RS);
    LCD_ENABLE_DDR |= (1 << LCD_E);
    LCD_DATA_DDR |= (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
	
	//forced lcd reset
	_delay_ms(40);
	lcd_send_nibble(0x03); //function set command
	_delay_ms(5);
	lcd_send_nibble(0x03); //repeat command (in case was ignored)
	_delay_us(150);
	lcd_send_nibble(0x03); //repeat command (ensures synchronization)
	lcd_send_nibble(0x02); //4-bit mode
	
	//initialize the lcd
	lcd_send_byte(0x28, 0); //2 lines, 5x8 font
	lcd_send_byte(0x0C, 0); // display ON
	lcd_send_byte(0x06, 0); //entry mode
	lcd_send_byte(0x01, 0); //clear display
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? col : (0x40 + col);
    lcd_send_byte(0x80 | address, 0);
}

static void lcd_write_char(char c) {
    lcd_send_byte((uint8_t)c, 1);
}

void lcd_write_string(const char *str) {
    while (*str) {
        lcd_write_char(*str++);
    }
}
void lcd_write_int(uint16_t value) {
    char buffer[6];
    uint8_t i = 0;

    if (value == 0) {
        lcd_write_char('0');
        return;
    }

    while (value > 0) {
        buffer[i++] = '0' + value % 10;
        value /= 10;
    }
    while (i > 0) {
        lcd_write_char(buffer[--i]);
    }
}