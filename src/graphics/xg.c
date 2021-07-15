
#define GRAPHICS_C
#include "../dstypes.h"

#include "../../tools/data/bmp.h"

#include <stdlib.h>
#include <X11/Xutil.h>

Display* g_display = NULL;
int g_screen = 0;
Window g_window;
Visual* g_visual;

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int16_t graphics_platform_init( struct GRAPHICS_ARGS* args ) {

   g_display = XOpenDisplay( NULL );
   if( NULL == g_display ) {
      error_printf( "unable to open display" );
      return 0;
   }

   g_screen = DefaultScreen( g_display );

   g_window = XCreateSimpleWindow(
      g_display,
      RootWindow( g_display, g_screen ),
      100, 100,
      SCREEN_REAL_W,
      SCREEN_REAL_H,
      1,
      BlackPixel( g_display, g_screen ),
      WhitePixel( g_display, g_screen ) );

   g_visual = DefaultVisual( g_display, g_screen );

   /* TODO: Check? */

   XMapWindow( g_display, g_window );

   return 1;
}

void graphics_platform_shutdown( struct GRAPHICS_ARGS* args ) {
   XCloseDisplay( g_display );
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
   int retval = 1;

   XPutImage(
      g_display,
      g_window,
      DefaultGC( g_display, g_screen ),
      b->pixmap,
      0, 0, x, y, w, h );

   return retval;
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
   struct BITMAP_FILE_HEADER* file_header = NULL;
   struct BITMAP_DATA_HEADER* data_header = NULL;
   uint8_t* bitmap_bits = NULL;
   uint8_t* buffer = NULL;
   RESOURCE_BITMAP_HANDLE buffer_handle = NULL;
   uint32_t id = 0,
      retval = 1,
      buffer_sz = 0,
      px = 0;
   int
      x = 0,
      y = 0;

   assert( NULL != b );
   assert( 0 == b->ref_count );

   /* Load resource into buffer. */
   buffer_handle = resource_get_bitmap_handle( id );
   if( NULL == buffer_handle ) {
      retval = 0;
      error_printf( "unable to get resource handle" );
      goto cleanup;
   }

   buffer_sz = memory_sz( buffer_handle );
   buffer = resource_lock_handle( buffer_handle );

   file_header = (struct BITMAP_FILE_HEADER*)buffer;
   data_header =
      (struct BITMAP_DATA_HEADER*)&(buffer[sizeof( struct BITMAP_FILE_HEADER )]);

   assert( 'B' == file_header->id[0] );
   assert( 'M' == file_header->id[1] );

   debug_printf( 1, "bitmap is %d bytes long, data at %d bytes",
      file_header->file_sz, file_header->bmp_offset );

   bitmap_bits = &(buffer[file_header->bmp_offset]);

   b->bits = calloc( data_header->bitmap_w * data_header->bitmap_h, 4 );
   assert( NULL != b->bits );

   b->pixmap = XCreateImage(
      g_display, g_visual, 
      DefaultDepth( g_display, g_screen ),
      ZPixmap,
      0,
      b->bits,
      data_header->bitmap_w, data_header->bitmap_h, 32, 0 );
   assert( NULL != b->pixmap );

   /*
   assert( BadDrawable != b->pixmap );
   assert( BadGC != b->pixmap );
   assert( BadMatch != b->pixmap );
   assert( BadValue != b->pixmap );
   */

   for( y = 0 ; y < data_header->bitmap_h ; y++ ) {
      for( x = 0 ; x < data_header->bitmap_w ; x++ ) {
         assert( (y * data_header->bitmap_w) + x < file_header->file_sz );
         px = bitmap_bits[(y * data_header->bitmap_w) + x];
         XPutPixel( b->pixmap, x, y, px );
      }
   }

cleanup:

   if( NULL != buffer ) {
      buffer = resource_unlock_handle( buffer_handle );
   }

   if( NULL != buffer_handle ) {
      resource_free_handle( buffer_handle );
   }

   return retval;
}

int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   XDestroyImage( b->pixmap );
   free( b->bits );

   return 1;
}

