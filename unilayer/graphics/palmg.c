
#define GRAPHICS_C
#include "../unilayer.h"

static BitmapType* g_screen = NULL;
static WinHandle g_win;
static uint32_t g_ticks_start = 0;
static int16_t g_ticks_target = 0;

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int16_t graphics_platform_init( struct GRAPHICS_ARGS* args ) {
   g_ticks_target = SysTicksPerSecond();
   return 1;
}

void graphics_platform_shutdown( struct GRAPHICS_ARGS* args ) {
}

void graphics_flip( struct GRAPHICS_ARGS* args ) {
}

void graphics_loop_start() {
   g_ticks_start = TimGetTicks();
}

void graphics_loop_end() {
   int16_t delta = 0;

   delta = SysTicksPerSecond() - (TimGetTicks() - g_ticks_start);

   if( 0 > delta || 150 < delta ) {
      return;
   }

   /* Sleep until ticks target. */
   SysTaskDelay( delta );
}

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {

   if( SCREEN_H <= y || SCREEN_W <= x ) {
      return;
   }

   WinSetForeColor( color );
   WinPaintPixel( x, y );
}

int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   int retval = 1;
   MEMORY_HANDLE rsrc = NULL;
   BitmapPtr ptr = NULL;

   if( NULL == bmp || !bmp->initialized ) {
      WinDrawChars( "X", 1, x, y );
      retval = 0;
      goto cleanup;
   }

   rsrc = resource_get_bitmap_handle( bmp->id );

   if( NULL == rsrc ) {
      WinDrawChars( "Z", 1, x, y );
      retval = 0;
      goto cleanup;
   }

   ptr = resource_lock_handle( rsrc );
   WinDrawBitmap( ptr, x, y );
   ptr = resource_unlock_handle( rsrc );

cleanup:

   if( NULL != rsrc ) {
      resource_free_handle( rsrc );
   }

   return retval;
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   GRAPHICS_COLOR color
) {
}

void graphics_draw_rect(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   uint16_t thickness, const GRAPHICS_COLOR color
) {
}

void graphics_draw_line(
   uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t thickness,
   const GRAPHICS_COLOR color
) {
}

/*
 * @return 1 if bitmap is loaded and 0 otherwise.
 */
int16_t graphics_platform_load_bitmap(
   RESOURCE_BITMAP_HANDLE res_handle, struct GRAPHICS_BITMAP* b
) {
   int retval = 1;

   if( 0 != b->ref_count ) {
      retval = 0;
      goto cleanup;
   }

   /* "Loading" happens in draw routine, since it's coming from RAM anyway. */

cleanup:
   return retval;
}

/*
 * @return 1 if bitmap is unloaded and 0 otherwise.
 */
int16_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   if( NULL == b ) {
      return 0;
   }
   b->ref_count--;
   if( 0 >= b->ref_count ) {
      b->initialized = 0;
      b->id = 0;
      return 1;
   }
   return 0;
}

#ifndef USE_SOFTWARE_TEXT

void graphics_string_at(
   const char* s, uint16_t s_len, uint16_t x_orig, uint16_t y_orig,
   GRAPHICS_COLOR color, uint8_t scale
) {
   WinDrawChars( s, s_len, x_orig, y_orig );
}

void graphics_string_sz(
   const char* str, uint16_t str_sz, uint8_t scale, struct GRAPHICS_RECT* sz_out
) {
   /* TODO */
}

#endif /* !USE_SOFTWARE_TEXT */

