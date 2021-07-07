
#include "dstypes.h"

int16_t control_push(
   uint32_t control_id, uint16_t type, uint16_t status,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h,
   MEMORY_HANDLE data, uint32_t window_id, struct DSEKAI_STATE* state
) {
   int i = 0;
   struct WINDOW* windows = NULL;
   struct CONTROL* controls = NULL;
   int16_t window_idx = -1,
      retval = 1;

   assert( NULL != state->windows_handle );
   windows = memory_lock( state->windows_handle );

   for( i = 0 ; state->windows_count > i ; i++ ) {
      if( windows[i].id == window_id ) {
         window_idx = i;
         break;
      }
   }

   if( 0 > window_idx ) {
      retval = 0;
      error_printf( "could not find requested window" );
      goto cleanup;
   }

   /* Move the controls on the window up to make room at the bottom. */
   if( NULL == windows[window_idx].controls_handle ) {
      windows[window_idx].controls_handle = memory_alloc(
         sizeof( struct CONTROL ), CONTROL_COUNT_MAX );
   }

   assert( windows[window_idx].controls_count + 1 < CONTROL_COUNT_MAX );

   controls = memory_lock( windows[window_idx].controls_handle );

   for( i = windows[window_idx].controls_count ; 0 < i ; i-- ) {
      memory_copy_ptr(
         &(controls[i]), &(controls[i - 1]), sizeof( struct CONTROL ) );
   }

   memory_zero_ptr( &(controls[0]), sizeof( struct CONTROL ) );

   controls[0].x = x;
   controls[0].y = y;
   controls[0].w = w;
   controls[0].h = h;
   controls[0].type = type;
   controls[0].status = CONTROL_STATE_ENABLED;
   controls[0].id = control_id;

   windows[window_idx].controls_count++;

cleanup:

   if( NULL != controls ) {
      controls = memory_unlock( windows[window_idx].controls_handle );
   }

   if( NULL != windows ) {
      windows = memory_unlock( state->windows_handle );
   }

   return retval;
}

void control_pop(
   uint32_t window_id, uint32_t control_id, struct DSEKAI_STATE* state
) {
   int i = 0;
   struct WINDOW* windows = NULL;
   struct CONTROL* controls = NULL;
   int16_t window_idx = 0,
      control_idx = 0;

   if( 0 == state->windows_count ) {
      return;
   }
   
   windows = memory_lock( state->windows_handle );

   /* Find the window IDX for the given ID. */
   for( i = 0 ; state->windows_count > i ; i++ ) {
      if( 0 == control_id || windows[i].id == window_id ) {
         window_idx = i;
      }
   }

   if( 0 > window_idx ) {
      error_printf( "could not find window with ID %u", window_id );
      goto cleanup;
   }

   if( 0 == windows[window_idx].controls_count ) {
      return;
   }
   
   controls = memory_lock( windows[window_idx].controls_handle );

   /* Find the control IDX for the given ID. */
   for( i = 0 ; windows[window_idx].controls_count > i ; i++ ) {
      if( 0 == control_id || controls[i].id == control_id ) {
         control_idx = i;
      }
   }

   if( 0 > control_idx ) {
      error_printf( "could not find control with ID %u", control_id );
      goto cleanup;
   }

   for( i = control_idx ; i < windows[window_idx].controls_count ; i++ ) {
      memory_copy_ptr(
         &(controls[i]), &(controls[i - 1]), sizeof( struct CONTROL ) );
   }

   windows[window_idx].controls_count--;

cleanup:

   if( NULL != controls ) {
      controls = memory_unlock( windows[window_idx].controls_handle );
   }

   if( NULL != windows ) {
      windows = memory_unlock( state->windows_handle );
   }
}

