
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

#define GRAPHICS_MODE_320_200_4_CGA    0x05
#define GRAPHICS_MODE_320_200_256_VGA  0x13

#define GRAPHICS_MODE_320_200_256_VGA_ADDR   0xA0000000L
#define GRAPHICS_MODE_320_200_4_CGA_ADDR     0xB0000000L

void graphics_init( uint8_t );
void graphics_shutdown();
void graphics_draw_px( uint16_t, uint16_t, uint8_t );
void graphics_draw_block( uint16_t, uint16_t, uint16_t, uint16_t, uint8_t );
void graphics_sprite_at( char[], uint16_t, uint16_t, uint8_t );
void graphics_char_at( char, uint16_t, uint16_t, uint8_t );
void graphics_string_at( char*, uint16_t, uint16_t, uint16_t, uint8_t );

#endif /* GRAPHICS_H */

