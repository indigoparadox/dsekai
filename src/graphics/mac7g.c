
#include "../graphics.h"

#include <Quickdraw.h>
#include <Windows.h>
#include <MacMemory.h>
#include <Sound.h>
#include <Fonts.h>
#include <NumberFormatting.h>

QDGlobals g_qd;
WindowPtr g_window;
Rect g_window_rect;

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int graphics_init() {

   InitGraf( &g_qd.thePort );
   InitFonts();
   InitWindows();
   InitMenus();
   TEInit();
   InitDialogs( NULL );
   InitCursor();

   g_window_rect = qd.screenBits.bounds;
   InsetRect( &g_window_rect, 50, 50 );
   g_window = NewWindow( NULL, &g_window_rect, "\pdsekai", true, documentProc,
      (WindowPtr)(-1), false, 0 );
   if( !g_window ) {
      return -1;
   }
   SetPort( g_window );
   return 0;
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
}

/*
 * @return 1 if blit was successful and 0 otherwise.
 */
int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   return 0;
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
}

int32_t graphics_load_bitmap( uint32_t id, struct GRAPHICS_BITMAP* b ) {
}

int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
}

