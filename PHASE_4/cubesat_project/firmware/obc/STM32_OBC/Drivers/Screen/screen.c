#include "screen.h"

#include <string.h>

#define PAGES (SCREEN_HEIGHT / 8)

static I2C_HandleTypeDef *s_hi2c = NULL;
static uint8_t s_framebuffer[SCREEN_WIDTH * PAGES];

typedef struct {
	char c;
	uint8_t cols[5];
} font_t;
static const font_t FONT[] = {
    {' ', {0x00,0x00,0x00,0x00,0x00}},
    {'-', {0x08,0x08,0x08,0x08,0x08}},
    {'.', {0x00,0x60,0x60,0x00,0x00}},
    {':', {0x00,0x36,0x36,0x00,0x00}},
    {'%', {0x62,0x64,0x08,0x13,0x23}},
    {'0', {0x3E,0x51,0x49,0x45,0x3E}},
    {'1', {0x00,0x42,0x7F,0x40,0x00}},
    {'2', {0x62,0x51,0x49,0x49,0x46}},
    {'3', {0x22,0x41,0x49,0x49,0x36}},
    {'4', {0x18,0x14,0x12,0x7F,0x10}},
    {'5', {0x27,0x45,0x45,0x45,0x39}},
    {'6', {0x3C,0x4A,0x49,0x49,0x30}},
    {'7', {0x01,0x71,0x09,0x05,0x03}},
    {'8', {0x36,0x49,0x49,0x49,0x36}},
    {'9', {0x06,0x49,0x49,0x29,0x1E}},
    {'A', {0x7E,0x11,0x11,0x11,0x7E}},
    {'C', {0x3E,0x41,0x41,0x41,0x22}},
    {'D', {0x7F,0x41,0x41,0x22,0x1C}},
    {'E', {0x7F,0x49,0x49,0x49,0x41}},
    {'G', {0x3E,0x41,0x49,0x49,0x7A}},
    {'H', {0x7F,0x08,0x08,0x08,0x7F}},
    {'I', {0x00,0x41,0x7F,0x41,0x00}},
    {'K', {0x7F,0x08,0x14,0x22,0x41}},
    {'L', {0x7F,0x40,0x40,0x40,0x40}},
    {'M', {0x7F,0x02,0x0C,0x02,0x7F}},
    {'N', {0x7F,0x04,0x08,0x10,0x7F}},
    {'O', {0x3E,0x41,0x41,0x41,0x3E}},
    {'P', {0x7F,0x09,0x09,0x09,0x06}},
    {'R', {0x7F,0x09,0x19,0x29,0x46}},
    {'S', {0x26,0x49,0x49,0x49,0x32}},
    {'T', {0x01,0x01,0x7F,0x01,0x01}},
    {'U', {0x3F,0x40,0x40,0x40,0x3F}},
    {'X', {0x63,0x14,0x08,0x14,0x63}},
    {'Y', {0x07,0x08,0x70,0x08,0x07}},
    {'Z', {0x61,0x51,0x49,0x45,0x43}},
};
#define FONT_COUNT (sizeof(FONT) / sizeof(FONT[0]))

static const font_t *find_char(char c) {
	for (size_t i = 0; i < FONT_COUNT; i++) {
		if (FONT[i].c == c) {
			return &FONT[i];
		}
	}
	return &FONT[0]; //return to blank space
}

//forward declaration
bool screen_init(I2C_HandleTypeDef *hi2c);
void screen_clear(void);
void screen_update(void);
static void screen_write_cmd(uint8_t cmd);
void screen_set_pixel(uint8_t x, uint8_t y, bool on);
static void screen_draw_char(uint8_t x, uint8_t y, char c);
void screen_draw_string(uint8_t x, uint8_t y, const char *text);

bool screen_init(I2C_HandleTypeDef *hi2c) {
	s_hi2c = hi2c;

	//standard power-on init sequence
	const uint8_t init_cmds[] = {
		0xAE,       /* display off */
		0xD5, 0x80, /* clock divide */
		0xA8, 0x3F, /* multiplex ratio = 64 rows */
		0xD3, 0x00, /* display offset = 0 */
		0x40,       /* start line = 0 */
		0x8D, 0x14, /* enable charge pump */
		0x20, 0x00, /* horizontal addressing mode */
		0xA1,       /* segment remap */
		0xC8,       /* COM scan direction */
		0xDA, 0x12, /* COM pins config */
		0x81, 0x7F, /* contrast */
		0xD9, 0xF1, /* pre-charge period */
		0xDB, 0x40, /* VCOMH deselect level */
		0xA4,       /* resume to RAM content display */
		0xA6,       /* normal (not inverted) display */
		0xAF        /* display on */
	};

	for (size_t i = 0; i < sizeof(init_cmds); i++) {
		screen_write_cmd(init_cmds[i]);
	}

	screen_clear();
	screen_update();
	return true;
}

void screen_clear(void) {
	memset(s_framebuffer, 0x00, sizeof(s_framebuffer));
}

void screen_update(void) {

	screen_write_cmd(0x21); screen_write_cmd(0); screen_write_cmd(SCREEN_WIDTH - 1);
	screen_write_cmd(0x22); screen_write_cmd(0); screen_write_cmd(PAGES - 1);

	//0x40 prefix = the following bytes are framebuffer data not commands
	uint8_t prefixed[1 + sizeof(s_framebuffer)];
	prefixed[0] = 0x40;
	memcpy(&prefixed[1], s_framebuffer, sizeof(s_framebuffer));

	HAL_I2C_Master_Transmit(s_hi2c, SCREEN_ADDR, prefixed, sizeof(prefixed), 200);

}

static void screen_write_cmd(uint8_t cmd) {
	//0x00 prefix byte to tell commands apart from data
	uint8_t buf[2] = {0x00, cmd};
	HAL_I2C_Master_Transmit(s_hi2c, SCREEN_ADDR, buf, 2, 100);
}

void screen_set_pixel(uint8_t x, uint8_t y, bool on) {
	if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
		return;
	}
	uint16_t index = x + (y / 8) * SCREEN_WIDTH;
	uint8_t bit = y % 8;

	if (on) {
		s_framebuffer[index] |= (1U << bit); //set bit
	} else {
		s_framebuffer[index] &= ~(1U << bit); //clear bit
	}
}

static void screen_draw_char(uint8_t x, uint8_t y, char c) {
	const font_t *symbol = find_char(c);
	for (uint8_t col = 0; col < 5; col++) {
		uint8_t colbits = symbol->cols[col];
		for (uint8_t row = 0; row < 7; row++) {
			bool on = (colbits >> row) & 0x01;
			screen_set_pixel(x + col, y + row, on);
		}
	}
}

void screen_draw_string(uint8_t x, uint8_t y, const char *text) {
	uint8_t cursor_x = x;
	while (*text != '\0') {
		screen_draw_char(cursor_x, y, *text);
		cursor_x += 6; //5px font + 1px space
		text++;
		if (cursor_x + 5 >= SCREEN_WIDTH) {
			break; // stop if it doesnt fit inside the line
		}
	}
}
