
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

#define graphics_pattern_masked_at( spr, mask, mo_x, mo_y, x, y ) \
   graphics_blit_masked_at( \
      (const GRAPHICS_BITMAP*)spr, mask, mo_x, mo_y, \
      x, y, PATTERN_W, PATTERN_H, sizeof( PATTERN_TYPE ) )

typedef struct GRAPHICS_MASK {uint8_t bits[16]; } GRAPHICS_MASK;
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
void graphics_char_at(
   const char, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );
void graphics_string_at(
   const char*, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );
void graphics_blit_at( const GRAPHICS_BITMAP*,
   uint16_t, uint16_t, uint8_t, uint8_t, const int );
void graphics_blit_masked_at( const GRAPHICS_BITMAP*, const GRAPHICS_MASK*,
   uint8_t, uint8_t, uint16_t, uint16_t, uint8_t, uint8_t, const int );

#endif /* GRAPHICS_H */

