
#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef PLATFORM_DOS
#include "graphics/dosg.h"
#elif defined( PLATFORM_SDL )
#include "graphics/sdlg.h"
#elif defined( PLATFORM_XLIB )
#include "graphics/xg.h"
#elif defined( PLATFORM_PALM )
#include "graphics/palmg.h"
#elif defined( PLATFORM_WIN )
#include "graphics/wing.h"
#elif defined( PLATFORM_MAC7 )
#include "graphics/mac7g.h"
#elif defined( PLATFORM_NDS )
#include "graphics/ndsg.h"
#elif defined( PLATFORM_GL )
#include "graphics/glg.h"
#else
#include "graphics/nullg.h"
#endif /* PLATFORM_DOS, PLATFORM_SDL, PLATFORM_PALM, PLATFORM_WIN16 */

#define GRAPHICS_MAX_PATH 32

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

struct GRAPHICS_RECT {
   int16_t x;
   int16_t y;
   int16_t w;
   int16_t h;
};

/**
 * This struct should be equivalent in size to whatever the bitmap struct for
 * the current platform looks like.
 */
struct GRAPHICS_BITMAP_BASE {
   RESOURCE_ID id;
   uint8_t initialized;
   uint16_t ref_count;

   /* Platform-Specific Overrides. */
   MEMORY_PTR ptr1;
   MEMORY_PTR ptr2;
   uint32_t res1;
   uint32_t res2;
   uint8_t res3;
   uint8_t res4;
};

int16_t graphics_init();
void graphics_shutdown();
void graphics_flip( struct GRAPHICS_ARGS* );
void graphics_loop_start();
void graphics_loop_end();
void graphics_draw_px( uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_draw_block(
   uint16_t, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_draw_rect(
   uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_draw_line(
   uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR );
void graphics_char_at(
   const char, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );
void graphics_string_at(
   const char*, uint16_t, uint16_t, uint16_t, const GRAPHICS_COLOR, uint8_t );
void graphics_string_sz(
   const char*, uint16_t, uint8_t, struct GRAPHICS_RECT* );
int16_t graphics_blit_at( RESOURCE_ID, uint16_t, uint16_t, uint16_t, uint16_t );
int16_t graphics_load_bitmap( RESOURCE_ID, struct GRAPHICS_BITMAP* );
int16_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* );
void graphics_blit_masked_at(
   const struct GRAPHICS_BITMAP*, const uint8_t*, uint16_t,
   uint16_t, uint16_t, uint16_t, uint16_t );
#ifdef GRAPHICS_C
int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP*, uint16_t, uint16_t, uint16_t, uint16_t );
int16_t graphics_platform_init( struct GRAPHICS_ARGS* );
void graphics_platform_shutdown( struct GRAPHICS_ARGS* );
int16_t graphics_platform_load_bitmap(
   RESOURCE_BITMAP_HANDLE, struct GRAPHICS_BITMAP* );
#endif /* GRAPHICS_C */

#endif /* GRAPHICS_H */

