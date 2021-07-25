
#define GRAPHICS_C
#include "../unilayer.h"

#include "../../tools/data/bmp.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xutil.h>

Display* g_display = NULL;
XImage* g_buffer = NULL;
int g_screen = 0;
Window g_window;
Visual* g_visual;
char* g_buffer_bits = NULL;
int g_buffer_bits_sz = 0;

volatile uint32_t g_ms;

const uint32_t gc_ns_target = 1000000000 / FPS;
static uint32_t g_ns_start = 0; 

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
      SCREEN_W,
      SCREEN_H,
      1,
      BlackPixel( g_display, g_screen ),
      WhitePixel( g_display, g_screen ) );

   g_visual = DefaultVisual( g_display, g_screen );

   /* TODO: Check? */

   /* Show the window. */
   XMapWindow( g_display, g_window );

   /* Create the buffer. */
   g_buffer_bits_sz = SCREEN_W * SCREEN_H * 4;
   g_buffer_bits = calloc( SCREEN_W * SCREEN_H, 4 );
   assert( NULL != g_buffer_bits );
   g_buffer = XCreateImage(
      g_display, g_visual, 
      DefaultDepth( g_display, g_screen ),
      ZPixmap,
      0,
      g_buffer_bits,
      SCREEN_W, SCREEN_H, 32, 0 );
   assert( NULL != g_buffer );

   return 1;
}

void graphics_platform_shutdown( struct GRAPHICS_ARGS* args ) {
   if( NULL != g_buffer ) {
      XDestroyImage( g_buffer );
   }
   /* if( NULL != g_buffer_bits ) {
      free( g_buffer_bits );
   } */
   XCloseDisplay( g_display );
}

void graphics_flip( struct GRAPHICS_ARGS* args ) {
   /* Draw the buffer to the window. */
   XPutImage(
      g_display,
      g_window,
      DefaultGC( g_display, g_screen ),
      g_buffer,
      0, 0, 0, 0, SCREEN_W, SCREEN_H );
}

void graphics_loop_start() {
   struct timespec spec;

   clock_gettime( CLOCK_MONOTONIC, &spec );

   g_ns_start = spec.tv_nsec;
}

void graphics_loop_end() {
   int16_t delta = 0;
   struct timespec spec;

   do {
      clock_gettime( CLOCK_MONOTONIC, &spec );
      delta = gc_ns_target - (spec.tv_nsec - g_ns_start);
   } while( 0 < delta );  
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
   int retval = 1,
      screen_bytes_per_pixel = 0,
      b_bytes_per_pixel = 0,
      screen_xy_byte = 0,
      src_y = 0;

   if( NULL == b->pixmap || NULL == b->pixmap->data ) {
      return 0;
   }

   /* Copy the image line by line into the buffer. */
   screen_bytes_per_pixel = g_buffer->bits_per_pixel / 8;
   b_bytes_per_pixel = b->pixmap->bits_per_pixel / 8;
   assert( screen_bytes_per_pixel == b_bytes_per_pixel );
   for( src_y = 0 ; b->pixmap->height > src_y ; src_y++ ) {
      /* Multiply offsets by px byte size. */
      screen_xy_byte = 
         ((y + src_y) * (SCREEN_W * screen_bytes_per_pixel)) +
         (x * screen_bytes_per_pixel);
   
      assert( screen_xy_byte < g_buffer_bits_sz );

      /* Perform the blit. */
      memcpy(
         &(g_buffer_bits[screen_xy_byte]),
         &(b->pixmap->data[(src_y * (b->pixmap->width * b_bytes_per_pixel))]),
         w * screen_bytes_per_pixel );
   }

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

int16_t graphics_platform_load_bitmap(
   RESOURCE_BITMAP_HANDLE res_handle, struct GRAPHICS_BITMAP* b
) {
   struct BITMAP_FILE_HEADER* file_header = NULL;
   struct BITMAP_DATA_HEADER* data_header = NULL;
   uint8_t* bitmap_bits = NULL;
   uint8_t* buffer = NULL;
   uint32_t* palette = NULL;
   uint32_t id = 0,
      retval = 1,
      buffer_sz = 0,
      px = 0;
   int
      px_offset = 0,
      x = 0,
      y = 0;

   buffer_sz = memory_sz( res_handle );
   if( 0 == buffer_sz ) {
      retval = 0;
      error_printf( "zero resource buffer sz" );
      goto cleanup;
   }
   buffer = resource_lock_handle( res_handle );

   file_header = (struct BITMAP_FILE_HEADER*)buffer;
   data_header =
      (struct BITMAP_DATA_HEADER*)&(buffer[sizeof( struct BITMAP_FILE_HEADER )]);
   palette = 
      (uint32_t*)&(buffer[
         sizeof( struct BITMAP_DATA_HEADER ) +
         sizeof( struct BITMAP_FILE_HEADER )]);

   assert( 'B' == file_header->id[0] );
   assert( 'M' == file_header->id[1] );

   debug_printf( 1, "bitmap is %d bytes long, data at %d bytes",
      file_header->file_sz, file_header->bmp_offset );

   bitmap_bits = &(buffer[file_header->bmp_offset]);

   /* Create buffer and XImage struct. */
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

   /* Load the image px by px into bit buffer. */
   for( y = 0 ; y < data_header->bitmap_h ; y++ ) {
      for( x = 0 ; x < data_header->bitmap_w ; x++ ) {
         px_offset =
            ((data_header->bitmap_h - y - 1) * data_header->bitmap_w) + x;
         memcpy( &px, &(palette[bitmap_bits[px_offset]]), 4 );
         XPutPixel( b->pixmap, x, y, px );
      }
   }

cleanup:

   if( NULL != buffer ) {
      buffer = resource_unlock_handle( res_handle );
   }

   if( NULL != res_handle ) {
      resource_free_handle( res_handle );
   }

   return retval;
}

int16_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   XDestroyImage( b->pixmap );
   free( b->bits );

   return 1;
}

