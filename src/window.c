
#include "window.h"

#include "graphics.h"
#include "memory.h"

#include "data/windows.h"

static MEMORY_HANDLE g_frames_handle = NULL;
static struct WINDOW g_windows[WINDOW_COUNT_MAX];
static uint8_t g_windows_count = 0;

void window_init() {
   struct WINDOW_FRAME* frames = NULL;

   memory_zero_ptr( g_windows, sizeof( struct WINDOW ) * WINDOW_COUNT_MAX );

   debug_printf( 1, "initalizing windowing system..." );
   g_frames_handle = memory_alloc( sizeof( struct WINDOW_FRAME ), 1 );
   frames = memory_lock( g_frames_handle );
   memory_copy_ptr(
      frames, &gc_frame_cm_checker, sizeof( struct WINDOW_FRAME ) );
   frames = memory_unlock( g_frames_handle );
}

void window_shutdown() {
   struct WINDOW_FRAME* frames = NULL;

   frames = memory_lock( g_frames_handle );
   graphics_unload_bitmap( &(frames[0].tr) );
   graphics_unload_bitmap( &(frames[0].tl) );
   graphics_unload_bitmap( &(frames[0].br) );
   graphics_unload_bitmap( &(frames[0].bl) );
   graphics_unload_bitmap( &(frames[0].t) );
   graphics_unload_bitmap( &(frames[0].b) );
   graphics_unload_bitmap( &(frames[0].r) );
   graphics_unload_bitmap( &(frames[0].l) );
   graphics_unload_bitmap( &(frames[0].c) );
   frames = memory_unlock( g_frames_handle );

   memory_free( g_frames_handle );
}

uint8_t windows_visible() {
   return g_windows_count;
}

int window_draw_all() {
   struct WINDOW_FRAME* frames = NULL;
   int i = 0,
      j = 0,
      x = 0,
      y = 0,
      x_max = 0,
      y_max = 0,
      x_min = 0,
      y_min = 0,
      blit_retval = 0;

   debug_printf( 0, "starting window drawing..." );

   frames =  memory_lock( g_frames_handle );
   for( i = 0 ; g_windows_count > i ; i++ ) {
      if(
         WINDOW_STATE_VISIBLE != g_windows[i].state
#ifndef IGNORE_DIRTY
         || 0 == g_windows[i].dirty
#endif /* !IGNORE_DIRTY */
      ) {
         debug_printf( 0, "ignoring window %d (dirty: %d, state: %d)",
            i, g_windows[i].dirty, g_windows[i].state );
         continue;
      }

      assert( 0 == g_windows[i].w % PATTERN_W );
      assert( 0 == g_windows[i].h % PATTERN_H );
      x_max = (SCREEN_W / 2) + (g_windows[i].w / 2); 
      y_max = (SCREEN_H / 2) + (g_windows[i].h / 2); 
      x_min = (SCREEN_W / 2) - (g_windows[i].w / 2);
      y_min = (SCREEN_H / 2) - (g_windows[i].h / 2);

      debug_printf( 1, "max: %d, %d; min: %d, %d", x_max, y_max, x_min, y_min );

      for( y = y_min ; y < y_max ; y += PATTERN_H ) {
         for( x = x_min ; x < x_max ; x += PATTERN_W ) {
            debug_printf( 1, "drawing window with frame %d...",
               g_windows[i].frame_idx );
            if( x_min == x && y_min == y ) {
               /* Top Left */
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].tl), x, y,
                  PATTERN_W, PATTERN_H );

            } else if( x_max - PATTERN_W == x && y_min == y ) {
               /* Top Right */
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].tr), x, y,
                  PATTERN_W, PATTERN_H );

            } else if( x_min == x && y_max - PATTERN_H == y ) {
               /* Bottom Left */
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].bl), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( x_max - PATTERN_W == x && y_max - PATTERN_H == y ) {
               /* Bottom Right */
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].br), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( x_max - PATTERN_W == x ) {
               /* Right */
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].r), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( x_min == x ) {
               /* Left */
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].l), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( y_min == y ) {
               /* Top */
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].t), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( y_max - PATTERN_H == y ) {
               /* Bottom */
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].b), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else {
               blit_retval = graphics_blit_at(
                  &(frames[g_windows[i].frame_idx].c), x, y,
                  PATTERN_W, PATTERN_H );
            }

            if( !blit_retval ) {
               error_printf( "window blit failed" );
               goto cleanup;
            }
         }
      }

      for( j = 0 ; g_windows[i].strings_count > j ; j++ ) {
         graphics_string_at( g_windows[i].strings[j],
            strlen( g_windows[i].strings[i] ),
            x_min + WINDOW_TEXT_X, y_min + (WINDOW_TEXT_Y * (i + 1)),
            g_windows[i].strings_color, 1 );
      }

      g_windows[i].dirty -= 1;
   }

cleanup:

   if( NULL != frames ) {
      frames = memory_unlock( g_frames_handle );
   }

   return blit_retval;
}

struct WINDOW* window_push() {
   int i = 0;

   assert( g_windows_count + 1 < WINDOW_COUNT_MAX );

   for( i = g_windows_count ; 0 < i ; i-- ) {
      memory_copy_ptr(
         &(g_windows[i]), &(g_windows[i - 1]), sizeof( struct WINDOW ) );
   }

   memory_zero_ptr( &(g_windows[0]), sizeof( struct WINDOW ) );

   g_windows_count++;

   return &(g_windows[0]);
}

void window_pop() {
   int i = 0;

   if( 0 == g_windows_count ) {
      return;
   }

   for( i = 0 ; g_windows_count > i ; i++ ) {
      memory_copy_ptr(
         &(g_windows[i]), &(g_windows[i + 1]), sizeof( struct WINDOW ) );
   }

   g_windows_count--;
}

