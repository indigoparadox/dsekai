
#ifndef CONFIG_H
#define CONFIG_H

/* Do platform tests first, so they can force options below if they need to. */

/* ------ */
#ifdef PLATFORM_DOS
/* ------ */

#include "../gen/dos/resext.h"

#ifndef DIO_PATH_TEMP
#define DIO_PATH_TEMP "c:\\temp"
#endif /* !DIO_PATH_TEMP */

#ifndef DIO_PATH_SEP
#define DIO_PATH_SEP '\\'
#endif /* !DIO_PATH_SEP */

#define GRAPHICS_M_320_200_4_CGA  0x05
#define GRAPHICS_M_320_200_256_V  0x13

#define GRAPHICS_M_320_200_256_VGA_A   0xA0000000L
#define GRAPHICS_M_320_200_4_CGA_A     0xB8000000L

#ifndef GRAPHICS_MODE
#define GRAPHICS_MODE      0x05
#endif /* !GRAPHICS_MODE */

#ifndef CGA_COLD
#define CGA_COLD           0x01
#endif /* CGA_COLD */

#if GRAPHICS_M_320_200_4_CGA == GRAPHICS_MODE
#define GRAPHICS_ADDR     GRAPHICS_M_320_200_4_CGA_A
#elif GRAPHICS_M_320_200_256_VGA == GRAPHICS_MODE
#define GRAPHICS_ADDR     GRAPHICS_M_320_200_256_VGA_A
#endif /* GRAPHICS_MODE */

#ifndef DRC_ARCHIVE
#define DRC_ARCHIVE "doscga.drc"
#endif /* !DRC_ARCHIVE */

#define USE_SOFTWARE_TEXT
#define MEMORY_CALLOC

/* ------ */
#elif defined( PLATFORM_SDL )
/* ------ */

#include "../gen/sdl/resext.h"

#ifndef DIO_PATH_TEMP
#define DIO_PATH_TEMP "/tmp"
#endif /* !DIO_PATH_TEMP */

#ifndef DIO_PATH_SEP
#define DIO_PATH_SEP '/'
#endif /* !DIO_PATH_SEP */

#ifndef DRC_ARCHIVE
#define DRC_ARCHIVE "sdl16.drc"
#endif /* !DRC_ARCHIVE */

#define USE_SOFTWARE_TEXT
#define MEMORY_CALLOC

/* ------ */
#elif defined( PLATFORM_PALM )
/* ------ */

#include "../gen/palm/resext.h"

#define USE_SOFT_ASSERT
#define MEMORY_STATIC
#define DISABLE_FILESYSTEM

/* ------ */
#elif defined( PLATFORM_WIN16 )
/* ------ */

#include <windows.h>

#include "../gen/win16/resext.h"

#ifndef DIO_PATH_TEMP
#define DIO_PATH_TEMP "c:\\temp"
#endif /* !DIO_PATH_TEMP */

#ifndef DIO_PATH_SEP
#define DIO_PATH_SEP '\\'
#endif /* !DIO_PATH_SEP */

#define USE_SOFTWARE_TEXT
#define MEMORY_CALLOC

#define SCREEN_BPP 4

/* ------ */
#elif defined( PLATFORM_MAC7 )
/* ------ */

#include "../gen/mac7/resext.h"

#ifndef DIO_PATH_SEP
#define DIO_PATH_SEP ':'
#endif /* !DIO_PATH_SEP */

#ifndef DRC_ARCHIVE
#define DRC_ARCHIVE "mac7.drc"
#endif /* !DRC_ARCHIVE */

#define USE_SOFTWARE_TEXT
/* #define MEMORY_STATIC */
#define DRC_TOC_INITIAL_ALLOC 20 /* Fake it until we have realloc. */

/* ------ */
#else /* !PLATFORM_DOS, !PLATFORM_SDL, !PLATFORM_PALM, !PLATFORM_WIN16 */
/* ------ */

#ifndef DIO_PATH_TEMP
#define DIO_PATH_TEMP "/tmp"
#endif /* !DIO_PATH_TEMP */

#ifndef DIO_PATH_SEP
#define DIO_PATH_SEP '/'
#endif /* !DIO_PATH_SEP */

#ifndef DRC_ARCHIVE
#define DRC_ARCHIVE "null.drc"
#endif /* !DRC_ARCHIVE */

#ifndef NO_RESEXT
#include "../gen/check_null/resext.h"
#endif /* NO_RESEXT */

#define USE_SOFTWARE_TEXT

/* ------ */
#endif /* PLATFORM_DOS, PLATFORM_SDL, PLATFORM_PALM, PLATFORM_WIN16 */
/* ------ */

#define TILEMAP_TW (60)
#define TILEMAP_TH (60)

#define TILEMAP_TILESETS_MAX  (12)

#ifndef SCREEN_W
#define SCREEN_W (320)
#endif /* !SCREEN_W */

#ifndef SCREEN_H
#define SCREEN_H (200)
#endif /* !SCREEN_H */

#ifndef SCREEN_SCALE
#define SCREEN_SCALE (1)
#endif /* SCREEN_SCALE */

#ifndef FPS
#define FPS (30)
#endif /* FPS */

#ifndef DIO_READ_FILE_BLOCK_SZ
#define DIO_READ_FILE_BLOCK_SZ (4096)
#endif /* !DIO_READ_FILE_BLOCK_SZ */

#ifndef NAMEBUF_MAX
#define NAMEBUF_MAX (255)
#endif /* !NAMEBUF_MAX */

#ifndef DRC_COPY_BLOCK_SZ
#define DRC_COPY_BLOCK_SZ (1024)
#endif /* !DRC_COPY_BLOCK_SZ */

#ifndef DRC_BMP_TYPE
#define DRC_BMP_TYPE "BMP1"
#endif /* DRC_BMP_TYPE */

#ifndef DRC_MAP_TYPE
#define DRC_MAP_TYPE "TMAP"
#endif /* DRC_BMP_TYPE */

#ifndef MOBILES_MAX
#define MOBILES_MAX (10)
#endif /* !MOBILES_MAX */

#ifndef DEBUG_THRESHOLD
#define DEBUG_THRESHOLD (2)
#endif /* !DEBUG_THRESHOLD */

#ifndef DIRTY_THRESHOLD
#define DIRTY_THRESHOLD (3)
#endif /* !DIRTY_THRESHOLD */

#ifndef FAKE_HEAP_SIZE
#define FAKE_HEAP_SIZE (524288)
#endif /* !FAKE_HEAP_SIZE */

#ifndef DRC_TOC_INITIAL_ALLOC
#define DRC_TOC_INITIAL_ALLOC 1
#endif /* !DRC_TOC_INITIAL_ALLOC */

#ifndef DIO_PATH_MAX
#define DIO_PATH_MAX 254
#endif /* DIO_PATH_MAX */

/* ! */
#ifdef DEBUG_LOG
/* ! */

#  include <stdio.h>

#  define internal_debug_printf( lvl, ... ) \
      if( lvl >= DEBUG_THRESHOLD ) { \
         printf( "(%d) " __FILE__ ": %d: ", lvl, __LINE__ ); \
         printf( __VA_ARGS__ ); \
         printf( "\n" ); \
         fflush( stdout ); \
      }

#  define internal_error_printf( ... ) \
      fprintf( stderr, __FILE__ ": %d: ", __LINE__ ); \
      fprintf( stderr, __VA_ARGS__ ); \
      fprintf( stderr, "\n" ); \

#  define debug_printf( lvl, ... ) \
      internal_debug_printf( lvl, __VA_ARGS__ )

#  define error_printf( ... ) \
      internal_error_printf( __VA_ARGS__ )

/* ! */
#elif defined( ANCIENT_C )
/* ! */

#  define debug_printf( x )
#  define error_printf( x )

/* ! */
#else /* !DEBUG_LOG, !ANCIENT_C */
/* ! */

#  define debug_printf( ... )
#  define error_printf( ... )

/* ! */
#endif /* DEBUG_LOG, ANCIENT_C */
/* ! */

#ifdef ANCIENT_C
#  include <stdio.h>
#  define DIO_SILENT
#  define NO_VARGS
#  define NO_I86
#  define NO_CGA_FUNCTIONS
#endif /* ANCIENT_C */

/* ! */
#ifdef USE_SOFT_ASSERT
/* ! */

#  define assert( comp ) if( !(comp) ) { g_assert_failed_len = dio_snprintf( g_assert_failed, 255, __FILE__ ": %d: ASSERT FAILED", __LINE__ ); }

#else

#include <assert.h>

/* ! */
#endif /* USE_SOFT_ASSERT */
/* ! */

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

#define SCREEN_REAL_W (SCREEN_W * SCREEN_SCALE)
#define SCREEN_REAL_H (SCREEN_H * SCREEN_SCALE)

#ifndef MAIN_C
extern char g_assert_failed[];
extern int g_assert_failed_len;
#endif /* !MAIN_C */

#endif /* CONFIG_H */

