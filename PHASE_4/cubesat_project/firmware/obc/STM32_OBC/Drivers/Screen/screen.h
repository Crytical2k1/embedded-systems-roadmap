#ifndef SCREEN_SCREEN_H_
#define SCREEN_SCREEN_H_

#include <stdbool.h>

#include "main.h"

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128

#define SCREEN_ADDR (0x3C << 1)

bool screen_init(I2C_HandleTypeDef *hi2c);
void screen_clear(void); //clear the framebuffer memory (does not touch the physical screen by itself)
void screen_update(void); //pushed the framebuffer memory into the physical screen

void screen_set_pixel(uint8_t x, uint8_t y, bool on); //sets/clears one pixel of the framebuffer
void screen_draw_string(uint8_t x, uint8_t y, const char *text); //draws the text into the buffer 6px wide, 8px tall

#endif /* SCREEN_SCREEN_H_ */
