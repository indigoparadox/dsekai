
#define WINDOW_C
#include "dsekai.h"

/* === Static Utility Functions === */

static void window_print_all( struct WINDOW* windows ) {
   /*
   int16_t i = 0;

   for( i = 1 ; DSEKAI_WINDOWS_MAX > i ; i++ ) {
      debug_printf( 1, "windows[%d] id: %d parent: %d flags: %u",
         i, windows[i].id, windows[i].parent_id, windows[i].flags );
   }
   */
}

static struct WINDOW* window_get( uint16_t id, struct WINDOW* windows ) {
   int16_t i = 0;

   for( i = 1 ; DSEKAI_WINDOWS_MAX > i ; i++ ) {
      if(
         /* Skip live windows looking for free window. */
         (0 == id &&
            WINDOW_FLAG_ACTIVE == (WINDOW_FLAG_ACTIVE & windows[i].flags)) ||
         /* Skip dead windows looking for requested ID. */
         (0 != id &&
            WINDOW_FLAG_ACTIVE != (WINDOW_FLAG_ACTIVE & windows[i].flags))
      ) {
         continue;
      }

      if( 0 == id || windows[i].id == id ) {
         return &(windows[i]);
      }
   }

   return NULL;
}

static void window_placement(
   struct WINDOW* c, struct WINDOW* p, int16_t coord, uint8_t x_y
) {
   int16_t* p_grid = NULL;
   const int16_t* p_coords;

   assert( 2 > x_y );

   if( NULL == p ) {
      /* Position relative to screen. */
      debug_printf( 0, "window %d rel screen", c->id );
      p_coords = gc_window_screen_coords;
      p_grid = g_window_screen_grid;
   } else {
      /* Position relative to parent. */
      debug_printf( 0, "window %d rel window %d", c->id, c->parent_id );
      p_coords = gc_window_screen_coords;
      p_coords = p->coords;
      p_grid = p->data.grid;
   }

   if( 0 >= p_grid[x_y] ) {
      /* Setup initial grid. */
      p_grid[x_y] = WINDOW_PADDING_OUTSIDE;
      p_grid[x_y + 2] = WINDOW_PADDING_OUTSIDE;
   }

   switch( coord ){
   case WINDOW_PLACEMENT_CENTER:
      /* Window width / 2 - Control width / 2 */
      assert( p_coords[x_y + 2] > 0 );
      c->coords[x_y] = (p_coords[x_y + 2] / 2) - (c->coords[x_y + 2] / 2);
      debug_printf( 0, "window %d center coord %d (%d / 2) - (%d / 2): %d",
         c->id, x_y, p_coords[x_y + 2], c->coords[x_y + 2], c->coords[x_y] );
      break;

   case WINDOW_PLACEMENT_RIGHT_BOTTOM:
      c->coords[x_y] =
         /* Window width - Padding - Control width */
         p_coords[x_y + 2] - WINDOW_PADDING_OUTSIDE - c->coords[x_y + 2];
      break;

   case WINDOW_PLACEMENT_GRID_RIGHT_DOWN:
      p_grid[x_y + 2] = p_grid[x_y];
      p_grid[x_y] += c->coords[x_y + 2] + WINDOW_PADDING_INSIDE;
      /* No break. */

   case WINDOW_PLACEMENT_GRID:
      debug_printf( 0, "adding control using grid at: %d", p_grid[x_y + 2] );
      c->coords[x_y] = p_grid[x_y + 2];
      break;

   default:
      c->coords[x_y] = coord;
      break;
   }
}

static int16_t window_sizing(
   int16_t w_id, int16_t dimension, uint8_t w_h,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX], struct DSEKAI_STATE* state
) {
   int16_t win_sz[2];
   int16_t retval = 0;
   struct WINDOW* c = NULL;

   assert( 4 > w_h );
   assert( 1 < w_h );

   debug_printf( 0, "sizing window ID: %d", w_id );

   c = window_get( w_id, windows );
   assert( NULL != c );

   /* Width and Height */
   if( WINDOW_SIZE_AUTO != dimension && 0 < dimension ) {
      c->coords[w_h] = dimension;
      debug_printf( 0, "window %d manual size %d: %d",
         c->id, w_h, dimension );
      retval = dimension;

   } else if(
      gc_window_sz_callbacks[c->type]( w_id, windows, state, win_sz )
   ) {
      c->coords[w_h] = win_sz[w_h - 2];
      debug_printf( 0, "window %d auto-size %d: %d",
         c->id, w_h, win_sz[w_h - 2] );
      retval = win_sz[w_h - 2];

   } else {
      error_printf( "unable to automatically size window" );
      retval = 0;
   }

   /* TODO: Make sure window doesn't exceed parent size. */

   return retval;
}

static void window_draw_text(
   struct WINDOW* c, struct WINDOW* p, const char* str, uint16_t str_sz
) {
   int16_t offset_x = 0,
      offset_y = 0;

   if( NULL != p ) {
      offset_x = p->coords[GUI_X];
      offset_y = p->coords[GUI_Y];
   }

   assert( NULL != c );

   debug_printf( 0, "drawing string at %d, %d: %s",
      offset_x + c->coords[GUI_X],
      offset_y + c->coords[GUI_Y], str );

   graphics_string_at( 
      str, str_sz,
      offset_x + c->coords[GUI_X],
      offset_y + c->coords[GUI_Y], c->fg, c->render_flags );
}

/* === Drawing Callbacks === */

static int16_t window_draw_WINDOW(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state
) {
   struct WINDOW_FRAME* frames = NULL;
   int16_t x = 0,
      y = 0,
      i = 0,
      x_max = 0,
      y_max = 0,
      blit_retval = 0;
   struct WINDOW* c = NULL;

   debug_printf( 0, "drawing window ID: %d", w_id );

   c = window_get( w_id, windows );
   assert( NULL != c );

   frames =  (struct WINDOW_FRAME*)memory_lock( g_frames_handle );

   debug_printf(
      0, "min: %d, %d; max: %d, %d",
      c->coords[GUI_X], c->coords[GUI_Y],
      x_max, y_max );

   x_max = c->coords[GUI_X] + c->coords[GUI_W];
   y_max = c->coords[GUI_Y] + c->coords[GUI_H];

   debug_printf( 0, "drawing window with frame %d...",
      c->render_flags );

   /* Draw the window background. */
   for( y = c->coords[GUI_Y] ; y < y_max ; y += PATTERN_H ) {
      for( x = c->coords[GUI_X] ; x < x_max ; x += PATTERN_W ) {
         if(
            c->coords[GUI_X] == x &&
            c->coords[GUI_Y] == y
         ) {
            /* Top Left */
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].tl, 0, 0, x, y,
               PATTERN_W, PATTERN_H );

         } else if(
            x_max - PATTERN_W == x && c->coords[GUI_Y] == y
         ) {
            /* Top Right */
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].tr, 0, 0, x, y,
               PATTERN_W, PATTERN_H );

         } else if(
            c->coords[GUI_X] == x && y_max - PATTERN_H == y
         ) {
            /* Bottom Left */
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].bl, 0, 0, x, y,
               PATTERN_W, PATTERN_H );
         
         } else if( x_max - PATTERN_W == x && y_max - PATTERN_H == y ) {
            /* Bottom Right */
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].br, 0, 0, x, y,
               PATTERN_W, PATTERN_H );
         
         } else if( x_max - PATTERN_W == x ) {
            /* Right */
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].r, 0, 0, x, y,
               PATTERN_W, PATTERN_H );
         
         } else if( c->coords[GUI_X] == x ) {
            /* Left */
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].l, 0, 0, x, y,
               PATTERN_W, PATTERN_H );
         
         } else if( c->coords[GUI_Y] == y ) {
            /* Top */
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].t, 0, 0, x, y,
               PATTERN_W, PATTERN_H );
         
         } else if( y_max - PATTERN_H == y ) {
            /* Bottom */
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].b, 0, 0, x, y,
               PATTERN_W, PATTERN_H );
         
         } else {
            blit_retval = graphics_blit_tile_at(
               frames[c->render_flags].c, 0, 0, x, y,
               PATTERN_W, PATTERN_H );
         }

         if( !blit_retval ) {
            /* error_printf( "window blit failed" ); */
            goto cleanup;
         }
      }
   }

   /* Draw the window's children. */
   for( i = 1 ; DSEKAI_WINDOWS_MAX > i ; i++ ) {
      /* Skip inactive and non-children. */
      if(
         WINDOW_FLAG_ACTIVE != (WINDOW_FLAG_ACTIVE & windows[i].flags) ||
         windows[i].parent_id != w_id
      ) {
         continue;
      }

      blit_retval = gc_window_draw_callbacks[windows[i].type](
         windows[i].id, windows, state );
      if( !blit_retval ) {
         error_printf( "error drawing window children!" );
         goto cleanup;
      }

      windows[i].flags &= ~WINDOW_FLAG_DIRTY;
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

static int16_t window_draw_BUTTON(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state
) {
   /* TODO */
   return 1;
}

static int16_t window_draw_CHECK(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state
) {
   /* TODO */
   return 1;
}

static const char* window_get_text(
   struct WINDOW* c, const struct DSEKAI_STATE* state, int16_t* sz_out
) {
   const char* str_ptr;

   if( WINDOW_FLAG_TEXT_PTR == (WINDOW_FLAG_TEXT_PTR & c->flags) ) {
      /* Get the string from a directly passed pointer. */
      str_ptr = c->data.string;
      /* TODO: Sane maximum length default. */
      *sz_out = memory_strnlen_ptr( str_ptr, 100 );

   } else if(
      WINDOW_FLAG_TEXT_TILEMAP == (WINDOW_FLAG_TEXT_TILEMAP & c->flags)
   ) {
      /* Get the string from the tilemap strpool. */
      str_ptr = strpool_get( state->map.strpool, c->data.scalar, sz_out );

   } else if( WINDOW_FLAG_TEXT_MENU == (WINDOW_FLAG_TEXT_MENU & c->flags) ) {
      /* Get the string from the global menu table. */
      str_ptr = gc_menu_tokens[c->data.scalar];
      *sz_out = 
         memory_strnlen_ptr( gc_menu_tokens[c->data.scalar], MENU_TEXT_SZ );
   
   } else if( WINDOW_FLAG_TEXT_ITEM == (WINDOW_FLAG_TEXT_ITEM & c->flags) ) {
      /* Get the string from the player items list. */
      str_ptr = state->items[(0xff & c->data.scalar)].name;
      *sz_out = memory_strnlen_ptr( str_ptr, ITEM_NAME_SZ );

   } else if( WINDOW_FLAG_TEXT_NUM == (WINDOW_FLAG_TEXT_NUM & c->flags) ) {
      *sz_out = dio_itoa(
         g_window_num_buf, WINDOW_NUM_BUFFER_SZ, c->data.scalar, 10 );
      str_ptr = g_window_num_buf;
      debug_printf( 0, "text num: %s (%d)", str_ptr, *sz_out );
   
   } else {
      str_ptr = NULL;
   }

   return str_ptr;
}

static int16_t window_draw_LABEL(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state
) {
   int16_t str_sz = 0;
   const char* str_ptr;
   struct WINDOW* c = NULL,
      * p = NULL;

   c = window_get( w_id, windows );
   assert( NULL != c );

   p = window_get( c->parent_id, windows );

   str_ptr = window_get_text( window_get( w_id, windows ), state, &str_sz );

   if( NULL == str_ptr ) {
      error_printf( "invalid string specified to control" );
      return 0;
   }

   window_draw_text( c, p, str_ptr, str_sz );

   return 1;
}

static int16_t window_draw_SPRITE(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state
) {
   int16_t offset_sprite = 0,
      offset_x = 0,
      offset_y = 0;
   struct WINDOW* c = NULL,
      * p = NULL;
   uint8_t dir = 0;

   c = window_get( w_id, windows );
   assert( NULL != c );

   p = window_get( c->parent_id, windows );
   if( NULL != p ) {
      offset_x = p->coords[GUI_X];
      offset_y = p->coords[GUI_Y];
   }

   debug_printf(
      0, "sprite %u screen offset: %d, %d", w_id, offset_x, offset_y );

   /* Set offset_sprite based on dir flags. */
   dir = ((c->flags & 0x30) >> 4);
   offset_sprite = dir * TILE_H;

   graphics_blit_sprite_at(
      c->data.res_id,
      0, offset_sprite,
      offset_x + c->coords[GUI_X] + 2,
      offset_y + c->coords[GUI_Y] + 2,
      SPRITE_W, SPRITE_H );

   graphics_draw_rect(
      offset_x + c->coords[GUI_X],
      offset_y + c->coords[GUI_Y],
      SPRITE_W + 4, SPRITE_H + 4,
      1, GRAPHICS_COLOR_BLACK );

   graphics_draw_rect(
      offset_x + c->coords[GUI_X] + 1,
      offset_y + c->coords[GUI_Y] + 1,
      SPRITE_W + 3, SPRITE_H + 3,
      1, GRAPHICS_COLOR_WHITE );

   return 1;
}

/* === Sizing Callbacks === */

static uint8_t window_sz_WINDOW(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state,
   int16_t r[2]
) {
   /* TODO */
   return 0;
}

static uint8_t window_sz_BUTTON(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state,
   int16_t r[2]
) {
   /* TODO */
   return 0;
}

static uint8_t window_sz_CHECK(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state,
   int16_t r[2]
) {
   /* TODO */
   return 0;
}

static uint8_t window_sz_LABEL(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state,
   int16_t r[2]
) {
   int16_t str_sz = 0;
   const char* str_ptr;
   struct GRAPHICS_RECT sz;
   struct WINDOW* c = NULL;

   c = window_get( w_id, windows );
   assert( NULL != c );

   str_ptr = window_get_text( c, state, &str_sz );

   if( NULL == str_ptr ) {
      error_printf( "invalid string specified to control" );
      return 0;
   }

   graphics_string_sz( str_ptr, str_sz, 0, &sz );

   r[0] = sz.w;
   r[1] = sz.h;

   return 1;
}

static uint8_t window_sz_SPRITE(
   uint16_t w_id,
   struct WINDOW windows[DSEKAI_WINDOWS_MAX],
   struct DSEKAI_STATE* state,
   int16_t r[2]
) {
   /* TODO: Verify sprite exists. */
   r[0] = SPRITE_W + 4; /* For border. */
   r[1] = SPRITE_W + 4; /* For border. */
   return 1;
}

/* === General Functions === */

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

int16_t window_draw_all( struct DSEKAI_STATE* state ) {
   int16_t i = 0,
      blit_retval = 1;
   struct WINDOW* windows = NULL;

   windows = (struct WINDOW*)memory_lock( state->windows_handle );
   assert( NULL != windows );

   debug_printf( 0, "starting window drawing..." );

   for( i = DSEKAI_WINDOWS_MAX - 1; 1 <= i ; i-- ) {
      /* Only draw active windows. Those windows will recurse to draw their
       * children.
       */
      if(
         WINDOW_TYPE_WINDOW != windows[i].type ||
         WINDOW_FLAG_ACTIVE != (WINDOW_FLAG_ACTIVE & windows[i].flags) ||
         WINDOW_FLAG_DIRTY != (WINDOW_FLAG_DIRTY & windows[i].flags)
      ) {
         continue;
      }

      assert( 0 == windows[i].coords[GUI_W] % PATTERN_W );
      assert( 0 == windows[i].coords[GUI_H] % PATTERN_H );

      blit_retval = gc_window_draw_callbacks[windows[i].type](
         windows[i].id, windows, state );
      if( !blit_retval ) {
         error_printf( "error drawing windows!" );
         goto cleanup;
      }

      windows[i].flags &= ~WINDOW_FLAG_DIRTY;
   }

cleanup:

   windows = (struct WINDOW*)memory_unlock( state->windows_handle );

   return blit_retval;
}

int16_t window_push(
   uint16_t id, uint16_t parent_id, uint8_t type, uint8_t flags,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, uint8_t render_flags,
   int32_t data_scalar, RESOURCE_ID data_res_id, const char* data_string,
   struct DSEKAI_STATE* state
) {
   int16_t retval = 0;
   struct WINDOW* window_new = NULL,
      * parent = NULL;
   struct WINDOW* windows = NULL;

   windows = (struct WINDOW*)memory_lock( state->windows_handle );
   assert( NULL != windows );

   assert( 0 != id );

   /* Check for dupe ID. */
   window_new = window_get( id, windows );
   if( NULL != window_new ) {
      error_printf( "window with ID %u already exists", id );
      retval = 0;
      goto cleanup;
   }

   /* Get parent window for placement, etc. */
   if( 0 != parent_id ) {
      parent = window_get( parent_id, windows );
      assert( NULL != parent );
      if( WINDOW_TYPE_WINDOW != parent->type ) {
         error_printf( "parent window is not WINDOW type" );
         retval = 0;
         goto cleanup;
      }
   }

   /* Find a free window slot. */
   window_new = window_get( 0, windows );
   if( NULL == window_new ) {
      error_printf( "unable to find free window!" );
      retval = 0;
      goto cleanup;
   }

   assert( NULL != window_new );

   memory_zero_ptr( (MEMORY_PTR)window_new, sizeof( struct WINDOW ) );

   /* Sizing callbacks below might need these. */
   if( NULL != data_string ) {
      window_new->data.string = data_string;
   } else if( 0 != data_scalar ) {
      window_new->data.scalar = data_scalar;
   } else if( !resource_compare_id( 0, data_res_id ) ) {
      resource_assign_id( window_new->data.res_id, data_res_id );
   }

   window_new->flags = flags | WINDOW_FLAG_DIRTY | WINDOW_FLAG_ACTIVE;
   window_new->render_flags = render_flags;
   window_new->type = type;
   window_new->id = id;
   window_new->fg = fg;
   window_new->bg = bg;
   window_new->parent_id = parent_id;

   /* X/Y sizing and placement. Sizing comes first, used for centering. */
   window_sizing( id, w, GUI_W, windows, state );
   window_sizing( id, h, GUI_H, windows, state );
   window_placement( window_new, parent, x, GUI_X );
   window_placement( window_new, parent, y, GUI_Y );

cleanup:

   windows = (struct WINDOW*)memory_unlock( state->windows_handle );

   return retval;
}

void window_pop( uint16_t id, struct DSEKAI_STATE* state ) {
   int16_t i = 0,
      id_recurse = 0;
   struct WINDOW* window_out = NULL;
   struct WINDOW* windows = NULL;

   windows = (struct WINDOW*)memory_lock( state->windows_handle );
   assert( NULL != windows );

   window_print_all( windows );

   debug_printf( 1, "popping window %u...", id );

   /* Deactivate any children of the window. */
   for( i = 1 ; DSEKAI_WINDOWS_MAX > i ; i++ ) {
      if(
         WINDOW_FLAG_ACTIVE == (WINDOW_FLAG_ACTIVE & windows[i].flags) &&
         id == windows[i].parent_id
      ) {
         /* Unlock handle before recursing. */
         id_recurse = windows[i].id;
         windows = (struct WINDOW*)memory_unlock( state->windows_handle );
         window_pop( id_recurse, state );
         windows = (struct WINDOW*)memory_lock( state->windows_handle );
         assert( NULL != windows );
      }
   }

   /* Deactivate the window, itself. */
   window_out = window_get( id, windows );
   if( NULL != window_out ) {
      window_out->flags &= ~WINDOW_FLAG_ACTIVE;
   }

   debug_printf( 0, "window %d popped", id );

   window_print_all( windows );

   windows = (struct WINDOW*)memory_unlock( state->windows_handle );
}

int16_t window_modal( struct DSEKAI_STATE* state ) {
   int i = 0;
   int16_t modal = 0;
   struct WINDOW* windows = NULL;

   windows = (struct WINDOW*)memory_lock( state->windows_handle );
   assert( NULL != windows );

   for( i = 1 ; DSEKAI_WINDOWS_MAX > i ; i++ ) {
      if(
         WINDOW_FLAG_ACTIVE == (WINDOW_FLAG_ACTIVE & windows[i].flags) &&
         WINDOW_FLAG_MODAL == (WINDOW_FLAG_MODAL & windows[i].flags)
      ) {
         modal++;
      }
   }

   windows = (struct WINDOW*)memory_unlock( state->windows_handle );

   return modal;
}

