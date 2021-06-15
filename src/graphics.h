
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "dstypes.h"

#ifdef __DOS__
#include "graphics/dosg.h"
#else
#include "graphics/sdlg.h"
#endif /* __DOS__ */

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
void graphics_flip();
void graphics_draw_px( uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_draw_block(
   uint16_t, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_sprite_at(
   const uint8_t[SPRITE_H], uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_char_at( const char, uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_string_at(
   const char*, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR );

#endif /* GRAPHICS_H */

