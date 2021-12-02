
#define CONTROL_C
#include "dsekai.h"

char g_control_num_buf[CONTROL_NUM_BUFFER_SZ] = "";

#ifdef PLATFORM_PALM

int16_t control_push(
   uint32_t control_id, uint16_t type, uint8_t flags,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, int8_t scale,
   int32_t data_scalar, RESOURCE_ID data_res_id,
   uint32_t window_id, struct DSEKAI_STATE* state
) {
   return 0;
}

#else

/* Drawing Callbacks */

int16_t control_draw_BUTTON(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state
) {
   /* TODO */
   return 1;
}

int16_t control_draw_CHECK(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state
) {
   /* TODO */
   return 1;
}

static void control_draw_text(
   struct WINDOW* w, struct CONTROL* c, const char* str, uint8_t str_sz
) {

   assert( NULL != c );

   graphics_string_at( 
      str, str_sz,
      w->coords[GUI_X] + c->coords[GUI_X],
      w->coords[GUI_Y] + c->coords[GUI_Y], c->fg, c->scale );
}

static const char* control_get_text(
   struct CONTROL* c, const struct DSEKAI_STATE* state, int16_t* sz_out
) {
   const char* str_ptr;

   if( CONTROL_FLAG_TEXT_TILEMAP == (CONTROL_FLAG_TEXT_TILEMAP & c->flags) ) {
      /* Get the string from the tilemap strpool. */
      str_ptr = strpool_get( state->map.strpool, c->data.scalar, sz_out );

   } else if( CONTROL_FLAG_TEXT_MENU == (CONTROL_FLAG_TEXT_MENU & c->flags) ) {
      /* Get the string from the global menu table. */
      str_ptr = gc_menu_tokens[c->data.scalar];
      *sz_out = 
         memory_strnlen_ptr( gc_menu_tokens[c->data.scalar], MENU_TEXT_SZ );
   
   } else if( CONTROL_FLAG_TEXT_ITEM == (CONTROL_FLAG_TEXT_ITEM & c->flags) ) {
      /* Get the string from the player items list. */
      str_ptr = state->items[(0xff & c->data.scalar)].name;
      *sz_out = memory_strnlen_ptr( str_ptr, ITEM_NAME_SZ );

   } else if( CONTROL_FLAG_TEXT_NUM == (CONTROL_FLAG_TEXT_NUM & c->flags) ) {
      *sz_out = dio_itoa(
         g_control_num_buf, CONTROL_NUM_BUFFER_SZ, c->data.scalar, 10 );
      str_ptr = g_control_num_buf;
      debug_printf( 3, "text num: %s (%d)", str_ptr, *sz_out );
   
   } else {
      str_ptr = NULL;
   }

   return str_ptr;
}

int16_t control_draw_LABEL(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state
) {
   int16_t str_sz = 0;
   const char* str_ptr;

   str_ptr = control_get_text( c, state, &str_sz );

   if( NULL == str_ptr ) {
      error_printf( "invalid string specified to control" );
      return 0;
   }

   debug_printf( 3, "drawing menu string %d at %d, %d: %s",
      c->data.scalar,
      w->coords[GUI_X] + c->coords[GUI_X],
      w->coords[GUI_Y] + c->coords[GUI_Y], str_ptr );

   control_draw_text( w, c, str_ptr, str_sz );

   return 1;
}

int16_t control_draw_SPRITE(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state
) {

   assert( NULL != c );

   /* TODO: Blit a defined mobile, rather than a sprite, so that we can adjust
    *       dir and other effects.
    */
   graphics_blit_sprite_at(
      c->data.res_id,
      0, 0,
      w->coords[GUI_X] + c->coords[GUI_X] + 2,
      w->coords[GUI_Y] + c->coords[GUI_Y] + 2,
      SPRITE_W, SPRITE_H );

   graphics_draw_rect(
      w->coords[GUI_X] + c->coords[GUI_X],
      w->coords[GUI_Y] + c->coords[GUI_Y],
      SPRITE_W + 4, SPRITE_H + 4,
      1, GRAPHICS_COLOR_BLACK );

   graphics_draw_rect(
      w->coords[GUI_X] + c->coords[GUI_X] + 1,
      w->coords[GUI_Y] + c->coords[GUI_Y] + 1,
      SPRITE_W + 3, SPRITE_H + 3,
      1, GRAPHICS_COLOR_WHITE );

   return 1;
}

/* Sizing Callbacks */

uint8_t control_sz_BUTTON(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state,
   struct GRAPHICS_RECT* sz
) {
   /* TODO */
   return 0;
}

uint8_t control_sz_CHECK(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state,
   struct GRAPHICS_RECT* sz
) {
   /* TODO */
   return 0;
}

uint8_t control_sz_LABEL(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state,
   struct GRAPHICS_RECT* sz
) {
   int16_t str_sz = 0;
   const char* str_ptr;

   str_ptr = control_get_text( c, state, &str_sz );

   if( NULL == str_ptr ) {
      error_printf( "invalid string specified to control" );
      return 0;
   }

   graphics_string_sz( str_ptr, str_sz, c->scale, sz );

   return 1;
}

uint8_t control_sz_SPRITE(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state,
   struct GRAPHICS_RECT* sz
) {
   /* TODO: Verify sprite exists. */
   sz->w = SPRITE_W + 4; /* For border. */
   sz->h = SPRITE_W + 4; /* For border. */
   return 1;
}

/* General Functions */

void control_placement(
   struct CONTROL* c, struct WINDOW* w, int16_t coord, uint8_t x_y
) {

   assert( 2 > x_y );

   if( 0 >= w->grid[x_y] ) {
      /* Setup initial grid. */
      w->grid[x_y] = CONTROL_PADDING_OUTSIDE;
      w->grid[x_y + 2] = CONTROL_PADDING_OUTSIDE;
   }

   switch( coord ){
   case CONTROL_PLACEMENT_CENTER:
      /* Window width / 2 - Control width / 2 */
      c->coords[x_y] = (w->coords[x_y + 2] / 2) - (c->coords[x_y + 2] / 2);
      break;

   case CONTROL_PLACEMENT_RIGHT_BOTTOM:
      c->coords[x_y] =
         /* Window width - Padding - Control width */
         w->coords[x_y + 2] - CONTROL_PADDING_OUTSIDE - c->coords[x_y + 2];
      break;

   case CONTROL_PLACEMENT_GRID_RIGHT_DOWN:
      w->grid[x_y + 2] = w->grid[x_y];
      w->grid[x_y] += c->coords[x_y + 2] + CONTROL_PADDING_INSIDE;
      /* No break. */

   case CONTROL_PLACEMENT_GRID:
      debug_printf( 0, "adding control using grid at: %d", w->grid[x_y + 2] );
      c->coords[x_y] = w->grid[x_y + 2];
      break;

   default:
      c->coords[x_y] = coord;
      break;
   }
}

int16_t control_push(
   uint32_t control_id, uint16_t type, uint8_t flags,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, int8_t scale,
   int32_t data_scalar, RESOURCE_ID data_res_id,
   uint32_t window_id, struct DSEKAI_STATE* state
) {
   int i = 0;
   struct WINDOW* windows = NULL;
   struct CONTROL* controls = NULL;
   int16_t window_idx = -1,
      retval = 1;
   struct GRAPHICS_RECT control_sz;

   debug_printf( 1, "pushing new control %u to window %u",
      control_id, window_id );

   assert( (MEMORY_HANDLE)NULL != state->windows_handle );
   windows = (struct WINDOW*)memory_lock( state->windows_handle );

   memory_zero_ptr( (MEMORY_PTR)&control_sz, sizeof( struct GRAPHICS_RECT ) );

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
   if( (MEMORY_HANDLE)NULL == windows[window_idx].controls_handle ) {
      windows[window_idx].controls_handle = memory_alloc(
         sizeof( struct CONTROL ), CONTROL_COUNT_MAX );
   }

   assert( windows[window_idx].controls_count + 1 < CONTROL_COUNT_MAX );

   controls = (struct CONTROL*)memory_lock(
      windows[window_idx].controls_handle );

   for( i = windows[window_idx].controls_count ; 0 < i ; i-- ) {
      debug_printf( 1, "shifting control %u in window %u up by one...",
         controls[i - 1].id, windows[window_idx].id );
      memory_copy_ptr(
         (MEMORY_PTR)&(controls[i]), (MEMORY_PTR)&(controls[i - 1]),
         sizeof( struct CONTROL ) );
   }

   memory_zero_ptr( (MEMORY_PTR)&(controls[0]), sizeof( struct CONTROL ) );

   /* Sizing callbacks below might need these. */
   controls[0].type = type;
   if( 0 != data_scalar ) {
      controls[0].data.scalar = data_scalar;
   } else if( !resource_compare_id( 0, data_res_id ) ) {
      resource_assign_id( controls[0].data.res_id, data_res_id );
   }
   controls[0].scale = scale;
   controls[0].flags = flags;
   controls[0].id = control_id;

   if(
      !gc_control_sz_callbacks[type](
         &(windows[window_idx]), &(controls[0]), state, &control_sz )
   ) {
      error_printf( "unable to create control" );
      goto cleanup;
   }

   controls[0].coords[GUI_W] = control_sz.w;
   controls[0].coords[GUI_H] = control_sz.h;

   /* X/Y Placement */
   control_placement(
      &(controls[0]), &(windows[window_idx]), x, GUI_X );
   control_placement(
      &(controls[0]), &(windows[window_idx]), y, GUI_Y );

   controls[0].fg = fg;
   controls[0].bg = bg;

   windows[window_idx].controls_count++;
   windows[window_idx].dirty = 1;

cleanup:

   if( NULL != controls ) {
      controls = (struct CONTROL*)memory_unlock(
         windows[window_idx].controls_handle );
   }

   if( NULL != windows ) {
      windows = (struct WINDOW*)memory_unlock( state->windows_handle );
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
   
   windows = (struct WINDOW*)memory_lock( state->windows_handle );

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
   
   controls = (struct CONTROL*)memory_lock(
      windows[window_idx].controls_handle );

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
            controls[i + 1].id, windows[window_idx].id );
         memory_copy_ptr(
            (MEMORY_PTR)&(controls[i]), (MEMORY_PTR)&(controls[i + 1]),
            sizeof( struct CONTROL ) );
      }
   }

   windows[window_idx].controls_count--;

cleanup:

   if( NULL != controls ) {
      controls = (struct CONTROL*)memory_unlock(
         windows[window_idx].controls_handle );
   }

   if( NULL != windows ) {
      windows = (struct WINDOW*)memory_unlock( state->windows_handle );
   }
}

void control_draw_all( struct WINDOW* w, struct DSEKAI_STATE* state ) {
   struct CONTROL* controls = NULL;
   int16_t i = 0;

   if( (MEMORY_HANDLE)NULL == w->controls_handle ) {
      return;
   }

   controls = (struct CONTROL*)memory_lock( w->controls_handle );

   for( i = w->controls_count - 1 ; 0 <= i ; i-- ) {
      gc_control_draw_callbacks[controls[i].type](
         w, &(controls[i]), state );
   }

   if( NULL != controls ) {
      controls = (struct CONTROL*)memory_unlock( w->controls_handle );
   }
}

#endif /* PLATFORM */

