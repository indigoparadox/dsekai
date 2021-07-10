
#include "../dstypes.h"

#include <Multiverse.h>

QDGlobals g_qd;
WindowPtr g_window;
WindowRecord g_window_record;
Rect g_window_rect;

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int16_t graphics_platform_init() {
   ControlHandle beepbutton;
   Rect r;

   InitGraf( &g_qd.thePort );
   /* InitFonts(); */
   InitWindows();
   /* InitMenus(); */
   InitCursor();
   /* TEInit();
   InitDialogs( NULL ); */

   g_window_rect = qd.screenBits.bounds;
   /* InsetRect( &g_window_rect, 50, 50 ); */
   g_window = NewWindow(
      &g_window_record, &g_window_rect, "\p", true, 2, (WindowPtr)-1, true, 0 );
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
      return 0;
   }
   SetPort( g_window );

   SetRect( &r, 100, 100, 230, 120 );
   beepbutton = \
   NewControl( 
      g_window,&r,"\pBeep Hit",true,0,0,0,pushButProc,0);

   MoveTo( 100, 175 );
   DrawString( "\pPress any key to exit." );

   return 1;
}

void graphics_platform_shutdown() {
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
   const struct GRAPHICS_BITMAP* b,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   Rect r;

   if( NULL == b || NULL == b->pict ) {
      return 0;
   }

   DrawPicture( &(b->pict[512]), &r );
   /*HLock(
   CopyBits( &(**b*/

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
   /* uint8_t buffer[MAC7_RSRC_BUFFER_SZ] = NULL;
   int32_t buffer_sz = MAC7_RSRC_BUFFER_SZ; */
   Handle buffer_handle;
   union DRC_TYPE type = DRC_BITMAP_TYPE;
   uint32_t id = 0;
   int32_t retval = 0;

   assert( NULL != b );
   assert( 0 == b->ref_count );

   if( 0 < id_in ) {
      id = id_in;
   } else {
      id = b->id;
   }

   /* Load resource into buffer. */
   buffer_handle = resource_get_bitmap_handle( id );
   if( NULL == buffer_handle ) {
      error_printf( "unable to get resource %d info", id );
      retval = 0;
      goto cleanup;
   }

   /*
   buffer_sz = memory_sz( buffer_handle );
   buffer = memory_lock( buffer_handle );

   b->pict = (PicHandle)NewHandle( buffer_sz - 512 );
   if( NULL == b->pict ) {
      goto cleanup;
   }
   memcpy( b->pict, buffer, buffer_sz - 512 );
   b->pict_sz = buffer_sz; */

   b->pict = buffer_handle;

   b->ref_count++;
   b->initialized = 1;

   retval = 1;

cleanup:

   return retval;
}

int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
}

