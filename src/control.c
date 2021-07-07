
#include "dstypes.h"

typedef int16_t (*CONTROL_CB)( struct WINDOW*, struct CONTROL* );

/* Drawing Callbacks */

static int16_t control_draw_label( struct WINDOW* w, struct CONTROL* c ) {
   char* str = NULL;

   assert( NULL != c );
   assert( NULL != c->data );

   str = memory_lock( c->data );

   graphics_string_at( 
      str, strlen( str ), w->x + c->x, w->y + c->y, c->fg, c->scale );

   str = memory_unlock( c->data );

   return 1;
}

const CONTROL_CB gc_control_draw_callbacks[] = {
   control_draw_label
};

/* Sizing Callbacks */

static int16_t control_width_label( struct WINDOW* w, struct CONTROL* c ) {
   int16_t width_out = 0;
   char* str = NULL;

   assert( NULL != c );
   assert( NULL != c->data );

   str = memory_lock( c->data );

   width_out += 
      (((memory_strnlen_ptr( str, memory_sz( c->data ) ) *
      (FONT_W + FONT_SPACE)) - 
         /* Take off one space for the end. */
         FONT_SPACE) *
      c->scale);

   str = memory_unlock( c->data );

   return width_out;
}

static int16_t control_height_label( struct WINDOW* w, struct CONTROL* c ) {
   /* TODO: Based on newlines in text. */
   return FONT_H;
}

const CONTROL_CB gc_control_width_callbacks[] = {
   control_width_label
};

const CONTROL_CB gc_control_height_callbacks[] = {
   control_height_label
};

/* General Functions */

int16_t control_push(
   uint32_t control_id, uint16_t type, uint16_t status,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, int8_t scale,
   MEMORY_HANDLE data, uint32_t window_id, struct DSEKAI_STATE* state
) {
   int i = 0;
   struct WINDOW* windows = NULL;
   struct CONTROL* controls = NULL;
   int16_t window_idx = -1,
      retval = 1;

   debug_printf( 1, "pushing new control %u to window %u",
      control_id, window_id );

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
      debug_printf( 1, "shifting control %u in window %u up by one...",
         controls[i - 1], windows[window_idx].id );
      memory_copy_ptr(
         &(controls[i]), &(controls[i - 1]), sizeof( struct CONTROL ) );
   }

   memory_zero_ptr( &(controls[0]), sizeof( struct CONTROL ) );

   /* Sizing callbacks below might need these. */
   controls[0].type = type;
   controls[0].data = data;
   controls[0].scale = scale;
   controls[0].status = status;
   controls[0].id = control_id;
   
   if( WINDOW_CENTERED == w ) {
      controls[0].w = gc_control_width_callbacks[type](
         &(windows[window_idx]),
         &(controls[0]) );
   } else {
      controls[0].w = w;
   }

   if( WINDOW_CENTERED == h ) {
      controls[0].h = gc_control_height_callbacks[type](
         &(windows[window_idx]),
         &(controls[0]) );
   } else {
      controls[0].h = h;
   }

   if( WINDOW_CENTERED == x ) {
      controls[0].x = (windows[window_idx].w / 2) - (controls[0].w / 2);
   } else {
      controls[0].x = x;
   }

   if( WINDOW_CENTERED == y ) {
      controls[0].y = (windows[window_idx].h / 2) - (controls[0].h / 2);
   } else {
      controls[0].y = y;
   }

   controls[0].fg = fg;
   controls[0].bg = bg;

   windows[window_idx].controls_count++;
   windows[window_idx].dirty = 1;

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
      debug_printf( 1, "searching for window %u (trying window %u)",
         window_id, windows[i].id );
      if( 0 == window_id || windows[i].id == window_id ) {
         debug_printf( 1, "window %u is at index %d", window_id, i );
         window_idx = i;
      }
   }

   if( 0 > window_idx ) {
      error_printf( "could not find window with ID %u", window_id );
      goto cleanup;
   }

   if( 0 == windows[window_idx].controls_count ) {
      error_printf( "no controls in window %u", window_id );
      return;
   }
   
   controls = memory_lock( windows[window_idx].controls_handle );

   /* Find the control IDX for the given ID. */
   for( i = 0 ; windows[window_idx].controls_count > i ; i++ ) {
      debug_printf( 1, "searching for control %u (trying window %u)",
         control_id, controls[i].id );
      if( 0 == control_id || controls[i].id == control_id ) {
         control_idx = i;
      }
   }

   if( 0 > control_idx ) {
      error_printf( "could not find control with ID %u", control_id );
      goto cleanup;
   }

   if( control_idx > 0 ) {
      for( i = control_idx ; i < windows[window_idx].controls_count ; i++ ) {
         debug_printf( 1, "shifting control %u in window %u down by one...",
            controls[i + 1], windows[window_idx].id );
         memory_copy_ptr(
            &(controls[i]), &(controls[i + 1]), sizeof( struct CONTROL ) );
      }
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

void control_draw_all( struct WINDOW* window ) {
   struct CONTROL* controls = NULL;
   int16_t i = 0;

   if( NULL == window->controls_handle ) {
      return;
   }

   controls = memory_lock( window->controls_handle );

   for( i = window->controls_count - 1 ; 0 <= i ; i-- ) {
      gc_control_draw_callbacks[controls[i].type]( window, &(controls[i]) );
   }

cleanup:

   if( NULL != controls ) {
      controls = memory_unlock( window->controls_handle );
   }
}

