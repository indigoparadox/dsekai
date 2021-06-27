
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "dstypes.h"

#ifdef PLATFORM_DOS
#include "graphics/dosg.h"
#elif defined( PLATFORM_SDL )
#include "graphics/sdlg.h"
#elif defined( PLATFORM_PALM )
#include "graphics/palmg.h"
#elif defined( PLATFORM_WIN16 )
#include "graphics/win16g.h"
#elif defined( PLATFORM_MAC7 )
#include "graphics/mac7g.h"
#else
#include "graphics/nullg.h"
#endif /* PLATFORM_DOS, PLATFORM_SDL, PLATFORM_PALM, PLATFORM_WIN16 */

/*
typedef struct GRAPHICS_MASK {uint8_t bits[16]; } GRAPHICS_MASK;
typedef struct GRAPHICS_PATTERN {
   PATTERN_TYPE bits[PATTERN_H]; } GRAPHICS_PATTERN;

#define graphics_pattern_masked_at( spr, mask, mo_x, mo_y, x, y ) \
   graphics_blit_masked_at( \
      (const GRAPHICS_PATTERN*)spr, mask, mo_x, mo_y, \
      x, y, PATTERN_W, PATTERN_H )

#define graphics_pattern_at( spr, x, y ) \
   graphics_blit_at( spr, x, y, PATTERN_W, PATTERN_H )
*/

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
int32_t graphics_load_bitmap( uint32_t, struct GRAPHICS_BITMAP* );
int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* );
void graphics_blit_masked_at(
   const struct GRAPHICS_BITMAP*, const uint8_t*, uint16_t,
   uint16_t, uint16_t, uint16_t, uint16_t );

#endif /* GRAPHICS_H */

