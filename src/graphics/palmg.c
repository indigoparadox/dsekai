
#include "../graphics.h"
#include "../data/dio.h"

static BitmapType* g_screen = NULL;
static WinHandle g_win;
static uint32_t g_ticks_start = 0;
static int16_t g_ticks_target = 0;

static int16_t graphics_load_bitmap_surface( struct GRAPHICS_BITMAP* b ) {
   int16_t retval = 1;
   struct DIO_RESOURCE rsc;

   if( NULL == b ) {
      retval = 0;
      goto cleanup;
   }

   retval = dio_get_resource_handle( b->id, 'Tbmp', &rsc );
   if( !retval ) {
      goto cleanup;
   }

   b->handle = rsc.handle;
   b->bitmap = rsc.ptr;

cleanup:
   return retval;
}

static void graphics_unload_bitmap_surface( struct GRAPHICS_BITMAP* b ) {
   struct DIO_RESOURCE rsc;

   if( NULL != b->handle ) {
      MemHandleUnlock( b->handle );
      b->handle = NULL;
   }

   if( NULL != b->bitmap ) {
      /* TODO: Free bitmap? Don't need to, right? */
   }
}

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int graphics_init() {
   g_ticks_target = SysTicksPerSecond();
   return 1;
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

int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   int retval = 1;
   struct DIO_RESOURCE rsrc;

   memset( &rsrc, '\0', sizeof( struct DIO_RESOURCE ) );

   if( NULL == bmp ) {
      WinDrawChars( "X", 1, x, y );
      retval = 0;
      goto cleanup;
   }

   memset( &rsrc, '\0', sizeof( struct DIO_RESOURCE ) );
   retval = dio_get_resource_handle( bmp->id, 'Tbmp', &rsrc );
   if( !retval ) {
      goto cleanup;
   }

   if( NULL == bmp->bitmap ) {
      WinDrawChars( "Z", 1, x, y );
      retval = 0;
      goto cleanup;
   }

   WinDrawBitmap( (BitmapPtr)rsrc.ptr, x, y );

cleanup:

   dio_free_resource_handle( &rsrc );

   return retval;
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

#ifndef USE_SOFTWARE_TEXT

void graphics_string_at(
   const char* s, uint16_t s_len, uint16_t x_orig, uint16_t y_orig,
   GRAPHICS_COLOR color, uint8_t scale
) {
   WinDrawChars( s, s_len, x_orig, y_orig );
}

#endif /* !USE_SOFTWARE_TEXT */

