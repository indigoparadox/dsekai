
#ifndef CONFIG_H
#define CONFIG_H

#define TILEMAP_TW 20
#define TILEMAP_TH 11

#define TILEMAP_TILESETS_MAX  12

#ifndef SCREEN_W
#define SCREEN_W 320
#endif /* SCREEN_W */

#ifndef SCREEN_H
#define SCREEN_H 200
#endif /* SCREEN_H */

#ifdef USE_DOS

#include <assert.h>

#elif defined( USE_SDL )

#include <assert.h>
#define USE_FAKE_CGA

#elif defined( USE_PALM )

#ifdef NDEBUG
#define assert( comp )
#else
/* Fake assert. */
#define assert( comp ) \
   if( !(comp) ) { \
      g_assert_failed_len = psprintf( \
         g_assert_failed, 255, \
         "%s, %d: ASSERT FALED", __FUNCTION__, __LINE__ ); \
      g_assert_failed_len = 1 / 0; \
   }
#endif /* !NDEBUG */
//#define USE_FAKE_CGA

#endif /* USE_DOS, USE_SDL, USE_PALM */

#define FONT_W 8
#define FONT_H 8
#define FONT_SPACE 0
#define SPRITE_H 16
#define SPRITE_W 16
#define SPRITE_TYPE uint32_t
#define TILE_W 16
#define TILE_H 16
#define TILE_TYPE uint32_t
#define PATTERN_W 8
#define PATTERN_H 8
#define PATTERN_TYPE uint16_t

#define WINDOW_TEXT_X 8
#define WINDOW_TEXT_Y 8

#define MOBILES_MAX 10
#define WINDOW_STRINGS_MAX 5
#define WINDOW_STRING_LEN_MAX 64

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

#define GRAPHICS_MODE_320_200_4_CGA    0x05
#define GRAPHICS_MODE_320_200_256_VGA  0x13

#define GRAPHICS_MODE_320_200_256_VGA_ADDR   0xA0000000L
#define GRAPHICS_MODE_320_200_4_CGA_ADDR     0xB8000000L

#ifndef GRAPHICS_MODE
#define GRAPHICS_MODE      0x05
#endif /* !GRAPHICS_MODE */

#if GRAPHICS_MODE_320_200_4_CGA == GRAPHICS_MODE
#define GRAPHICS_ADDR     GRAPHICS_MODE_320_200_4_CGA_ADDR
#elif GRAPHICS_MODE_320_200_256_VGA == GRAPHICS_MODE
#define GRAPHICS_ADDR     GRAPHICS_MODE_320_200_256_VGA_ADDR
#endif /* GRAPHICS_MODE */

#ifndef MAIN_C
extern char g_assert_failed[];
extern int g_assert_failed_len;
#endif /* !MAIN_C */

#endif /* CONFIG_H */

