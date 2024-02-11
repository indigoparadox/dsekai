
#define WINDOW_C
#include "unilayer.h"

/* Private Prototypes w/ Sections */

void window_placement(
   int16_t w_id, int16_t coord, uint8_t x_y, struct WINDOW* windows
) SECTION_WINDOW;
int16_t window_parent_placement(
   int16_t w_id, uint8_t x_y, struct WINDOW* windows ) SECTION_WINDOW;
int16_t window_sizing(
   int16_t w_id, uint16_t dimension, uint8_t w_h,
   struct WINDOW* windows ) SECTION_WINDOW;
void window_parent_sizing(
   int16_t w_id, uint8_t w_h, struct WINDOW* windows ) SECTION_WINDOW;
void window_pop_internal( uint16_t id, struct WINDOW* windows ) SECTION_WINDOW;

/* Function Definitions */

/* === Common Internal Utility Functions === */

static struct WINDOW* window_get( uint16_t id, struct WINDOW* windows ) {
   int16_t i = 0;

   for( i = 1 ; WINDOWS_MAX > i ; i++ ) {
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

void window_placement(
   int16_t w_id, int16_t coord, uint8_t x_y, struct WINDOW* windows
) {
   int16_t* p_grid = NULL;
   int16_t parent_w_h = 0;
   uint16_t parent_coords[4];
   struct WINDOW* p = NULL,
      * c = NULL;

   assert( 2 > x_y );

   c = window_get( w_id, windows );

   window_trace_printf( 1, "placing window %u", w_id );

   /* This is only called internally so it should NEVER have a null c! */
   assert( NULL != c );

   assert( x_y < 2 );

   if( 0 < c->parent_id ) {
      p = window_get( c->parent_id, windows );

      /* Parent ID should be validated on push! */
      assert( NULL != p );
   }

   if( NULL == p ) {
      /* Position relative to screen. */
      window_trace_printf( 0, "> window %u rel screen", c->id );
      parent_coords[0] = g_window_screen_coords[0];
      parent_coords[1] = g_window_screen_coords[1];
      parent_coords[2] = g_window_screen_coords[2];
      parent_coords[3] = g_window_screen_coords[3];
      p_grid = g_window_screen_grid;
   } else {
      /* Position relative to parent. */
      window_trace_printf(
         0, "> window %u rel window %d", c->id, c->parent_id );
      parent_coords[0] = 0; /* Parent offset is added in drawing. */
      parent_coords[1] = 0;
      parent_coords[2] = window_get_coords( p, 2 );
      parent_coords[3] = window_get_coords( p, 3 );
      p_grid = p->data.grid;
   }

   if( 0 >= p_grid[x_y] ) {
      /* Setup initial grid. */
      p_grid[x_y] = WINDOW_PADDING_OUTSIDE;
      p_grid[x_y + 2] = WINDOW_PADDING_OUTSIDE;
   }

   switch( (coord & WINDOW_PLACEMENT_AUTO_MASK) ){
   case WINDOW_PLACEMENT_CENTER:
      /* Parent w/h (idx 2 & 3) correspond to x/w (idx 0 & 1) respectively. */
      parent_w_h = (parent_coords[x_y + 2] & WINDOW_SIZE_PHYS_MASK);
      assert( parent_w_h > 0 );

      /* Note: any changes to this formula should also be made to
       *       window_parent_sizing() below.
       */

      /* TODO: Consolidate so these use the same formula in the same place! */

      /* Window width / 2 - Control width / 2 */
      window_update_coords( c, x_y,
         (parent_coords[x_y] & WINDOW_PLACEMENT_PHYS_MASK) +
            ((parent_w_h / 2) - (window_get_coords( c, x_y + 2 ) / 2)) );

      window_trace_printf(
         1, "> window %u center coord(%d) %d + (%d / 2) - (%d / 2): %d",
         c->id, x_y,
         (parent_coords[x_y] & WINDOW_PLACEMENT_PHYS_MASK),
         (parent_coords[x_y + 2] & WINDOW_SIZE_PHYS_MASK),
         window_get_coords( c, x_y + 2 ), window_get_coords( c, x_y ) );
      break;

   case WINDOW_PLACEMENT_RIGHT_BOTTOM:
      /* TODO: Add parent_coords[x/y] offset. */
      window_update_coords( c, x_y, 
         /* Window width - Padding - Control width */
         parent_coords[x_y + 2] - WINDOW_PADDING_OUTSIDE - 
               window_get_coords( c, x_y + 2 ) );
      break;

   case WINDOW_PLACEMENT_GRID_RIGHT_DOWN:
      /* TODO: Add parent_coords[x/y] offset. */
      p_grid[x_y + 2] = p_grid[x_y];
      p_grid[x_y] += window_get_coords( c, x_y + 2 ) + WINDOW_PADDING_INSIDE;
      /* No break; proceed to place according to modified grid. */

   case WINDOW_PLACEMENT_GRID:
      /* TODO: Add parent_coords[x/y] offset. */
      window_trace_printf( 1, "> window %u adding control using grid at: %d",
         c->id, p_grid[x_y + 2] );
      window_update_coords( c, x_y, p_grid[x_y + 2] );
      break;

   default:
      window_update_coords( c, x_y, coord );
      break;
   }
}

int16_t window_parent_placement(
   int16_t w_id, uint8_t x_y, struct WINDOW* windows
) {
   uint16_t parent_coords[4];
   int16_t parent_w_h = 0;
   struct WINDOW* p = NULL,
      * p_p = NULL,
      * c = NULL;
   int16_t retval = 0;

   assert( 2 > x_y );

   c = window_get( w_id, windows );

   window_trace_printf( 1, "placing window %u parent, %u coords(%d)",
      w_id, c->parent_id, x_y );

   /* This is only called internally so it should NEVER have a null c! */
   assert( NULL != c );

   /* Get and verify parent to modify. */
   if( 0 < c->parent_id ) {
      p = window_get( c->parent_id, windows );
   }
   if( NULL == p ) {
      /* No valid parent. */
      return retval;
   }

   if( 0 < p->parent_id ) {
      p_p = window_get( p->parent_id, windows );
   }

   if( NULL == p_p ) {
      /* Position relative to screen. */
      window_trace_printf( 1, "> window %u rel screen", p->id );
      parent_coords[0] = g_window_screen_coords[0];
      parent_coords[1] = g_window_screen_coords[1];
      parent_coords[2] = g_window_screen_coords[2];
      parent_coords[3] = g_window_screen_coords[3];
   } else {
      /* Position relative to parent. */
      window_trace_printf(
         1, "> window %u rel window %d", p->id, p->parent_id );
      parent_coords[0] = window_get_coords( p_p, 0 );
      parent_coords[1] = window_get_coords( p_p, 1 );
      parent_coords[2] = window_get_coords( p_p, 2 );
      parent_coords[3] = window_get_coords( p_p, 3 );
   }

   window_trace_printf(
      1, "> previous coords(%d): %04x", x_y, p->coords[x_y] );

   switch( (p->coords[x_y] & WINDOW_PLACEMENT_AUTO_MASK) ){
   case WINDOW_PLACEMENT_CENTER:
      
      parent_w_h = (parent_coords[x_y + 2] & WINDOW_SIZE_PHYS_MASK);
      assert( parent_coords[x_y + 2] > 0 );

      /* Window width / 2 - Control width / 2 */
      window_update_coords( p, x_y, 
         (parent_coords[x_y] & WINDOW_PLACEMENT_PHYS_MASK) +
            ((parent_w_h / 2) - (window_get_coords( p, x_y + 2 ) / 2) ) );
      
      window_trace_printf(
         1, "> window %u center coord(%d) %d + (%d / 2) - (%d / 2): %d", c->id,
         x_y, parent_coords[x_y] & WINDOW_PLACEMENT_PHYS_MASK, parent_w_h,
         window_get_coords( p, x_y + 2 ), window_get_coords( p, x_y ) );
      break;
   }

   return retval;
}

int16_t window_sizing(
   int16_t w_id, uint16_t dimension, uint8_t w_h,
   struct WINDOW* windows
) {
   uint16_t win_sz; /* What the callback will write to. */
   int16_t retval = 0;
   struct WINDOW* c = NULL;

   assert( 4 > w_h );
   assert( 1 < w_h );

   window_trace_printf( 1, "sizing window ID: %u", w_id );

   c = window_get( w_id, windows );
   assert( NULL != c );

   /* Set the output variable to the FULL input, without window_get_coords(),
    * in case callback needs flags!
    */
   win_sz = dimension;

   /* Width and Height */
   if(
      WINDOW_SIZE_AUTO != (WINDOW_SIZE_AUTO_MASK & dimension) &&
      0 < dimension
   ) {
      c->coords[w_h] = dimension;
      window_trace_printf( 1, "> window %u manual size %d: %d",
         c->id, w_h, dimension );
      retval = dimension;

   } else if(
      gc_window_sz_callbacks[c->type]( w_id, windows, w_h, &win_sz )
   ) {
      /* Set without window_update_coords(), to preserve flags! */
      c->coords[w_h] = win_sz;
      window_trace_printf( 1, "> window %u auto-size %d: %d",
         c->id, w_h, win_sz );
      retval = win_sz;

   } else {
      error_printf( "unable to automatically size window %u", c->id );
      retval = 0;
   }

   return retval;
}

/**
 * \brief Redo parent sizing if parent is dynamically sized to account for new
 *        children. This should be called whenever a child is added.
 */
void window_parent_sizing(
   int16_t w_id, uint8_t w_h, struct WINDOW* windows
) {
   struct WINDOW* c = NULL,
      * p = NULL;
   int16_t sz_mod = 0;
   uint8_t c_extreme_x_y = 0;

   c = window_get( w_id, windows );

   /* Get and verify parent to modify. */
   if( 0 < c->parent_id ) {
      p = window_get( c->parent_id, windows );
   }
   if( NULL == p ) {
      /* No valid parent. */
      return;
   }

   window_trace_printf( 1, "sizing window %u parent, %u sz(%d)",
      c->id, c->parent_id, w_h );

   if(
      WINDOW_PLACEMENT_CENTER != (p->coords[w_h] & WINDOW_PLACEMENT_AUTO_MASK)
   ) {
      /* Parent is not auto-sized. */
      window_trace_printf( 1, "> not autosized (%d)!", p->coords[w_h] );
      return;
   }

   c_extreme_x_y = 
      window_get_coords( c, w_h ) + window_get_coords( c, w_h - 2 );
   if(
      /* Parent's e.g. x + w */
      window_get_coords( p, w_h ) >
      /* Child's e.g. x + w */
      c_extreme_x_y
   ) {
      window_trace_printf( 1, "> parent %u sz(%d) %d smaller than %d",
         p->id, w_h,
         c_extreme_x_y, window_get_coords( p, w_h ) );
      return;
   }

   window_update_coords( p, w_h, c_extreme_x_y );
   /* TODO: Determine whether to use WINDOW_PATTERN_W or _H! */
   sz_mod = window_get_coords( p, w_h ) % WINDOW_PATTERN_W;
   window_trace_printf( 1,
      "> bumping auto window %d sz(%d) %d up by %d to match pattern",
      p->id, w_h, window_get_coords( p, w_h ), (WINDOW_PATTERN_H - sz_mod) );
   
   window_update_coords( p, w_h,
      window_get_coords( p, w_h ) + (WINDOW_PATTERN_H - sz_mod) );

   window_trace_printf( 1, "> auto window %d sz(%d) now %d",
      p->id, w_h, window_get_coords( p, w_h ) );
}

static void window_draw_text(
   struct WINDOW* c, struct WINDOW* p, const char* str, uint16_t str_sz
) {
   int16_t offset_x = 0,
      offset_y = 0;

   if( NULL != p ) {
      offset_x = window_get_coords( p, GUI_X );
      offset_y = window_get_coords( p, GUI_Y );
   }

   assert( NULL != c );

#ifdef WINDOW_TRACE
   debug_printf( 1, "window %u drawing string at %d, %d: %s",
      c->id,
      offset_x + window_get_coords( c, GUI_X ),
      offset_y + window_get_coords( c, GUI_Y ), str );
#endif /* WINDOW_TRACE */

   graphics_string_at( 
      str, str_sz,
      offset_x + window_get_coords( c, GUI_X ),
      offset_y + window_get_coords( c, GUI_Y ), c->fg, c->render_flags );
}

/* === Drawing Callbacks === */

int16_t window_draw_WINDOW( uint16_t w_id, struct WINDOW* windows ) {
   struct WINDOW_FRAME* frames = NULL;
   int16_t x = 0,
      y = 0,
      i = 0,
      x_max = 0,
      y_max = 0,
      blit_retval = 0;
   struct WINDOW* c = NULL;
#ifndef NO_WINDOW_BG
   int16_t bg_idx = -1;
#endif /* NO_WINDOW_BG */
 
   window_trace_printf( 1, "window %u drawing...", w_id );

   c = window_get( w_id, windows );
   assert( NULL != c );

   frames = (struct WINDOW_FRAME*)memory_lock( g_frames_handle );

   window_trace_printf(
      0, "> window %u min: %d, %d; max: %d, %d",
      c->id,
      window_get_coords( c, GUI_X ),
      window_get_coords( c, GUI_Y ), x_max, y_max );

   x_max = window_get_coords( c, GUI_X ) + window_get_coords( c, GUI_W );
   y_max = window_get_coords( c, GUI_Y ) + window_get_coords( c, GUI_H );

   window_trace_printf( 0, "> window %u drawing with frame %d...",
      c->id, c->render_flags );

#ifndef NO_WINDOW_BG
   /* Draw the window background. */
   for(
      y = window_get_coords( c, GUI_Y ) ; y < y_max ; y += WINDOW_PATTERN_H
   ) {
      for(
         x = window_get_coords( c, GUI_X ) ; x < x_max ; x += WINDOW_PATTERN_W
      ) {
         if(
            window_get_coords( c, GUI_X ) == x &&
            window_get_coords( c, GUI_Y ) == y
         ) {
            /* Top Left */
            bg_idx = frames[c->render_flags].tl;

         } else if(
            x_max - WINDOW_PATTERN_W == x && window_get_coords( c, GUI_Y ) == y
         ) {
            /* Top Right */
            bg_idx = frames[c->render_flags].tr;

         } else if(
            window_get_coords( c, GUI_X ) == x && y_max - WINDOW_PATTERN_H == y
         ) {
            /* Bottom Left */
            bg_idx = frames[c->render_flags].bl;
         
         } else if(
            x_max - WINDOW_PATTERN_W == x && y_max - WINDOW_PATTERN_H == y
         ) {
            /* Bottom Right */
            bg_idx = frames[c->render_flags].br;
         
         } else if( x_max - WINDOW_PATTERN_W == x ) {
            /* Right */
            bg_idx = frames[c->render_flags].r;
         
         } else if( window_get_coords( c, GUI_X ) == x ) {
            /* Left */
            bg_idx = frames[c->render_flags].l;
         
         } else if( window_get_coords( c, GUI_Y ) == y ) {
            /* Top */
            bg_idx = frames[c->render_flags].t;
         
         } else if( y_max - WINDOW_PATTERN_H == y ) {
            /* Bottom */
            bg_idx = frames[c->render_flags].b;
         
         } else {
            bg_idx = frames[c->render_flags].c;
         }

         if( 0 <= bg_idx ) {
            blit_retval = graphics_cache_blit_at(
               bg_idx, GRAPHICS_INSTANCE_WINDOW, 0, 0, x, y,
               WINDOW_PATTERN_W, WINDOW_PATTERN_H );
         }

         if( !blit_retval ) {
            /* error_printf( "window blit failed" ); */
            goto cleanup;
         }
      }
   }
#endif /* !NO_WINDOW_BG */

   /* Draw the window's children. */
   for( i = 1 ; WINDOWS_MAX > i ; i++ ) {
      /* Skip inactive and non-children. */
      if(
         WINDOW_FLAG_ACTIVE != (WINDOW_FLAG_ACTIVE & windows[i].flags) ||
         windows[i].parent_id != w_id
      ) {
         continue;
      }

      blit_retval = gc_window_draw_callbacks[windows[i].type](
         windows[i].id, windows );
      if( !blit_retval ) {
         error_printf( "error drawing window children!" );
         goto cleanup;
      }

      windows[i].flags &= ~WINDOW_FLAG_DIRTY;
   }

cleanup:

   if( NULL != frames ) {
      frames = (struct WINDOW_FRAME*)memory_unlock( g_frames_handle );
   }

   return blit_retval;
}

int16_t window_draw_BUTTON( uint16_t w_id, struct WINDOW* windows ) {
   /* TODO */
   return 1;
}

int16_t window_draw_CHECK( uint16_t w_id, struct WINDOW* windows ) {
   /* TODO */
   return 1;
}

int16_t window_get_text(
   struct WINDOW* c, char* buffer, uint16_t buffer_sz
) {
   char* str_ptr = NULL;
   int16_t sz_out = 0;

   if( WINDOW_FLAG_TEXT_PTR == (WINDOW_FLAG_TEXT_PTR & c->flags) ) {
      /* Get the string from a directly passed pointer. */
      str_ptr = (char*)memory_lock( c->data.string );
      sz_out = memory_strnlen_ptr( str_ptr, buffer_sz - 1 );
#ifdef WINDOW_TRACE
      debug_printf( 1, "window %u text str: %s (%d)",
         c->id, str_ptr, sz_out );
#endif /* WINDOW_TRACE */
      memory_strncpy_ptr( buffer, str_ptr, sz_out );
      str_ptr = (char*)memory_unlock( c->data.string );

   } else if( WINDOW_FLAG_TEXT_NUM == (WINDOW_FLAG_TEXT_NUM & c->flags) ) {
      sz_out = dio_itoa(
         buffer, buffer_sz, c->data.scalar, 10 );
#ifdef WINDOW_TRACE
      debug_printf( 1, "window %u text num: %s (%d)",
         c->id, buffer, sz_out );
#endif /* WINDOW_TRACE */
   
   }

   return sz_out;
}

int16_t window_draw_LABEL( uint16_t w_id, struct WINDOW* windows ) {
   int16_t str_sz = 0;
   char str_ptr[WINDOW_STRING_SZ_MAX] = { 0 };
   struct WINDOW* c = NULL,
      * p = NULL;

   c = window_get( w_id, windows );
   assert( NULL != c );

   p = window_get( c->parent_id, windows );

   str_sz = window_get_text(
      window_get( w_id, windows ), str_ptr, WINDOW_STRING_SZ_MAX );

   window_draw_text( c, p, str_ptr, str_sz );

   return 1;
}

int16_t window_draw_SPRITE( uint16_t w_id, struct WINDOW* windows ) {
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
      offset_x = window_get_coords( p, GUI_X );
      offset_y = window_get_coords( p, GUI_Y );
   }

#ifdef WINDOW_TRACE
   debug_printf(
      1, "window %u screen offset: %d, %d", w_id, offset_x, offset_y );
#endif /* WINDOW_TRACE */

   /* Set offset_sprite based on dir flags. */
   dir = ((c->flags & WINDOW_FLAG_SPRITE_DIR_MASK) >> 4);
#ifdef WINDOW_TRACE
   debug_printf( 1, "window %u sprite flags 0x%02x dir 0x%02x",
      w_id, c->flags, dir );
#endif /* WINDOW_TRACE */
   offset_sprite = dir * WINDOW_SPRITE_H;

   /* TODO: Track instance IDs for window sprites. */
   graphics_cache_blit_at(
      c->data.scalar, GRAPHICS_INSTANCE_NONE,
      0, offset_sprite,
      offset_x + window_get_coords( c, GUI_X ),
      offset_y + window_get_coords( c, GUI_Y ),
      WINDOW_SPRITE_W, WINDOW_SPRITE_H );

   if(
      WINDOW_FLAG_SPRITE_BORDER_SINGLE ==
      (WINDOW_FLAG_SPRITE_BORDER_SINGLE & c->flags)
   ) {
#if 0
      graphics_draw_rect(
         offset_x + c->coords[GUI_X],
         offset_y + c->coords[GUI_Y],
         WINDOW_SPRITE_W + 4, WINDOW_SPRITE_H + 4,
         1, GRAPHICS_COLOR_BLACK );

      graphics_draw_rect(
         offset_x + c->coords[GUI_X] + 1,
         offset_y + c->coords[GUI_Y] + 1,
         WINDOW_SPRITE_W + 3, WINDOW_SPRITE_H + 3,
         1, GRAPHICS_COLOR_WHITE );
#endif
   }

   return 1;
}

/* === Sizing Callbacks === */

uint8_t window_sz_WINDOW(
   uint16_t w_id, struct WINDOW* windows, uint8_t w_h, uint16_t* out
) {
   struct WINDOW* w = NULL;
   /* TODO */
   w = window_get( w_id, windows );
   debug_printf( 2, "setting window sz(%d) to %d", w_h, *out );
   w->coords[w_h] = *out;
   return 1;
}

uint8_t window_sz_BUTTON(
   uint16_t w_id, struct WINDOW* windows, uint8_t w_h, uint16_t* out
) {
   /* TODO */
   return 0;
}

uint8_t window_sz_CHECK(
   uint16_t w_id, struct WINDOW* windows, uint8_t w_h, uint16_t* out
) {
   /* TODO */
   return 0;
}

uint8_t window_sz_LABEL(
   uint16_t w_id, struct WINDOW* windows, uint8_t w_h, uint16_t* out
) {
   int16_t str_sz = 0;
   struct GRAPHICS_RECT sz;
   struct WINDOW* c = NULL;
   char str_ptr[WINDOW_STRING_SZ_MAX] = { 0 };

   c = window_get( w_id, windows );
   assert( NULL != c );

   str_sz = window_get_text( c, str_ptr, WINDOW_STRING_SZ_MAX );
   graphics_string_sz( str_ptr, str_sz, 0, &sz );

   if( GUI_W == w_h ) {
      *out = sz.w;
   } else {
      *out = sz.h;
   }

   return 1;
}

uint8_t window_sz_SPRITE(
   uint16_t w_id, struct WINDOW* windows, uint8_t w_h, uint16_t* out
) {
   /* TODO: Verify sprite exists. */
   *out = WINDOW_SPRITE_W + 4; /* For border. */
   return 1;
}

/* === General Functions === */

int16_t window_init(
   uint16_t auto_x, uint16_t auto_y, uint16_t auto_w, uint16_t auto_h
) {
   int16_t retval = 1;

   if( 0 < auto_x ) {
      g_window_screen_coords[0] = auto_x;
   }

   if( 0 < auto_y ) {
      g_window_screen_coords[1] = auto_y;
   }
   if( 0 < auto_w ) {
      g_window_screen_coords[2] = auto_w;
   }

   if( 0 < auto_h ) {
      g_window_screen_coords[3] = auto_h;
   }

   debug_printf( 1, "initalizing windowing system..." );

   g_windows_handle =
      memory_alloc( sizeof( struct WINDOW ), WINDOWS_MAX );
   assert( (MEMORY_HANDLE)NULL != g_windows_handle );

   g_frames_handle = memory_alloc( sizeof( struct WINDOW_FRAME ), 1 );
   assert( (MEMORY_HANDLE)NULL != g_frames_handle );

   window_reload_frames();

   return retval;
}

int16_t window_reload_frames() {
   struct WINDOW_FRAME* frames = NULL;
   int16_t retval = 1;
   RESOURCE_ID pattern_id;

   if( (MEMORY_HANDLE)NULL == g_frames_handle ) {
      return 0;
   }

   frames = (struct WINDOW_FRAME*)memory_lock( g_frames_handle );

   /* TODO: Check for frame load success. */

#if defined( PLATFORM_CURSES )
   frames[0].tr = '\\';
   frames[0].tl = '/';
   frames[0].br = '/';
   frames[0].bl = '\\';
   frames[0].t = '-';
   frames[0].b = '-';
   frames[0].r = '|';
   frames[0].l = '|';
   frames[0].c = ' ';
#else
   resource_id_from_name( &pattern_id, "p_chk_tr", RESOURCE_EXT_GRAPHICS );
   frames[0].tr = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );
   
   resource_id_from_name( &pattern_id, "p_chk_tl", RESOURCE_EXT_GRAPHICS );
   frames[0].tl = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );

   resource_id_from_name( &pattern_id, "p_chk_br", RESOURCE_EXT_GRAPHICS );
   frames[0].br = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );

   resource_id_from_name( &pattern_id, "p_chk_bl", RESOURCE_EXT_GRAPHICS );
   frames[0].bl = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );
   
   resource_id_from_name( &pattern_id, "p_chk_t", RESOURCE_EXT_GRAPHICS );
   frames[0].t  = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );
   
   resource_id_from_name( &pattern_id, "p_chk_b", RESOURCE_EXT_GRAPHICS );
   frames[0].b  = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );
   
   resource_id_from_name( &pattern_id, "p_chk_r", RESOURCE_EXT_GRAPHICS );
   frames[0].r  = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );
   
   resource_id_from_name( &pattern_id, "p_chk_l", RESOURCE_EXT_GRAPHICS );
   frames[0].l  = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );
   
   resource_id_from_name( &pattern_id, "p_chk_c", RESOURCE_EXT_GRAPHICS );
   frames[0].c  = graphics_cache_load_bitmap(
      pattern_id, GRAPHICS_BMP_FLAG_TYPE_TILE );
#endif /* PLATFORM_CURSES */

   frames = (struct WINDOW_FRAME*)memory_unlock( g_frames_handle );

   return retval;
}

void window_shutdown() {
   /* TODO: Verify all windows have closed/freed their resources. */
   memory_free( g_windows_handle );
   g_windows_handle = (MEMORY_HANDLE)NULL;
   memory_free( g_frames_handle );
   g_frames_handle = (MEMORY_HANDLE)NULL;
}

int16_t window_draw_all() {
   int16_t i = 0,
      blit_retval = 1;
   struct WINDOW* windows = NULL;

   windows = (struct WINDOW*)memory_lock( g_windows_handle );
   assert( NULL != windows );

   debug_printf( 0, "starting window drawing..." );

   /* TODO: Don't redraw windows each cycle; use a dirty grid to redraw parts.
    */

   for( i = WINDOWS_MAX - 1; 1 /* TODO */ <= i ; i-- ) {
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

      assert( 0 ==
         window_get_coords( &(windows[i]), GUI_W ) % WINDOW_PATTERN_W );
      assert( 0 == 
         window_get_coords( &(windows[i]), GUI_H ) % WINDOW_PATTERN_H );

      assert( NULL != windows );
      blit_retval = gc_window_draw_callbacks[windows[i].type](
         windows[i].id, windows );
      if( !blit_retval ) {
         error_printf( "error drawing windows!" );
         goto cleanup;
      }

      windows[i].flags &= ~WINDOW_FLAG_DIRTY;
   }

cleanup:

   if( NULL != windows ) {
      windows = (struct WINDOW*)memory_unlock( g_windows_handle );
   }

   return blit_retval;
}

int16_t window_push(
   uint16_t id, uint16_t parent_id, uint8_t type, uint8_t flags,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, uint8_t render_flags,
   int32_t data_scalar, const char* data_string
) {
   int16_t retval = 0,
      string_sz = 0;
   struct WINDOW* window_new = NULL,
      * parent = NULL,
      * windows = NULL;
   char* str_ptr = NULL;

   windows = (struct WINDOW*)memory_lock( g_windows_handle );
   assert( NULL != windows );

   assert( 0 != id );
   assert( id != parent_id );

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
      string_sz = memory_strnlen_ptr( data_string, WINDOW_STRING_SZ_MAX );
      
      /* Allocate new string handle. */
      window_new->data.string = memory_alloc( 1, string_sz + 1 );
      assert( (MEMORY_HANDLE)NULL != window_new->data.string );

      /* Copy incoming string into handle. */
      str_ptr = (char*)memory_lock( window_new->data.string );
      assert( NULL != str_ptr );
      memory_strncpy_ptr( str_ptr, data_string, string_sz );
      str_ptr = (char*)memory_unlock( window_new->data.string );

      /* Ensure flag consistency. */
      flags &= ~WINDOW_FLAG_TEXT_MASK;
      flags |= WINDOW_FLAG_TEXT_PTR;
   } else if( 0 != data_scalar ) {
      window_new->data.scalar = data_scalar;

      if( WINDOW_TYPE_LABEL == type ) {
         /* Ensure flag consistency. */
         flags &= ~WINDOW_FLAG_TEXT_MASK;
         flags |= WINDOW_FLAG_TEXT_NUM;
      }
   }

   window_new->flags = flags | WINDOW_FLAG_DIRTY | WINDOW_FLAG_ACTIVE;
   window_new->render_flags = render_flags;
   window_new->type = type;
   window_new->id = id;
   window_new->fg = fg;
   window_new->bg = bg;
   window_new->parent_id = parent_id;
   window_new->coords[GUI_X] = x;
   window_new->coords[GUI_Y] = y;

   /* X/Y sizing and placement. Sizing comes first, used for centering. */
   window_sizing( id, w, GUI_W, windows );
   window_sizing( id, h, GUI_H, windows );
   window_placement( id, x, GUI_X, windows );
   window_placement( id, y, GUI_Y, windows );
   window_parent_sizing( id, GUI_W, windows );
   window_parent_sizing( id, GUI_H, windows );
   window_parent_placement( id, GUI_X, windows );
   window_parent_placement( id, GUI_Y, windows );

   /* Increment modal counter if MODAL flag specified. */
   if( WINDOW_FLAG_MODAL == (WINDOW_FLAG_MODAL & window_new->flags) ) {
      g_window_modals++;
      debug_printf( 1, "incremented modal counter: %d", g_window_modals );
   }

cleanup:

   windows = (struct WINDOW*)memory_unlock( g_windows_handle );

   return retval;
}

void window_pop_internal( uint16_t id, struct WINDOW* windows ) {
   int16_t i = 0,
      id_recurse = 0;
   struct WINDOW* window_out = NULL;

   /* Deallocate the window resources. */
   window_out = window_get( id, windows );
   
#ifdef WINDOW_TRACE
   debug_printf( 1, "window %d popping...", id );
#endif /* WINDOW_TRACE */

   if( NULL != window_out ) {
      if(
         WINDOW_TYPE_LABEL == window_out->type &&
         WINDOW_FLAG_TEXT_PTR == (WINDOW_FLAG_TEXT_MASK & window_out->flags)
      ) {
#ifdef WINDOW_TRACE
         debug_printf( 1, "window %u freeing memory for flags %02x: %p...",
            id, (WINDOW_FLAG_TEXT_MASK & window_out->flags),
            window_out->data.string );
#endif /* WINDOW_TRACE */
         memory_free( window_out->data.string );
         window_out->data.string = (MEMORY_HANDLE)NULL;
      }
      
      if( WINDOW_FLAG_MODAL == (WINDOW_FLAG_MODAL & window_out->flags) ) {
         g_window_modals--;
         debug_printf( 1, "decremented modal counter: %d", g_window_modals );
      }

      window_out->flags &= ~WINDOW_FLAG_ACTIVE;
      window_out->id = 0;
   }

   /* Deactivate any children of the window. */
   for( i = 1 ; WINDOWS_MAX > i ; i++ ) {
      if(
         WINDOW_FLAG_ACTIVE == (WINDOW_FLAG_ACTIVE & windows[i].flags) &&
         id == windows[i].parent_id
      ) {
         id_recurse = windows[i].id;
         window_pop_internal( id_recurse, windows );
      }
   }

}

void window_pop( uint16_t id ) {
   struct WINDOW* windows = NULL;

   windows = (struct WINDOW*)memory_lock( g_windows_handle );
   assert( NULL != windows );

   window_pop_internal( id, windows );

   windows = (struct WINDOW*)memory_unlock( g_windows_handle );
}

void window_refresh( uint16_t w_id ) {
   struct WINDOW* windows = NULL,
      * w = NULL;

   windows = (struct WINDOW*)memory_lock( g_windows_handle );
   assert( NULL != windows );

   w = window_get( w_id, windows );
   if( NULL != w ) {
      w->flags |= WINDOW_FLAG_DIRTY;
   }

   windows = (struct WINDOW*)memory_unlock( g_windows_handle );
}

