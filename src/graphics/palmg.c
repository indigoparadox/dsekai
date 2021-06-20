
#include "palmg.h"

#include <PalmOS.h>

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

void graphics_draw_px( uint16_t x, uint16_t y, GRAPHICS_COLOR color ) {

   if( SCREEN_H <= y || SCREEN_W <= x ) {
      return;
   }

#if 0
#ifdef PALM_USE_WIN
   UInt32* display = NULL;

   WinSetForeColor( color );
   WinDrawPixel( x, y );
#elif defined( PALM_USE_BMP )
   display = WinGetDisplayWindow()->displayAddrV20;

   *display = 0xffffffff;

#else
#endif
#endif
   WinSetForeColor( color );
   WinPaintPixel( x, y );
}

void graphics_blit_masked_at(
   const GRAPHICS_PATTERN* bmp, const GRAPHICS_MASK* mask,
   uint8_t mask_o_x, uint8_t mask_o_y,
   uint16_t x, uint16_t y, uint8_t w, uint8_t h, const int byte_width
) {
}

void graphics_blit_at(
   const GRAPHICS_BITMAP* bmp, uint16_t x, uint16_t y, uint8_t w, uint8_t h,
   const int bytes
) {
   MemHandle bitmap_h;
   BitmapPtr bitmap;

   if( NULL == bmp ) {
      WinDrawChars( "Z", 1, x, y );
      return;
   }

   if( NULL == *bmp ) {
      WinDrawChars( "Y", 1, x, y );
      return;
   }

   bitmap_h = DmGetResource( 'Tbmp', *bmp );

   if( NULL == bitmap_h ) {
      WinDrawChars( "X", 1, x, y );
      return;
   }

   bitmap = MemHandleLock( bitmap_h );
   WinDrawBitmap( bitmap, x, y );
   MemHandleUnlock( bitmap_h );

/*
   MemHandle bitmap_h;
   void* bitmap_p;
   BitmapPtr bitmap;

   if( NULL == bmp ) {
      WinDrawChars( "Z", 1, x, y );
      return;
   }

   if( NULL == *bmp ) {
      WinDrawChars( "Y", 1, x, y );
      return;
   }

   bitmap_h = DmGet1Resource( "Tbmp", *bmp );

   if( NULL == bitmap_h ) {
      WinDrawChars( "X", 1, x, y );
      return;
   }

   bitmap_p = MemHandleLock( bitmap_h );

   if( NULL == bitmap_p ) {
      WinDrawChars( "Q", 1, x, y );
      return;
   }

   WinDrawBitmap( bitmap_p, x, y );

   MemHandleUnlock( bitmap_p );
   */
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   GRAPHICS_COLOR color
) {
}

