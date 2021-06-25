
#include "../graphics.h"

static BitmapType* g_screen = NULL;
static WinHandle g_win;

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
}

void graphics_shutdown() {
}

void graphics_flip() {
}

void graphics_loop_start() {
}

void graphics_loop_end() {
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
   if( NULL == bmp | NULL == bmp->bitmap ) {
      WinDrawChars( "X", 1, x, y );
      return;
   }

   WinDrawBitmap( bmp->bitmap, x, y );
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   GRAPHICS_COLOR color
) {
}

/*
 * @return 1 if bitmap is loaded and 0 otherwise.
 */
int32_t graphics_load_bitmap( uint32_t id, struct GRAPHICS_BITMAP** b ) {
   int retval = 1;

   /* assert( NULL != b ); */
   /* assert( NULL == *b ); */

   *b = memory_alloc( 1, sizeof( struct GRAPHICS_BITMAP ) );
   if( 0 != (*b)->ref_count ) {
      retval = 1;
      goto cleanup;
   }
   (*b)->ref_count++;

   (*b)->handle = DmGetResource( 'Tbmp', id );
   if( NULL == (*b)->handle ) {
      retval = 0;
      goto cleanup;
   }

   (*b)->bitmap = MemHandleLock( (*b)->handle );
   if( NULL == (*b)->bitmap ) {
      retval = 0;
      goto cleanup;
   }

cleanup:
   return retval;
}

/*
 * @return 1 if bitmap is unloaded and 0 otherwise.
 */
int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP** b ) {
   assert( NULL != *b );
   (*b)->ref_count--;
   if( 0 >= (*b)->ref_count ) {
      MemHandleUnlock( (*b)->handle );
      memory_free( b );
      return 1;
   }
   return 0;
}

