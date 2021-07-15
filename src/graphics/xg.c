
#define GRAPHICS_C
#include "../dstypes.h"

Display* g_display = NULL;
Window g_window;

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int16_t graphics_platform_init( struct GRAPHICS_ARGS* args ) {
   int screen = 0;

   g_display = XOpenDisplay( NULL );
   if( NULL == g_display ) {
      error_printf( "unable to open display" );
      return 0;
   }

   g_window = XCreateSimpleWindow(
      g_display,
      RootWindow( g_display, screen ),
      100, 100,
      SCREEN_REAL_W,
      SCREEN_REAL_H,
      1,
      BlackPixel( g_display, screen ),
      WhitePixel( g_display, screen ) );

   /* TODO: Check? */

   XMapWindow( g_display, g_window );

   return 1;
}

void graphics_platform_shutdown( struct GRAPHICS_ARGS* args ) {
}

void graphics_flip( struct GRAPHICS_ARGS* args ) {
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
   const struct GRAPHICS_BITMAP* b,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   return 0;
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
}

void graphics_draw_rect(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   uint16_t thickness, const GRAPHICS_COLOR color
) {
}

void graphics_draw_line(
   uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
   uint16_t thickness, const GRAPHICS_COLOR color
) {
}

int32_t graphics_load_bitmap( uint32_t id_in, struct GRAPHICS_BITMAP* b ) {
   return 0;
}

int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   return 0;
}

