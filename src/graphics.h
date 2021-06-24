
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "dstypes.h"

#ifdef USE_DOS
#include "graphics/dosg.h"
#elif defined( USE_SDL )
#include "graphics/sdlg.h"
#elif defined( USE_PALM )
#include "graphics/palmg.h"
#elif defined( USE_WIN16 )
#include "graphics/win16g.h"
#elif defined( USE_NULL )
#include "graphics/nullg.h"
#endif /* USE_DOS, USE_SDL, USE_PALM, USE_WIN16, USE_NULL */

#if 0
#define graphics_pattern_masked_at( spr, mask, mo_x, mo_y, x, y ) \
   graphics_blit_masked_at( \
      (const GRAPHICS_PATTERN*)spr, mask, mo_x, mo_y, \
      x, y, PATTERN_W, PATTERN_H, sizeof( PATTERN_TYPE ) )
#endif

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
   const char*, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );
void graphics_blit_at( const struct GRAPHICS_BITMAP*,
   uint16_t, uint16_t, uint16_t, uint16_t );
int32_t graphics_load_bitmap( uint32_t, struct GRAPHICS_BITMAP** );
int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP** );

#if 0
void graphics_blit_masked_at( const GRAPHICS_PATTERN*, const GRAPHICS_MASK*,
   uint8_t, uint8_t, uint16_t, uint16_t, uint8_t, uint8_t, const int );
#endif

#endif /* GRAPHICS_H */

