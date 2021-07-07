
#include "dstypes.h"

#include "data/windows.h"

static MEMORY_HANDLE g_frames_handle = NULL;

void window_init() {
   struct WINDOW_FRAME* frames = NULL;

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

int window_draw_all( struct DSEKAI_STATE* state ) {
   struct WINDOW_FRAME* frames = NULL;
   struct WINDOW* windows = NULL;
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
   windows = memory_lock( state->windows_handle );
   for( i = 0 ; state->windows_count > i ; i++ ) {
#ifndef IGNORE_DIRTY
      if( 0 == windows[i].dirty ) {
#else
      if( 0 ) {
#endif /* !IGNORE_DIRTY */
         debug_printf( 0, "ignoring window %d (dirty: %d)",
            i, windows[i].dirty );
         continue;
      }

      assert( 0 == windows[i].w % PATTERN_W );
      assert( 0 == windows[i].h % PATTERN_H );

      debug_printf(
         1, "min: %d, %d; max: %d, %d",
         windows[i].x, windows[i].y, x_max, y_max );

      x_max = windows[i].x + windows[i].w;
      y_max = windows[i].y + windows[i].h;

      for( y = windows[i].y ; y < y_max ; y += PATTERN_H ) {
         for( x = windows[i].x ; x < x_max ; x += PATTERN_W ) {
            debug_printf( 1, "drawing window with frame %d...",
               windows[i].frame_idx );
            if( windows[i].x == x && windows[i].y == y ) {
               /* Top Left */
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].tl), x, y,
                  PATTERN_W, PATTERN_H );

            } else if( x_max - PATTERN_W == x && windows[i].y == y ) {
               /* Top Right */
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].tr), x, y,
                  PATTERN_W, PATTERN_H );

            } else if( windows[i].x == x && y_max - PATTERN_H == y ) {
               /* Bottom Left */
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].bl), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( x_max - PATTERN_W == x && y_max - PATTERN_H == y ) {
               /* Bottom Right */
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].br), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( x_max - PATTERN_W == x ) {
               /* Right */
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].r), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( windows[i].x == x ) {
               /* Left */
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].l), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( windows[i].y == y ) {
               /* Top */
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].t), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( y_max - PATTERN_H == y ) {
               /* Bottom */
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].b), x, y,
                  PATTERN_W, PATTERN_H );
            
            } else {
               blit_retval = graphics_blit_at(
                  &(frames[windows[i].frame_idx].c), x, y,
                  PATTERN_W, PATTERN_H );
            }

            if( !blit_retval ) {
               error_printf( "window blit failed" );
               goto cleanup;
            }
         }
      }

      control_draw_all( &(windows[i]) );
      windows[i].dirty -= 1;
   }

cleanup:

   if( NULL != windows ) {
      windows = memory_unlock( state->windows_handle );
   }

   if( NULL != frames ) {
      frames = memory_unlock( g_frames_handle );
   }

   return blit_retval;
}

int16_t window_push(
   uint32_t id, uint8_t status,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t frame_idx,
   struct DSEKAI_STATE* state
) {
   int i = 0;
   struct WINDOW* windows = NULL;
   int16_t retval = 0;

   if( NULL == state->windows_handle ) {
      state->windows_handle =
         memory_alloc( WINDOW_COUNT_MAX, sizeof( struct WINDOW ) );
   }

   windows = memory_lock( state->windows_handle );

   for( i = 0 ; state->windows_count > i ; i++ ) {
      if( windows[i].id == id ) {
         error_printf( "window with ID %u already exists", id );
         retval = 0;
         goto cleanup;
      }
   }

   assert( state->windows_count + 1 < WINDOW_COUNT_MAX );

   for( i = state->windows_count ; 0 < i ; i-- ) {
      debug_printf( 1, "shifting window %u up by one...",
         windows[i - 1].id );
      memory_copy_ptr(
         &(windows[i]), &(windows[i - 1]), sizeof( struct WINDOW ) );
   }

   memory_zero_ptr( &(windows[0]), sizeof( struct WINDOW ) );

   windows[0].status =
      WINDOW_STATUS_MODAL == status ?
         WINDOW_STATUS_MODAL : WINDOW_STATUS_VISIBLE;
   windows[0].w = w;
   windows[0].h = h;

   assert( 0 < windows[0].w );
   assert( 0 < windows[0].h );

   if( WINDOW_CENTERED == x ) {
      windows[0].x = (SCREEN_W / 2) - (windows[0].w / 2);
   } else {
      windows[0].x = x;
   }

   if( WINDOW_CENTERED == y ) {
      windows[0].y = (SCREEN_H / 2) - (windows[0].h / 2);
   } else {
      windows[0].y = y;
   }

   windows[0].frame_idx = frame_idx;
   windows[0].dirty = DIRTY_THRESHOLD;
   windows[0].id = id;

   state->windows_count++;

cleanup:

   if( NULL != windows ) {
      windows = memory_unlock( state->windows_handle );
   }

   return retval;
}

void window_pop( uint32_t id, struct DSEKAI_STATE* state ) {
   int i = 0;
   struct WINDOW* windows = NULL;
   int16_t idx = -1;

   if( 0 == state->windows_count ) {
      error_printf( "tried to pop window with no windows present" );
      return;
   }

   windows = memory_lock( state->windows_handle );

   for( i = 0 ; state->windows_count > i ; i++ ) {
      debug_printf( 1, "searching for window %u (trying window %u)",
         id, windows[i].id );
      if( 0 == id || windows[i].id == id ) {
         debug_printf( 1, "window %u is at index %d", id, i );
         idx = i;
      }
   }

   debug_printf( 1, "popping window %u...", id );

   if( 0 > idx ) {
      error_printf( "could not find window with ID %u", id );
      goto cleanup;
   }

   /* Clear window controls. */
   if( NULL != windows[idx].controls_handle ) {
      while( 0 < windows[idx].controls_count ) {
         windows = memory_unlock( state->windows_handle );
         control_pop( id, 0, state );
         windows = memory_lock( state->windows_handle );
      }
      memory_free( windows[idx].controls_handle );
   }

   for( i = idx ; state->windows_count > i ; i++ ) {
      debug_printf( 1, "shifting window %u down by one...",
         windows[i + 1].id );
      memory_copy_ptr(
         &(windows[i]), &(windows[i + 1]), sizeof( struct WINDOW ) );
   }

   state->windows_count--;

cleanup:

   windows = memory_unlock( state->windows_handle );
}

/**
 * \return 0 if no modal windows showing, 1+ otherwise.
 */
int16_t window_modal( struct DSEKAI_STATE* state ) {
   int i = 0;
   struct WINDOW* windows = NULL;
   int16_t modal = 0;

   if( 0 == state->windows_count ) {
      return 0;
   }

   windows = memory_lock( state->windows_handle );

   for( i = 0 ; state->windows_count > i ; i++ ) {
      if( WINDOW_STATUS_MODAL == windows[i].status ) {
         modal++;
      }
   }

   windows = memory_unlock( state->windows_handle );

   return 0;
}

