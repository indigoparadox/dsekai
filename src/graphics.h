
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "config.h"
#include "dstypes.h"

#ifdef USE_DOS
#include "graphics/dosg.h"
#elif defined( USE_SDL )
#include "graphics/sdlg.h"
#endif /* USE_DOS, USE_SDL */

#define graphics_sprite_at( spr, x, y ) \
   graphics_blit_at( \
      (const GRAPHICS_BITMAP*)spr, x, y, SPRITE_W, SPRITE_H - 1, \
      sizeof( SPRITE_TYPE ) )
#define graphics_tile_at( spr, x, y ) \
   graphics_blit_at( \
      (const GRAPHICS_BITMAP*)spr, x, y, TILE_W, TILE_H, \
      sizeof( TILE_TYPE ) )
#define graphics_pattern_at( spr, x, y ) \
   graphics_blit_at( \
      (const GRAPHICS_BITMAP*)spr, x, y, PATTERN_W, PATTERN_H, \
      sizeof( PATTERN_TYPE ) )

typedef struct GRAPHICS_BITMAP { uint8_t bits[1]; } GRAPHICS_BITMAP;
typedef struct GRAPHICS_SPRITE { SPRITE_TYPE bits[SPRITE_H]; } GRAPHICS_SPRITE;
typedef struct GRAPHICS_TILE { TILE_TYPE bits[TILE_H]; } GRAPHICS_TILE;
typedef struct GRAPHICS_PATTERN {
   PATTERN_TYPE bits[PATTERN_H]; } GRAPHICS_PATTERN;

void graphics_init();
void graphics_shutdown();
void graphics_flip();
void graphics_loop_start();
void graphics_loop_end();
void graphics_draw_px( uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_draw_block(
   uint16_t, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR );
#if 0
ndef USE_SDL
void graphics_pattern_at( const GRAPHICS_PATTERN* uint16_t, uint16_t );
void graphics_tile_at( const GRAPHICS_TILE*, uint16_t, uint16_t );
void graphics_sprite_at( const GRAPHICS_SPRITE*, uint16_t, uint16_t );
#endif /* USE_SDL */
void graphics_char_at(
   const char, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );
void graphics_string_at(
   const char*, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );
void graphics_blit_at( const GRAPHICS_BITMAP*,
   uint16_t, uint16_t, uint8_t, uint8_t, const int );

#endif /* GRAPHICS_H */

