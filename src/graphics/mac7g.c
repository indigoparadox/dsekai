
#include "../graphics.h"

#include <stdlib.h>
#include <string.h>

#include <Quickdraw.h>
#include <Windows.h>
#include <MacMemory.h>
#include <Sound.h>
#include <Fonts.h>
#include <NumberFormatting.h>

#include "../data/drc.h"

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
   const struct GRAPHICS_BITMAP* b,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   BitMapHandle bmh = NULL;
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

int32_t graphics_load_bitmap( uint32_t id_in, struct GRAPHICS_BITMAP* b ) {
   uint8_t* buffer = NULL;
   int32_t buffer_sz = 0;
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
   buffer_sz = drc_get_resource(
      DRC_ARCHIVE, *(uint32_t*)DRC_BMP_TYPE, id, &buffer );
   if( 0 >= buffer_sz ) {
      goto cleanup;
   }

   b->pict = (PicHandle)NewHandle( buffer_sz - 512 );
   if( NULL == b->pict ) {
      goto cleanup;
   }
   memcpy( b->pict, buffer, buffer_sz - 512 );
   b->pict_sz = buffer_sz;

   b->ref_count++;
   b->initialized = 1;

   retval = 1;

cleanup:
   if( NULL != buffer ) {
      free( buffer );
   }

   return retval;
}

int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
}

