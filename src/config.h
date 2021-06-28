
#ifndef CONFIG_H
#define CONFIG_H

#define TILEMAP_TW 80
#define TILEMAP_TH 44

#define TILEMAP_TILESETS_MAX  12

#ifndef SCREEN_W
#define SCREEN_W 320
#endif /* !SCREEN_W */

#ifndef SCREEN_H
#define SCREEN_H 200
#endif /* !SCREEN_H */

#ifndef NAMEBUF_MAX
#define NAMEBUF_MAX 255
#endif /* !NAMEBUF_MAX */

#ifndef DRC_COPY_BLOCK_SZ
#define DRC_COPY_BLOCK_SZ 1024
#endif /* !DRC_COPY_BLOCK_SZ */

#ifndef DRC_BMP_TYPE
#define DRC_BMP_TYPE "BMP1"
#endif /* DRC_BMP_TYPE */

#ifndef MOBILES_MAX
#define MOBILES_MAX 10
#endif /* !MOBILES_MAX */

#ifndef DEBUG_THRESHOLD
#define DEBUG_THRESHOLD 2
#endif /* !DEBUG_THRESHOLD */

#ifndef DIRTY_THRESHOLD
#define DIRTY_THRESHOLD 3
#endif /* DIRTY_THRESHOLD */

#ifdef DEBUG_LOG

#include <stdio.h>

#define internal_debug_printf( lvl, ... ) \
   if( lvl >= DEBUG_THRESHOLD ) { \
      printf( "(%d) " __FILE__ ": %d: ", lvl, __LINE__ ); \
      printf( __VA_ARGS__ ); \
      printf( "\n" ); \
   }

#define internal_error_printf( ... ) \
   fprintf( stderr, __FILE__ ": %d: ", __LINE__ ); \
   fprintf( stderr, __VA_ARGS__ ); \
   fprintf( stderr, "\n" ); \

#define debug_printf( lvl, ... ) \
   internal_debug_printf( lvl, __VA_ARGS__ )

#define error_printf( ... ) \
   internal_error_printf( __VA_ARGS__ )

#else

#ifdef ANCIENT_C

#define debug_printf( x )
#define error_printf( x )

#else

#define debug_printf( ... )
#define error_printf( ... )

#endif /* ANCIENT_C */

#endif /* DEBUG_LOG */


/* ------ */
#ifdef PLATFORM_DOS
/* ------ */

#include <assert.h>

#ifdef ANCIENT_C
#
/* XXX */
#define DEBUG_THRESHOLD 2
#define DIRTY_THRESHOLD 3

#ifdef DEBUG_LOG

#define internal_debug_printf( lvl, ... ) \
   if( lvl >= DEBUG_THRESHOLD ) { \
      printf( "(%d) " __FILE__ ": %d: ", lvl, __LINE__ ); \
      printf( __VA_ARGS__ ); \
      printf( "\n" ); \
   }

#define debug_printf( lvl, ... ) \
   internal_debug_printf( lvl, __VA_ARGS__ )

#else

#endif /* DEBUG_LOG */

include <stdio.h>
#define DIO_SILENT
#define NO_VARGS
#define USE_LOOKUPS
#define NO_I86
#endif /* ANCIENT_C */

#include "../gen/dos/resext.h"

#define GRAPHICS_M_320_200_4_CGA  0x05
#define GRAPHICS_M_320_200_256_V  0x13

#define GRAPHICS_M_320_200_256_VGA_A   0xA0000000L
#define GRAPHICS_M_320_200_4_CGA_A     0xB8000000L

#ifndef GRAPHICS_MODE
#define GRAPHICS_MODE      0x05
#endif /* !GRAPHICS_MODE */

#if GRAPHICS_M_320_200_4_CGA == GRAPHICS_MODE
#define GRAPHICS_ADDR     GRAPHICS_M_320_200_4_CGA_A
#elif GRAPHICS_M_320_200_256_VGA == GRAPHICS_MODE
#define GRAPHICS_ADDR     GRAPHICS_M_320_200_256_VGA_A
#endif /* GRAPHICS_MODE */

#ifndef DRC_ARCHIVE
#define DRC_ARCHIVE "doscga.drc"
#endif /* !DRC_ARCHIVE */

/* ------ */
#elif defined( PLATFORM_SDL )
/* ------ */

#include <assert.h>

#include "../gen/sdl/resext.h"

#ifndef DRC_ARCHIVE
#define DRC_ARCHIVE "sdl16.drc"
#endif /* !DRC_ARCHIVE */

/* ------ */
#elif defined( PLATFORM_PALM )
/* ------ */

#ifdef NDEBUG
#define assert( comp )
#else
/* Fake assert. */
#define assert( comp ) if( !(comp) ) { g_assert_failed_len = dio_snprintf( g_assert_failed, 255, __FILE__ ": %d: ASSERT FAILED", __LINE__ ); }
#endif /* !NDEBUG */

#include "../gen/palm/resext.h"

/* ------ */
#elif defined( PLATFORM_WIN16 )
/* ------ */

#include <windows.h>

#include <assert.h>

#include "../gen/win16/resext.h"

/* ------ */
#elif defined( PLATFORM_MAC7 )
/* ------ */

#include <assert.h>

#include "../gen/mac7/resext.h"

#ifndef DRC_ARCHIVE
#define DRC_ARCHIVE "mac7.drc"
#endif /* !DRC_ARCHIVE */

/* ------ */
#else /* !PLATFORM_DOS, !PLATFORM_SDL, !PLATFORM_PALM, !PLATFORM_WIN16 */
/* ------ */

#include <assert.h>

#ifndef NO_RESEXT
#include "../gen/sdl/resext.h"
#endif /* NO_RESEXT */

/* ------ */
#endif /* PLATFORM_DOS, PLATFORM_SDL, PLATFORM_PALM, PLATFORM_WIN16 */
/* ------ */

/* Graphics Parameters */

#define FONT_W 8
#define FONT_H 8
#define FONT_SPACE 0
#define SPRITE_H 16
#define SPRITE_W 16
#define TILE_W 16
#define TILE_H 16
#define PATTERN_W 16
#define PATTERN_H 16

#define WINDOW_TEXT_X 8
#define WINDOW_TEXT_Y 8
#define WINDOW_STRINGS_MAX 5
#define WINDOW_STRING_LEN_MAX 64

/* Derived Parameters */

#define SCREEN_TW (SCREEN_W / TILE_W)
#define SCREEN_TH (SCREEN_H / TILE_H)

#ifndef SCREEN_SCALE
#define SCREEN_SCALE 1
#endif /* SCREEN_SCALE */

#define SCREEN_REAL_W SCREEN_W * SCREEN_SCALE
#define SCREEN_REAL_H SCREEN_H * SCREEN_SCALE

#ifndef FPS
#define FPS 30
#endif /* FPS */

#ifndef DIO_READ_FILE_BLOCK_SZ
#define DIO_READ_FILE_BLOCK_SZ 4096
#endif /* !DIO_READ_FILE_BLOCK_SZ */

#ifndef MAIN_C
extern char g_assert_failed[];
extern int g_assert_failed_len;
#endif /* !MAIN_C */

#endif /* CONFIG_H */

