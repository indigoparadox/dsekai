
#include "dstypes.h"

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

   state = memory_lock( state_handle );

   if( !initialized ) {

      /* TODO: Generate this dynamically. */
#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( tile_field_grass, &(state->map.tileset[0]) );
      graphics_load_bitmap( tile_field_brick_wall, &(state->map.tileset[1]) );
      graphics_load_bitmap( tile_field_tree, &(state->map.tileset[2]) );
#endif /* !DISABLE_GRAPHICS */

#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( sprite_robe, &(state->mobiles[0].sprite) );
#endif /* !DISABLE_GRAPHICS */
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

#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( sprite_princess, &(state->mobiles[1].sprite) );
#endif /* !DISABLE_GRAPHICS */
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

      tilemap_load( map_field, &(state->map) );

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      tilemap_draw( &(state->map), state );

      initialized = 1;
   }

   graphics_loop_start();

   in_char = 0;

   if( 0 == state->windows_count ) {
      topdown_draw( state );
   }

   if( !state->window_shown ) {
#ifndef HIDE_WELCOME_DIALOG
      window_push( WINDOW_CENTERED, WINDOW_CENTERED, 80, 64, 0, state );
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
      if( 0 < state->windows_count ) { break; }
      if( !tilemap_collide(
         &(state->map), state->mobiles[state->player_idx].coords.x,
         state->mobiles[state->player_idx].coords.y - 1 )
      ) {
         mobile_walk_start( &(state->mobiles[state->player_idx]), 0, -1 );
      }
      break;

   case INPUT_KEY_LEFT:
      if( 0 < state->windows_count ) { break; }
      if( !tilemap_collide(
         &(state->map), state->mobiles[state->player_idx].coords.x - 1,
         state->mobiles[state->player_idx].coords.y )
      ) {
         mobile_walk_start( &(state->mobiles[state->player_idx]), -1, 0 );
      }
      break;

   case INPUT_KEY_DOWN:
      if( 0 < state->windows_count ) { break; }
      if( !tilemap_collide(
         &(state->map), state->mobiles[state->player_idx].coords.x,
         state->mobiles[state->player_idx].coords.y + 1 )
      ) {
         mobile_walk_start( &(state->mobiles[state->player_idx]), 0, 1 );
      }
      break;

   case INPUT_KEY_RIGHT:
      if( 0 < state->windows_count ) { break; }
      if( !tilemap_collide(
         &(state->map), state->mobiles[state->player_idx].coords.x + 1,
         state->mobiles[state->player_idx].coords.y )
      ) {
         mobile_walk_start( &(state->mobiles[state->player_idx]), 1, 0 );
      }
      break;

   case INPUT_KEY_OK:
      window_pop( state );
      tilemap_refresh_tiles( &(state->map) );
      break;

   case INPUT_KEY_QUIT:
      state = memory_unlock( state_handle );
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
      state->screen_scroll_x_tgt = state->screen_scroll_x + SCREEN_W;

   } else if(
      state->mobiles[state->player_idx].coords.y >=
         state->screen_scroll_y + SCREEN_TH
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y + SCREEN_H;

   } else if(
      state->mobiles[state->player_idx].coords.x < state->screen_scroll_tx
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x - SCREEN_W;

   } else if(
      state->mobiles[state->player_idx].coords.y < state->screen_scroll_ty
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y - SCREEN_H;
   }
   state = memory_unlock( state_handle );

   graphics_loop_end();

   return 1;
}

void topdown_deinit( MEMORY_HANDLE state_handle ) {
   int i = 0;
   struct DSEKAI_STATE* state = NULL;

   state = memory_lock( state_handle );

   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      mobile_deinit( &(state->mobiles[i]) );
   }

   tilemap_deinit( &(state->map) );

   state = memory_unlock( state_handle );
}

