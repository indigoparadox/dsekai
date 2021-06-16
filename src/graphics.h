
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "config.h"
#include "dstypes.h"

#ifdef USE_DOS
#include "graphics/dosg.h"
#elif defined( USE_SDL )
#include "graphics/sdlg.h"
#endif /* USE_DOS, USE_SDL */

void graphics_init();
void graphics_shutdown();
void graphics_flip();
void graphics_loop_start();
void graphics_loop_end();
void graphics_draw_px( uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_draw_block(
   uint16_t, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_tile_at( const GRAPHICS_TILE*, uint16_t, uint16_t );
void graphics_sprite_at( const GRAPHICS_SPRITE*, uint16_t, uint16_t );
void graphics_char_at(
   const char, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );
void graphics_string_at(
   const char*, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );

#endif /* GRAPHICS_H */

