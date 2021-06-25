
#include "../graphics.h"

static BitmapType* g_screen = NULL;
static WinHandle g_win;
static uint32_t g_ticks_start = 0;
static int16_t g_ticks_target = 0;

static int16_t graphics_load_bitmap_surface( struct GRAPHICS_BITMAP* b ) {
   int16_t retval = 1;

   if( NULL == b ) {
      retval = 0;
      goto cleanup;
   }

   b->handle = DmGetResource( 'Tbmp', b->id );
   if( NULL == b->handle ) {
      retval = 0;
      goto cleanup;
   }

   b->bitmap = MemHandleLock( b->handle );
   if( NULL == b->bitmap ) {
      retval = 0;
      if( NULL != b->handle ) {
         MemHandleUnlock( b->handle );
         b->handle = NULL;
      }
      goto cleanup;
   }

cleanup:
   return retval;
}

static void graphics_unload_bitmap_surface( struct GRAPHICS_BITMAP* b ) {
   if( NULL != b->handle ) {
      MemHandleUnlock( b->handle );
      b->handle = NULL;
   }

   if( NULL != b->bitmap ) {
      /* TODO: Free bitmap? Don't need to, right? */
   }
}

void graphics_init() {
#if PALM_USE_WIN
   Err error;

   g_screen = BmpCreate( SCREEN_W, SCREEN_H, 2, NULL, &error );
   if( g_screen ) {
      g_win = WinCreateBitmapWindow( g_screen, &error );
      if( g_win ) {
         WinSetDrawWindow( g_win );
      }
   }
#endif

   g_ticks_target = SysTicksPerSecond();
}

void graphics_shutdown() {
}

void graphics_flip() {
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

#if 0
void graphics_blit_masked_at(
   const GRAPHICS_PATTERN* bmp, const GRAPHICS_MASK* mask,
   uint8_t mask_o_x, uint8_t mask_o_y,
   uint16_t x, uint16_t y, uint8_t w, uint8_t h, const int byte_width
) {
}
#endif

void graphics_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   if( NULL == bmp ) {
      WinDrawChars( "X", 1, x, y );
      return;
   }

   graphics_load_bitmap_surface( (struct GRAPHICS_BITMAP*)bmp );

   if( NULL == bmp->bitmap ) {
      WinDrawChars( "Z", 1, x, y );
      return;
   }

   WinDrawBitmap( bmp->bitmap, x, y );

   graphics_unload_bitmap_surface( (struct GRAPHICS_BITMAP*)bmp );
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   GRAPHICS_COLOR color
) {
}

/*
 * @return 1 if bitmap is loaded and 0 otherwise.
 */
int32_t graphics_load_bitmap( uint32_t id, struct GRAPHICS_BITMAP* b ) {
   int retval = 1;

   if( 0 != b->ref_count ) {
      retval = 0;
      goto cleanup;
   }

   b->id = id;
   b->initialized = 1;

   /* "Loading" happens in draw routine, since it's coming from RAM anyway. */

   b->ref_count++;

cleanup:
   return retval;
}

/*
 * @return 1 if bitmap is unloaded and 0 otherwise.
 */
int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
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

