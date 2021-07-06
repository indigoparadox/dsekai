
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

#ifndef LOG_FILE_NAME
#define LOG_FILE_NAME "logdos.txt"
#endif /* !LOG_FILE_NAME */

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

#define LOG_TO_FILE
#define USE_SOFTWARE_TEXT

/* ------ */
#elif defined( PLATFORM_SDL )
/* ------ */

#include "../gen/sdl/resext.h"

/* Doesn't exist on this platform. */
#define far

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

/* ------ */
#elif defined( PLATFORM_PALM )
/* ------ */

#include <PalmOS.h>

#include "../gen/palm/resext.h"

#define far

#ifndef LOG_FILE_NAME
#define LOG_FILE_NAME "logpalm.txt"
#endif /* !LOG_FILE_NAME */

#define USE_SOFT_ASSERT
#define DISABLE_FILESYSTEM
#define LOG_TO_FILE

#define stringify_line( line ) #line

#define platform_file HostFILE*
#define platform_fprintf HostFPrintF
#define platform_fopen HostFOpen
#define platform_fflush HostFFlush
#define platform_fclose HostFClose

#define DRC_BITMAP_TYPE 'Tbmp'
#define DRC_MAP_TYPE 'json'

/* #define assert( test ) ErrFatalDisplayIf( !(test), __FILE__ ": " stringify_line( __LINE__ ) ": assert failure" ) */

/* ------ */
#elif defined( PLATFORM_WIN16 )
/* ------ */

#include <windows.h>

#include "../gen/win16/resext.h"

/* Doesn't exist on this platform. */
#define far

#ifndef DIO_PATH_TEMP
#define DIO_PATH_TEMP "c:\\temp"
#endif /* !DIO_PATH_TEMP */

#ifndef DIO_PATH_SEP
#define DIO_PATH_SEP '\\'
#endif /* !DIO_PATH_SEP */

#define DRC_BITMAP_TYPE 1
#define DRC_MAP_TYPE 2

#define USE_SOFTWARE_TEXT

#define SCREEN_BPP 4

/* ------ */
#elif defined( PLATFORM_MAC7 )
/* ------ */

#include "../gen/mac7/resext.h"

/* Doesn't exist on this platform. */
#define far

#ifndef DIO_PATH_TEMP
/* TODO */
#define DIO_PATH_TEMP ""
#endif /* !DIO_PATH_TEMP */

#ifndef DIO_PATH_SEP
#define DIO_PATH_SEP ':'
#endif /* !DIO_PATH_SEP */

#ifndef DRC_ARCHIVE
#define DRC_ARCHIVE "mac7.drc"
#endif /* !DRC_ARCHIVE */

#define platform_file FILE*
#define platform_fprintf fprintf
#define platform_fopen fopen
#define platform_fflush fflush
#define platform_fclose fclose

#define USE_SOFTWARE_TEXT
#define DRC_TOC_INITIAL_ALLOC 20 /* Fake it until we have realloc. */

/* ------ */
#else /* !PLATFORM_DOS, !PLATFORM_SDL, !PLATFORM_PALM, !PLATFORM_WIN16 */
/* ------ */

/* Doesn't exist on this platform. */
#define far

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

#define TILEMAP_TW (30)
#define TILEMAP_TH (30)

#define TILEMAP_TILESETS_MAX (12)

#ifndef platform_file
#define platform_file FILE*
#endif /* !platform_file */

#ifndef platform_fprintf
#define platform_fprintf fprintf
#endif /* !platform_fprintf */

#ifndef platform_fopen
#define platform_fopen fopen
#endif /* !platform_fopen */

#ifndef platform_fflush
#define platform_fflush fflush
#endif /* !platform_fflush */

#ifndef platform_fclose
#define platform_fclose fclose
#endif /* !platform_fclose */

#ifndef DRC_ARCHIVE_TYPE
#define DRC_ARCHIVE_TYPE {'D', 'R', 'C', 'T'}
#endif /* !DRC_ARCHIVE_TYPE */

#ifndef DRC_BITMAP_TYPE
#define DRC_BITMAP_TYPE {'B', 'M', 'P', '1'}
#endif /* !DRC_BITMAP_TYPE */

#ifndef DRC_MAP_TYPE
#define DRC_MAP_TYPE {'j', 's', 'o', 'n'}
#endif /* !DRC_MAP_TYPE */

#ifndef JSON_BUFFER_SZ
#define JSON_BUFFER_SZ 8192
#endif /* !JSON_BUFFER_SZ */

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

#ifndef MEMORY_FAKE_HEAP_SZ
#define MEMORY_FAKE_HEAP_SZ (112500)
#endif /* !MEMORY_FAKE_HEAP_S */

#ifndef DIO_READ_FILE_BLOCK_SZ
#define DIO_READ_FILE_BLOCK_SZ (4096)
#endif /* !DIO_READ_FILE_BLOCK_SZ */

#ifndef NAMEBUF_MAX
#define NAMEBUF_MAX (255)
#endif /* !NAMEBUF_MAX */

#ifndef DRC_COPY_BLOCK_SZ
#define DRC_COPY_BLOCK_SZ (1024)
#endif /* !DRC_COPY_BLOCK_SZ */

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

#ifdef LOG_TO_FILE
#ifndef DEBUG_LOG
#define DEBUG_LOG
#endif /* !DEBUG_LOG */
#define LOG_ERR_TARGET g_log_file
#define LOG_STD_TARGET g_log_file
#else
#define LOG_ERR_TARGET stderr
#define LOG_STD_TARGET stdout
#endif /* LOG_TO_FILE */

/* ! */
#ifdef DEBUG_LOG
/* ! */

#  include <stdio.h>

#  define internal_debug_printf( lvl, ... ) if( lvl >= DEBUG_THRESHOLD ) { platform_fprintf( LOG_STD_TARGET, "(%d) " __FILE__ ": %d: ", lvl, __LINE__ ); platform_fprintf( LOG_STD_TARGET, __VA_ARGS__ ); platform_fprintf( LOG_STD_TARGET, "\n" ); platform_fflush( LOG_STD_TARGET ); }

#  define internal_error_printf( ... ) platform_fprintf( LOG_ERR_TARGET, "(E) " __FILE__ ": %d: ", __LINE__ ); platform_fprintf( LOG_ERR_TARGET, __VA_ARGS__ ); platform_fprintf( LOG_ERR_TARGET, "\n" ); platform_fflush( LOG_ERR_TARGET );

#  define debug_printf( lvl, ... ) internal_debug_printf( lvl, __VA_ARGS__ )

#  define error_printf( ... ) internal_error_printf( __VA_ARGS__ )

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

#elif !defined( assert )

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

#ifdef LOG_TO_FILE
#ifdef MAIN_C
platform_file g_log_file = NULL;
#else /* !MAIN_C */
extern platform_file g_log_file;
#endif /* MAIN_C */
#endif /* LOG_TO_FILE */

#ifndef MAIN_C
extern char g_assert_failed[];
extern int g_assert_failed_len;
#endif /* !MAIN_C */

#endif /* CONFIG_H */

