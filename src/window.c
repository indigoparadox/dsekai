
#include "dsekai.h"

#ifdef PLATFORM_PALM

int16_t window_modal( struct DSEKAI_STATE* state ) {
   return 0;
}

int window_draw_all(
   struct DSEKAI_STATE* state
) {
   return 0;
}

int16_t window_push(
   uint32_t id, uint8_t status,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t frame_idx,
   struct DSEKAI_STATE* state
) {
   return 0;
}

void window_pop( uint32_t id, struct DSEKAI_STATE* state ) {
}

void window_init() {
}

void window_shutdown() {
}

#else

static MEMORY_HANDLE g_frames_handle = NULL;

void window_init() {
   struct WINDOW_FRAME* frames = NULL;

   debug_printf( 1, "initalizing windowing system..." );
   g_frames_handle = memory_alloc( sizeof( struct WINDOW_FRAME ), 1 );
   frames = (struct WINDOW_FRAME*)memory_lock( g_frames_handle );
   /* memory_copy_ptr(
      (MEMORY_PTR)frames, (MEMORY_PTR)&gc_frame_cm_checker,
      sizeof( struct WINDOW_FRAME ) ); */

#ifdef RESOURCE_FILE
   resource_assign_id( frames[0].tr, ASSETS_PATH DEPTH_SPEC "/p_chk_tr.bmp" );
   resource_assign_id( frames[0].tl, ASSETS_PATH DEPTH_SPEC "/p_chk_tl.bmp" );
   resource_assign_id( frames[0].br, ASSETS_PATH DEPTH_SPEC "/p_chk_br.bmp" );
   resource_assign_id( frames[0].bl, ASSETS_PATH DEPTH_SPEC "/p_chk_bl.bmp" );
   resource_assign_id( frames[0].t , ASSETS_PATH DEPTH_SPEC "/p_chk_t.bmp" );
   resource_assign_id( frames[0].b , ASSETS_PATH DEPTH_SPEC "/p_chk_b.bmp" );
   resource_assign_id( frames[0].r , ASSETS_PATH DEPTH_SPEC "/p_chk_r.bmp" );
   resource_assign_id( frames[0].l , ASSETS_PATH DEPTH_SPEC "/p_chk_l.bmp" );
   resource_assign_id( frames[0].c , ASSETS_PATH DEPTH_SPEC "/p_chk_c.bmp" );
#else
   resource_assign_id( frames[0].tr , p_chk_tr );
   resource_assign_id( frames[0].tl , p_chk_tl );
   resource_assign_id( frames[0].br , p_chk_br );
   resource_assign_id( frames[0].bl , p_chk_bl );
   resource_assign_id( frames[0].t , p_chk_t );
   resource_assign_id( frames[0].b , p_chk_b );
   resource_assign_id( frames[0].r , p_chk_r );
   resource_assign_id( frames[0].l , p_chk_l );
   resource_assign_id( frames[0].c , p_chk_c );
#endif

   frames = (struct WINDOW_FRAME*)memory_unlock( g_frames_handle );
}

void window_shutdown() {
   memory_free( g_frames_handle );
}

int window_draw_all( struct DSEKAI_STATE* state ) {
   struct WINDOW_FRAME* frames = NULL;
   struct WINDOW* windows = NULL;
   int i = 0,
      x = 0,
      y = 0,
      x_max = 0,
      y_max = 0,
      blit_retval = 0;

   debug_printf( 0, "starting window drawing..." );

   frames =  (struct WINDOW_FRAME*)memory_lock( g_frames_handle );
   windows = (struct WINDOW*)memory_lock( state->windows_handle );
   for( i = state->windows_count - 1 ; 0 <= i ; i-- ) {
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

      debug_printf( 1, "drawing window with frame %d...",
         windows[i].frame_idx );

      for( y = windows[i].y ; y < y_max ; y += PATTERN_H ) {
         for( x = windows[i].x ; x < x_max ; x += PATTERN_W ) {
            if( windows[i].x == x && windows[i].y == y ) {
               /* Top Left */
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].tl, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );

            } else if( x_max - PATTERN_W == x && windows[i].y == y ) {
               /* Top Right */
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].tr, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );

            } else if( windows[i].x == x && y_max - PATTERN_H == y ) {
               /* Bottom Left */
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].bl, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( x_max - PATTERN_W == x && y_max - PATTERN_H == y ) {
               /* Bottom Right */
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].br, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( x_max - PATTERN_W == x ) {
               /* Right */
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].r, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( windows[i].x == x ) {
               /* Left */
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].l, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( windows[i].y == y ) {
               /* Top */
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].t, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );
            
            } else if( y_max - PATTERN_H == y ) {
               /* Bottom */
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].b, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );
            
            } else {
               blit_retval = graphics_blit_tile_at(
                  frames[windows[i].frame_idx].c, 0, 0, x, y,
                  PATTERN_W, PATTERN_H );
            }

            if( !blit_retval ) {
               /* error_printf( "window blit failed" ); */
               goto cleanup;
            }
         }
      }

      control_draw_all( &(windows[i]), state );
      windows[i].dirty -= 1;
   }

cleanup:

   if( NULL != windows ) {
      windows = (struct WINDOW*)memory_unlock( state->windows_handle );
   }

   if( NULL != frames ) {
      frames = (struct WINDOW_FRAME*)memory_unlock( g_frames_handle );
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

   if( (MEMORY_HANDLE)NULL == state->windows_handle ) {
      state->windows_handle =
         memory_alloc( WINDOW_COUNT_MAX, sizeof( struct WINDOW ) );
   }

   windows = (struct WINDOW*)memory_lock( state->windows_handle );

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
         (MEMORY_PTR)&(windows[i]), (MEMORY_PTR)&(windows[i - 1]),
         sizeof( struct WINDOW ) );
   }

   memory_zero_ptr( (MEMORY_PTR)&(windows[0]), sizeof( struct WINDOW ) );

   windows[0].status =
      WINDOW_STATUS_MODAL == status ?
         WINDOW_STATUS_MODAL : WINDOW_STATUS_VISIBLE;
   windows[0].w = w;
   windows[0].h = h;

   assert( 0 < windows[0].w );
   assert( 0 < windows[0].h );

   if( WINDOW_CENTERED == x ) {
      windows[0].x = (SCREEN_MAP_W / 2) - (windows[0].w / 2);
   } else {
      windows[0].x = x;
   }

   if( WINDOW_CENTERED == y ) {
      windows[0].y = (SCREEN_MAP_H / 2) - (windows[0].h / 2);
   } else {
      windows[0].y = y;
   }

   windows[0].frame_idx = frame_idx;
   windows[0].dirty = DIRTY_THRESHOLD;
   windows[0].id = id;

   state->windows_count++;

cleanup:

   if( NULL != windows ) {
      windows = (struct WINDOW*)memory_unlock( state->windows_handle );
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

   windows = (struct WINDOW*)memory_lock( state->windows_handle );

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
   if( (MEMORY_HANDLE)NULL != windows[idx].controls_handle ) {
      while( 0 < windows[idx].controls_count ) {
         windows = (struct WINDOW*)memory_unlock( state->windows_handle );
         control_pop( id, 0, state );
         windows = (struct WINDOW*)memory_lock( state->windows_handle );
      }
      memory_free( windows[idx].controls_handle );
   }

   for( i = idx ; state->windows_count > i ; i++ ) {
      debug_printf( 1, "shifting window %u down by one...",
         windows[i + 1].id );
      memory_copy_ptr(
         (MEMORY_PTR)&(windows[i]), (MEMORY_PTR)&(windows[i + 1]),
         sizeof( struct WINDOW ) );
   }

   state->windows_count--;

cleanup:

   windows = (struct WINDOW*)memory_unlock( state->windows_handle );
}

int16_t window_modal( struct DSEKAI_STATE* state ) {
   int i = 0;
   struct WINDOW* windows = NULL;
   int16_t modal = 0;

   if( 0 == state->windows_count ) {
      return 0;
   }

   windows = (struct WINDOW*)memory_lock( state->windows_handle );

   for( i = 0 ; state->windows_count > i ; i++ ) {
      if( WINDOW_STATUS_MODAL == windows[i].status ) {
         modal++;
      }
   }

   windows = (struct WINDOW*)memory_unlock( state->windows_handle );

   return modal;
}

#endif /* PLATFORM */

