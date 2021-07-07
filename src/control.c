
#include "dstypes.h"

int16_t control_push(
   uint16_t type, uint16_t status,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h,
   MEMORY_HANDLE data, uint16_t window_idx, struct DSEKAI_STATE* state
) {
   int i = 0;
   struct WINDOW* windows = NULL;
   struct CONTROL* controls = NULL;

   assert( NULL != state->windows_handle );

   windows = memory_lock( state->windows_handle );

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

   windows[window_idx].controls_count++;

   controls = memory_unlock( windows[window_idx].controls_handle );
   windows = memory_unlock( state->windows_handle );
}

/*
void control_pop( struct DSEKAI_STATE* state ) {
   int i = 0;
   struct WINDOW* windows = NULL;

   if( 0 == state->windows_count ) {
      return;
   }
   
   windows = memory_lock( state->windows_handle );

   for( i = 0 ; state->windows_count > i ; i++ ) {
      memory_copy_ptr(
         &(windows[i]), &(windows[i + 1]), sizeof( struct WINDOW ) );
   }

   windows = memory_unlock( state->windows_handle );

   state->windows_count--;
}
*/

