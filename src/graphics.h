
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "dstypes.h"
#include "font8x8.h"

#define SCREEN_W 320
#define SCREEN_H 200
#define FONT_W 8
#define FONT_H 8
#define FONT_SPACE 1
#define SPRITE_H 8
#define SPRITE_W 8

void graphics_init();
void graphics_draw_px( uint16_t, uint16_t, uint8_t );
void graphics_draw_block( uint16_t, uint16_t, uint16_t, uint16_t, uint8_t );
void graphics_char_at( char, uint16_t, uint16_t, uint8_t );
void graphics_string_at( char*, uint16_t, uint16_t, uint16_t, uint8_t );

#endif /* GRAPHICS_H */

