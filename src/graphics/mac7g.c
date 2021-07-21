
#define GRAPHICS_C
#include "../dstypes.h"

#include <Multiverse.h>

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

   if( NULL == b || NULL == b->pict ) {
      return 0;
   }

   SetRect( &r, x, y, x + w, y + h );

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
   /* uint8_t buffer[MAC7_RSRC_BUFFER_SZ] = NULL;
   int32_t buffer_sz = MAC7_RSRC_BUFFER_SZ; */
   int16_t retval = 1;

   /*
   buffer_sz = memory_sz( buffer_handle );
   buffer = memory_lock( buffer_handle );

   b->pict = (PicHandle)NewHandle( buffer_sz - 512 );
   if( NULL == b->pict ) {
      goto cleanup;
   }
   memcpy( b->pict, buffer, buffer_sz - 512 );
   b->pict_sz = buffer_sz; */

   b->pict = res_handle;

cleanup:

   return retval;
}

int16_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   if( NULL != b->pict ) {
      /* resource_free_handle( b->pict );
      b->pict = NULL; */
   }
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

