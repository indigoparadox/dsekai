
#include "dstypes.h"

#include "data/tiles.h"
#ifndef USE_JSON_MAPS
#include "../gen/win16/map_field.h"
#endif /* USE_JSON_MAPS */

#define INPUT_BLOCK_DELAY 5
#define TOPDOWN_MOBILES_MAX 10

int topdown_draw( struct DSEKAI_STATE* state ) {
   int in_char = 0,
      i = 0;

   /*
   If the screen is scrolling, prioritize that before accepting more
   commands or drawing mobiles.
   */
   if(
      state->screen_scroll_x_tgt != state->screen_scroll_x ||
      state->screen_scroll_y_tgt != state->screen_scroll_y 
   ) {
      if( state->screen_scroll_x_tgt > state->screen_scroll_x ) {
         state->screen_scroll_x += TILE_W;
         state->screen_scroll_tx = state->screen_scroll_x / TILE_W;
      }
      if( state->screen_scroll_y_tgt > state->screen_scroll_y ) {
         state->screen_scroll_y += TILE_H;
         state->screen_scroll_ty = state->screen_scroll_y / TILE_H;
      }
      if( state->screen_scroll_x_tgt < state->screen_scroll_x ) {
         state->screen_scroll_x -= TILE_W;
         state->screen_scroll_tx = state->screen_scroll_x / TILE_W;
      }
      if( state->screen_scroll_y_tgt < state->screen_scroll_y ) {
         state->screen_scroll_y -= TILE_H;
         state->screen_scroll_ty = state->screen_scroll_y / TILE_H;
      }

      state->input_blocked_countdown = INPUT_BLOCK_DELAY;

      assert( 0 <= state->screen_scroll_x );
      assert( 0 <= state->screen_scroll_tx );

      tilemap_refresh_tiles( &(state->map) );
#ifdef ANIMATE_SCREEN_MOVEMENT
#ifndef DISABLE_GRAPHICS
      tilemap_draw( &(state->map), state );

      graphics_flip();
#endif /* !DISABLE_GRAPHICS */
#endif /* ANIMATE_SCREEN_MOVEMENT */

      /* Drain input. */
      in_char = input_poll();
      if( INPUT_KEY_QUIT == in_char ) {
         return 0;
      }

      if(
         state->screen_scroll_y == state->screen_scroll_y_tgt &&
         state->screen_scroll_x == state->screen_scroll_x_tgt
      ) {
         /* Screen scroll finished. */
         state->mobiles[state->player_idx].coords_prev.x =
            state->mobiles[state->player_idx].coords.x;
         state->mobiles[state->player_idx].coords_prev.y =
            state->mobiles[state->player_idx].coords.y;
         state->mobiles[state->player_idx].steps_x = 0;
         state->mobiles[state->player_idx].steps_y = 0;
      }

      /* Keep running. */
      return 1;
   }

#ifndef DISABLE_GRAPHICS
   tilemap_draw( &(state->map), state );
#endif /* !DISABLE_GRAPHICS */

   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      if(
         state->mobiles[i].coords.x < state->screen_scroll_tx ||
         state->mobiles[i].coords.y < state->screen_scroll_ty ||
         state->mobiles[i].coords.x > state->screen_scroll_tx + SCREEN_TW ||
         state->mobiles[i].coords.y > state->screen_scroll_ty + SCREEN_TH
      ) {
         /* Mobile is off-screen. */
         continue;
      }
#ifndef DISABLE_GRAPHICS
      mobile_draw(
         &(state->mobiles[i]),
         state->walk_offset, state->screen_scroll_x, state->screen_scroll_y );
      assert( NULL != &(state->mobiles[state->player_idx]) );
#endif /* !DISABLE_GRAPHICS */
   }

   /* Keep running. */
   return 1;
}

int topdown_loop( MEMORY_HANDLE state_handle ) {
   int i = 0;
   uint8_t in_char = 0;
   static int initialized = 0;
   struct DSEKAI_STATE* state = NULL;
   MEMORY_HANDLE welcome_string_handle = NULL;
   char* welcome_string = NULL;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   if( !initialized ) {

      /* TODO: Generate this dynamically. */

      state->mobiles[0].sprite = sprite_robe;
      state->mobiles[0].hp = 100;
      state->mobiles[0].mp = 100;
      state->mobiles[0].coords.x = 3;
      state->mobiles[0].coords.y = 4;
      state->mobiles[0].coords_prev.x = 3;
      state->mobiles[0].coords_prev.y = 4;
      state->mobiles[0].steps_x = 0;
      state->mobiles[0].steps_y = 0;
      state->mobiles[0].inventory = NULL;
      state->mobiles_count++;

      state->mobiles[1].sprite = sprite_princess;
      state->mobiles[1].hp = 100;
      state->mobiles[1].mp = 100;
      state->mobiles[1].coords.x = 5;
      state->mobiles[1].coords.y = 5;
      state->mobiles[1].coords_prev.x = 5;
      state->mobiles[1].coords_prev.y = 5;
      state->mobiles[1].steps_x = 0;
      state->mobiles[1].steps_y = 0;
      state->mobiles[1].inventory = NULL;
      state->mobiles_count++;

#ifdef USE_JSON_MAPS
      tilemap_load( map_field, &(state->map) );
#else
      memory_copy_ptr( (MEMORY_PTR)&(state->map), (MEMORY_PTR)&gc_map_field,
         sizeof( struct TILEMAP ) );
#endif /* USE_JSON_MAPS */
      memory_copy_ptr( (MEMORY_PTR)&(state->map.tileset[0]),
         (MEMORY_PTR)&(gc_tiles_field[0]),
         TILEMAP_TILESETS_MAX * sizeof( struct TILESET_TILE ) );
      tilemap_refresh_tiles( &(state->map) );

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      tilemap_draw( &(state->map), state );

      initialized = 1;
   }

   graphics_loop_start();

   in_char = 0;

   if( 0 >= window_modal( state ) ) {
      topdown_draw( state );
   }

   if( !state->window_shown ) {
      window_push(
         0x111, WINDOW_STATUS_VISIBLE,
         0, (SCREEN_TH * 16), STATUS_WINDOW_W, STATUS_WINDOW_H, 0, state );
#ifndef HIDE_WELCOME_DIALOG
      window_push(
         0x1212, WINDOW_STATUS_MODAL,
         WINDOW_CENTERED, WINDOW_CENTERED, 80, 64, 0, state );
      welcome_string_handle = memory_alloc( 9, 1 );
      welcome_string = (char*)memory_lock( welcome_string_handle );
      memory_copy_ptr( welcome_string, "Welcome!", 8 );
      welcome_string = (char*)memory_unlock( welcome_string_handle );
      control_push(
         0x2323, CONTROL_TYPE_LABEL, CONTROL_STATE_ENABLED,
         -1, -1, -1, -1, GRAPHICS_COLOR_BLACK, GRAPHICS_COLOR_MAGENTA, 1,
         welcome_string_handle, 0x1212, state );
#endif /* !HIDE_WELCOME_DIALOG */
      state->window_shown = 1;
   }

   window_draw_all( state );

   graphics_flip();

   if( state->input_blocked_countdown ) {
      state->input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   switch( in_char ) {
   case INPUT_KEY_UP:
      if( 0 < window_modal( state ) ) { break; }
      if( !tilemap_collide(
         &(state->map), state->mobiles[state->player_idx].coords.x,
         state->mobiles[state->player_idx].coords.y - 1 )
      ) {
         mobile_walk_start( &(state->mobiles[state->player_idx]), 0, -1 );
      }
      break;

   case INPUT_KEY_LEFT:
      if( 0 < window_modal( state ) ) { break; }
      if( !tilemap_collide(
         &(state->map), state->mobiles[state->player_idx].coords.x - 1,
         state->mobiles[state->player_idx].coords.y )
      ) {
         mobile_walk_start( &(state->mobiles[state->player_idx]), -1, 0 );
      }
      break;

   case INPUT_KEY_DOWN:
      if( 0 < window_modal( state ) ) { break; }
      if( !tilemap_collide(
         &(state->map), state->mobiles[state->player_idx].coords.x,
         state->mobiles[state->player_idx].coords.y + 1 )
      ) {
         mobile_walk_start( &(state->mobiles[state->player_idx]), 0, 1 );
      }
      break;

   case INPUT_KEY_RIGHT:
      if( 0 < window_modal( state ) ) { break; }
      if( !tilemap_collide(
         &(state->map), state->mobiles[state->player_idx].coords.x + 1,
         state->mobiles[state->player_idx].coords.y )
      ) {
         mobile_walk_start( &(state->mobiles[state->player_idx]), 1, 0 );
      }
      break;

   case INPUT_KEY_OK:
      window_pop( 0x1212, state );
      tilemap_refresh_tiles( &(state->map) );
      break;

   case INPUT_KEY_QUIT:
      window_pop( 0x111, state );
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
      topdown_deinit( state_handle );
      return 0;
   }

   /* Animate. */
   if( 10 < state->semi_cycles ) {
      state->semi_cycles = 0;

      if( 0 == state->walk_offset ) {
         /* Walk offset must be divisible by 2 for CGA blitting to work. */
         state->walk_offset = 2;
      } else {
         state->walk_offset = 0;
      }

   } else {
      state->semi_cycles++;
   }

   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      mobile_animate( &(state->mobiles[i]), &(state->map) );
   }

   /* Scroll the screen by one if the player goes off-screen. */
   if(
      state->mobiles[state->player_idx].coords.x >=
         state->screen_scroll_tx + SCREEN_TW
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x + SCREEN_MAP_W;
      debug_printf( 1, "scrolling screen right to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      state->mobiles[state->player_idx].coords.y >=
         state->screen_scroll_y + SCREEN_TH
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y + SCREEN_MAP_H;
      debug_printf( 1, "scrolling screen down to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      state->mobiles[state->player_idx].coords.x < state->screen_scroll_tx
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x - SCREEN_MAP_W;
      debug_printf( 1, "scrolling screen left to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      state->mobiles[state->player_idx].coords.y < state->screen_scroll_ty
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y - SCREEN_MAP_H;
      debug_printf( 1, "scrolling screen up to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );
   }
   state = (struct DSEKAI_STATE*)memory_unlock( state_handle );

   graphics_loop_end();

   return 1;
}

void topdown_deinit( MEMORY_HANDLE state_handle ) {
   int i = 0;
   struct DSEKAI_STATE* state = NULL;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      mobile_deinit( &(state->mobiles[i]) );
   }

   tilemap_deinit( &(state->map) );

   state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
}

