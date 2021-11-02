
#define CONTROL_C
#include "dsekai.h"

#ifdef PLATFORM_PALM

int16_t control_push(
   uint32_t control_id, uint16_t type, uint16_t status,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, int8_t scale,
   int32_t data_scalar, RESOURCE_ID data_res_id,
   uint32_t window_id, struct DSEKAI_STATE* state,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   return 0;
}

#else

/* Drawing Callbacks */

int16_t control_draw_BUTTON(
   struct WINDOW* w, struct CONTROL* c,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   /* TODO */
   return 1;
}

int16_t control_draw_CHECK(
   struct WINDOW* w, struct CONTROL* c,
   const char strings[][TILEMAP_STRINGS_SZ]
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
      w->x + c->x, w->y + c->y, c->fg, c->scale );
}

int16_t control_draw_LABEL_T(
   struct WINDOW* w, struct CONTROL* c,
   const char strings[][TILEMAP_STRINGS_SZ]
) {

   assert( NULL != c );
   assert( 0 <= c->data.scalar && c->data.scalar < TILEMAP_STRINGS_MAX );

   control_draw_text( w, c, 
      strings[c->data.scalar],
      memory_strnlen_ptr( strings[c->data.scalar], TILEMAP_STRINGS_SZ ) );

   return 1;
}

int16_t control_draw_LABEL_G(
   struct WINDOW* w, struct CONTROL* c,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   /* TODO */
   return 1;
}

int16_t control_draw_SPRITE(
   struct WINDOW* w, struct CONTROL* c,
   const char strings[][TILEMAP_STRINGS_SZ]
) {

   assert( NULL != c );

   /* TODO: Blit a defined mobile, rather than a sprite, so that we can adjust
    *       dir and other effects.
    */
   graphics_blit_at(
      c->data.res_id,
      0, 0, w->x + c->x + 2, w->y + c->y + 2, SPRITE_W, SPRITE_H );

   graphics_draw_rect( w->x + c->x, w->y + c->y, SPRITE_W + 4, SPRITE_H + 4,
      1, GRAPHICS_COLOR_BLACK );

   graphics_draw_rect(
      w->x + c->x + 1, w->y + c->y + 1, SPRITE_W + 3, SPRITE_H + 3,
      1, GRAPHICS_COLOR_WHITE );

   return 1;
}

/* Sizing Callbacks */

uint8_t control_sz_BUTTON(
   struct WINDOW* w, struct CONTROL* c, struct GRAPHICS_RECT* sz,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   /* TODO */
   return 0;
}

uint8_t control_sz_CHECK(
   struct WINDOW* w, struct CONTROL* c, struct GRAPHICS_RECT* sz,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   /* TODO */
   return 0;
}

uint8_t control_sz_LABEL_T(
   struct WINDOW* w, struct CONTROL* c, struct GRAPHICS_RECT* sz,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   int str_sz = 0;

   assert( NULL != c );

   if(
      0 > c->data.scalar ||
      c->data.scalar >= TILEMAP_STRINGS_MAX ||
      0 == memory_strnlen_ptr( strings[c->data.scalar], TILEMAP_STRINGS_SZ )
   ) {
      error_printf( "invalid string specified to control" );
      return 0;
   }

   str_sz = memory_strnlen_ptr( strings[c->data.scalar], TILEMAP_STRINGS_SZ );
   if( 0 >= str_sz ) {
      error_printf( "invalid string size returned: %d", str_sz );
      return 0;
   }

   graphics_string_sz( strings[c->data.scalar], str_sz, c->scale, sz );

   return 1;
}

uint8_t control_sz_LABEL_G(
   struct WINDOW* w, struct CONTROL* c, struct GRAPHICS_RECT* sz,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   /* TODO */
   return 0;
}

uint8_t control_sz_SPRITE(
   struct WINDOW* w, struct CONTROL* c, struct GRAPHICS_RECT* sz,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   /* TODO: Verify sprite exists. */
   sz->w = SPRITE_W + 4; /* For border. */
   sz->h = SPRITE_W + 4; /* For border. */
   return 1;
}

/* General Functions */

int16_t control_push(
   uint32_t control_id, uint16_t type, uint16_t status,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, int8_t scale,
   int32_t data_scalar, RESOURCE_ID data_res_id,
   uint32_t window_id, struct DSEKAI_STATE* state,
   const char strings[][TILEMAP_STRINGS_SZ]
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

   memory_zero_ptr( &control_sz, sizeof( struct GRAPHICS_RECT ) );

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
   } else if( 0 != (int)data_res_id ) {
      resource_assign_id( controls[0].data.res_id, data_res_id );
   }
   controls[0].scale = scale;
   controls[0].status = status;
   controls[0].id = control_id;

   if(
      !gc_control_sz_callbacks[type](
         &(windows[window_idx]), &(controls[0]), &control_sz, strings )
   ) {
      error_printf( "unable to create control" );
      goto cleanup;
   }

   if( WINDOW_CENTERED == w ) {
      controls[0].w = control_sz.w;
   } else {
      controls[0].w = w;
   }

   if( WINDOW_CENTERED == h ) {
      controls[0].h = control_sz.h;
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

void control_draw_all(
   struct WINDOW* w,
   const char strings[][TILEMAP_STRINGS_SZ]
) {
   struct CONTROL* controls = NULL;
   int16_t i = 0;

   if( (MEMORY_HANDLE)NULL == w->controls_handle ) {
      return;
   }

   controls = (struct CONTROL*)memory_lock( w->controls_handle );

   for( i = w->controls_count - 1 ; 0 <= i ; i-- ) {
      gc_control_draw_callbacks[controls[i].type](
         w, &(controls[i]), strings );
   }

   if( NULL != controls ) {
      controls = (struct CONTROL*)memory_unlock( w->controls_handle );
   }
}

#endif /* PLATFORM */

