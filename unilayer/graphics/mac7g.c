
#define GRAPHICS_C
#include "../dstypes.h"

#include "../../tools/data/bmp.h"

#include <Multiverse.h>

#include <string.h>

QDGlobals g_qd;
WindowPtr g_window;
WindowRecord g_window_record;
Rect g_window_rect;

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int16_t graphics_platform_init( struct GRAPHICS_ARGS* args ) {
   ControlHandle beepbutton;
   Rect r;

   InitGraf( &g_qd.thePort );
   InitFonts();
   FlushEvents( everyEvent, 0 );
   InitWindows();
   InitMenus();
   InitCursor();
   TEInit();
   InitDialogs( nil );

   g_window_rect = qd.screenBits.bounds;
   /* InsetRect( &g_window_rect, 50, 50 ); */
   SetRect( &g_window_rect, 100, 100, 420, 300 );
   g_window = NewWindow(
      &g_window_record,
      &g_window_rect,
      "\pdsekai",
      false,
      noGrowDocProc,
      (WindowPtr)-1L,
      0,
      0L );
   /* g_window = NewWindow(
      &g_window_record,
      &g_window_rect,
      "\p",
      true,
      plainDBox,
      (WindowPtr)(-1),
      true,
      0 ); */
   if( !g_window ) {
      error_printf( "unable to create window" );
      return 0;
   }
   SetPort( g_window );
   ShowWindow( g_window );

   /* beepbutton = \
   NewControl( 
      g_window,&r,"\pBeep Hit",true,0,0,0,pushButProc,0);
   ShowControl( beepbutton );

   MoveTo( 100, 175 );
   DrawString( "\pPress any key to exit." ); */

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
   Rect r;

   SetRect( &r, x, y, x + 1, y + 1 );

   FillRect( &r, &g_qd.black );
}

/*
 * @return 1 if blit was successful and 0 otherwise.
 */
int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* b,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   Rect r;

   /*
   if( NULL == b || NULL == b->pict ) {
      return 0;
   }
   */

   SetRect( &r, x, y, x + w, y + h );
   /* DrawPicture( b->pict, &r ); */

   /*DrawPicture( &(b->pict[512]), &r );*/
   /*HLock(
   CopyBits( &(**b*/

   return 1;
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
   int16_t tmp_row_bytes = 0;
   uint8_t* bitmap_bits = NULL;
   uint8_t* buffer = NULL;
   uint32_t* palette = NULL;
   uint32_t id = 0,
      file_sz = 0,
      bmp_offset = 0,
      retval = 1,
      buffer_sz = 0,
      px_idx = 0,
      bitmap_w = 0,
      bitmap_h = 0,
      px = 0;
   int
      px_offset = 0,
      x = 0,
      y = 0;
   Rect r;
   GrafPort bitmap_port;
   GrafPtr prev_port;

   buffer_sz = memory_sz( res_handle );
   if( 0 == buffer_sz ) {
      retval = 0;
      error_printf( "zero resource buffer sz" );
      goto cleanup;
   }
   debug_printf( 0, "resource buffer %u bytes", buffer_sz );
   buffer = resource_lock_handle( res_handle );
   if( NULL == buffer ) {
      error_printf( "resource buffer is NULL" );
      retval = 0;
      goto cleanup;
   }

   /* Setup resource bitmap areas to get info from. */
   file_header = (struct BITMAP_FILE_HEADER*)buffer;
   data_header =
      (struct BITMAP_DATA_HEADER*)&(buffer[sizeof( struct BITMAP_FILE_HEADER )]);

   palette = 
      (uint32_t*)&(buffer[
         sizeof( struct BITMAP_DATA_HEADER ) +
         sizeof( struct BITMAP_FILE_HEADER )]);

   bitmap_bits = &(buffer[bmp_offset]);

   if( 'B' != file_header->id[0] || 'M' != file_header->id[1] ) {
      error_printf( "invalid bitmap; first bytes are: 0x%02x 0x%02x",
         file_header->id[0], file_header->id[1] );
      retval = 0;
      goto cleanup;
   }

   file_sz = dio_reverse_endian_32( file_header->file_sz );
   bmp_offset = dio_reverse_endian_32( file_header->bmp_offset );

   debug_printf( 1, "bitmap is %d bytes long, data at %d bytes",
      file_sz, bmp_offset );

   bitmap_w = dio_reverse_endian_32( data_header->bitmap_w );
   bitmap_h = dio_reverse_endian_32( data_header->bitmap_h );
   debug_printf( 1, "bitmap dimensions are %u by %u", bitmap_w, bitmap_h );

   /* Setup the target QuickDraw bitmap. */
   tmp_row_bytes = ((bitmap_w / 16) + 1) * 2;
   b->qd_bitmap.baseAddr = NewPtr( (long)tmp_row_bytes * (long)bitmap_h );
   if( NULL == b->qd_bitmap.baseAddr ) {
      error_printf( "unable to allocate QuickDraw bitmap" );
      retval = 0;
      goto cleanup;
   }
   b->qd_bitmap.rowBytes = tmp_row_bytes;
   SetRect( &(b->qd_bitmap.bounds), 0, 0, bitmap_w, bitmap_h );

   /* Draw the resource bitmap into the QuickDraw bitmap. */
   GetPort( &prev_port );
   OpenPort( &bitmap_port );
   SetPort( &bitmap_port );
   SetPortBits( &(b->qd_bitmap) );

   /* Load the image px by px into bit buffer. */
   for( y = 0 ; y < bitmap_h ; y++ ) {
      for( x = 0 ; x < bitmap_w ; x++ ) {
         px_offset = ((bitmap_h - y - 1) * bitmap_w) + x;
         px_idx = bitmap_bits[px_offset];
         px = dio_reverse_endian_32( palette[px_idx] );
         SetRect( &r, x, y, x + 1, y + 1 );
         debug_printf( 0, "%d, %d px color: %u (%u)", x, y, px, px_idx );
         if( 0 < px_idx ) {
            FillRect( &r, &g_qd.black );
         } else {
            FillRect( &r, &g_qd.white );
         }
      }
   }

   SetPort( &prev_port );

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
   /* if( NULL != b->pict ) { */
      /* resource_free_handle( b->pict );
      b->pict = NULL; */
   /* } */
}

#ifndef USE_SOFTWARE_TEXT

void graphics_string_sz(
   const char* str, uint16_t str_sz, uint8_t scale, struct GRAPHICS_RECT* sz_out
) {
}

void graphics_string_at(
   const char* str, uint16_t str_sz, uint16_t x_orig, uint16_t y_orig,
   GRAPHICS_COLOR color, uint8_t scale
) {
}

#endif /* !USE_SOFTWARE_TEXT */

