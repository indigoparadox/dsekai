
#include "dsekai.h"

#ifdef RESOURCE_FILE

#include "tmjson.h"

#endif /* RESOURCE_FILE */

extern const struct TILEMAP gc_map_field;

#define engine_mapize_internal( map ) gc_map_ ## map

#define engine_mapize( map ) engine_mapize_internal( map )

/**
 * \brief Draw currently on-screen portion of a TILEMAP.
 * \param state ::MEMORY_PTR to the current engine state, used to determine
 *              what is currently on-screen.
 */
void topdown_draw_tilemap( struct DSEKAI_STATE* state ) {
   int x = 0,
      y = 0;
   uint8_t tile_id = 0;
   uint16_t
      viewport_tx2 = 0,
      viewport_ty2 = 0;

   /* Tile-indexed rectangle of on-screen tiles. */
   viewport_tx2 = state->screen_scroll_tx + SCREEN_TW;
   viewport_ty2 = state->screen_scroll_ty + SCREEN_TH;

   assert( viewport_tx2 <= TILEMAP_TW );
   assert( viewport_ty2 <= TILEMAP_TH );

   for( y = state->screen_scroll_ty ; viewport_ty2 > y ; y++ ) {
      for( x = state->screen_scroll_tx ; viewport_tx2 > x ; x++ ) {
#ifndef IGNORE_DIRTY
         if(
            !(state->map.tiles_flags[(y * TILEMAP_TW) + x] &
               TILEMAP_TILE_FLAG_DIRTY)
         ) {
            continue;
         }
#endif /* !IGNORE_DIRTY */

         assert( y < TILEMAP_TH );
         assert( x < TILEMAP_TW );
         assert( y >= 0 );
         assert( x >= 0 );

         state->map.tiles_flags[(y * TILEMAP_TW) + x] &=
            ~TILEMAP_TILE_FLAG_DIRTY;

         /* Grab the left byte if even or the right if odd. */
         tile_id = tilemap_get_tile_id( &(state->map), x, y );

         if( tile_id >= TILEMAP_TILESETS_MAX ) {
            error_printf( "invalid tile id: %d", tile_id );
            continue;
         }

         /* Blit the tile. */
         graphics_blit_at(
            state->map.tileset[tile_id].image,
            0, 0,
            (x * TILE_W) - state->screen_scroll_x,
            (y * TILE_H) - state->screen_scroll_y, TILE_W, TILE_H );
      }
   }
}

int topdown_draw( struct DSEKAI_STATE* state, struct GRAPHICS_ARGS* args ) {
   int in_char = 0,
      i = 0,
      x_offset = 0,
      y_offset = 0;

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
      topdown_draw_tilemap( state );

      /* Drain input. */
      in_char = input_poll();
      if( INPUT_KEY_QUIT == in_char ) {
         graphics_flip( args );
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
      graphics_flip( args );
      return 1;
   }

   topdown_draw_tilemap( state );

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if( !state->mobiles[i].active ) {
         continue;
      }

      if(
         state->mobiles[i].coords.x < state->screen_scroll_tx ||
         state->mobiles[i].coords.y < state->screen_scroll_ty ||
         state->mobiles[i].coords.x > state->screen_scroll_tx + SCREEN_TW ||
         state->mobiles[i].coords.y > state->screen_scroll_ty + SCREEN_TH
      ) {
         /* Mobile is off-screen. */
         continue;
      }

      /* Figure out direction to offset steps in. */
      if( state->mobiles[i].coords_prev.x > state->mobiles[i].coords.x ) {
         x_offset = SPRITE_W - state->mobiles[i].steps_x;
         y_offset = 0;
      } else if(
         state->mobiles[i].coords_prev.x < state->mobiles[i].coords.x
      ) {
         x_offset = (SPRITE_W - state->mobiles[i].steps_x) * -1;
         y_offset = 0;
      } else if(
         state->mobiles[i].coords_prev.y > state->mobiles[i].coords.y
      ) {
         x_offset = 0;
         y_offset = SPRITE_H - state->mobiles[i].steps_y;
      } else if(
         state->mobiles[i].coords_prev.y < state->mobiles[i].coords.y
      ) {
         x_offset = 0;
         y_offset = (SPRITE_H - state->mobiles[i].steps_y) * -1;
      } else {
         x_offset = 0;
         y_offset = 0;
      }

      assert( SPRITE_W > x_offset );
      assert( SPRITE_H > y_offset );

      if( state->mobiles[i].active ) {
         graphics_blit_at(
            state->mobiles[i].sprite,
            state->ani_sprite_x,
            state->mobiles[i].dir * SPRITE_H,
            ((state->mobiles[i].coords.x * SPRITE_W) + x_offset)
               - state->screen_scroll_x,
            ((state->mobiles[i].coords.y * SPRITE_H) + y_offset)
               - state->screen_scroll_y,
            SPRITE_W, SPRITE_H );
      }
   }

   /* Keep running. */
   graphics_flip( args );
   return 1;
}

void topdown_focus_player( struct DSEKAI_STATE* state ) {
   int player_x_px = 0,
      player_y_px = 0;

   player_x_px = state->mobiles[state->player_idx].coords.x * TILE_W;
   player_y_px = state->mobiles[state->player_idx].coords.y * TILE_H;

   state->screen_scroll_x_tgt = (player_x_px / SCREEN_MAP_W) * SCREEN_MAP_W;
   state->screen_scroll_x = (player_x_px / SCREEN_MAP_W) * SCREEN_MAP_W;
   state->screen_scroll_tx = state->screen_scroll_x / TILE_W;

   state->screen_scroll_y_tgt = (player_y_px / SCREEN_MAP_H) * SCREEN_MAP_H;
   state->screen_scroll_y = (player_y_px / SCREEN_MAP_H) * SCREEN_MAP_H;
   state->screen_scroll_ty = state->screen_scroll_y / TILE_H;

   debug_printf( 2, "player x, y: %d, %d (%d, %d)", 
      state->mobiles[state->player_idx].coords.x,
      state->mobiles[state->player_idx].coords.y,
      player_x_px, player_y_px );

}

int topdown_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args ) {
   int i = 0;
   uint8_t in_char = 0;
   struct DSEKAI_STATE* state = NULL;
   int retval = 1;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
      retval = 0;
      goto cleanup;
   }

   if( ENGINE_STATE_OPENING == state->engine_state ) {

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      topdown_draw_tilemap( state );

      topdown_focus_player( state );
      tilemap_refresh_tiles( &(state->map) );

      window_push(
         WINDOW_ID_STATUS, WINDOW_STATUS_VISIBLE,
         0, (SCREEN_TH * 16), STATUS_WINDOW_W, STATUS_WINDOW_H, 0, state );

#if 0
#ifndef HIDE_WELCOME_DIALOG
#ifndef PLATFORM_PALM
      state->engine_state = TOPDOWN_STATE_WELCOME;
#ifdef RESOURCE_FILE
      window_prefab_dialog(
         WINDOW_ID_WELCOME, 1, "assets/16x16x4/s_pncess.bmp", state );
#else
      window_prefab_dialog(
         WINDOW_ID_WELCOME, 1, s_pncess, state );
#endif /* RESOURCE_FILE */
#endif /* PLATFORM_PALM */
#endif /* !HIDE_WELCOME_DIALOG */
#endif

      state->engine_state = ENGINE_STATE_RUNNING;
   }

   graphics_loop_start();

   in_char = 0;

   if( 0 >= window_modal( state ) ) {
      topdown_draw( state, args );
   }

   window_draw_all( state );

   if( state->input_blocked_countdown ) {
      state->input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   #define handle_movement( dir_move ) if( 0 < window_modal( state ) ) { break; } state->mobiles[state->player_idx].dir = dir_move; if( !tilemap_collide( &(state->mobiles[state->player_idx]), dir_move, &(state->map) ) && NULL == mobile_get_facing( &(state->mobiles[state->player_idx]), state ) ) { mobile_walk_start( &(state->mobiles[state->player_idx]), dir_move ); }

   switch( in_char ) {
   case INPUT_KEY_UP:
      handle_movement( MOBILE_DIR_NORTH );
      break;

   case INPUT_KEY_LEFT:
      handle_movement( MOBILE_DIR_WEST );
      break;

   case INPUT_KEY_DOWN:
      handle_movement( MOBILE_DIR_SOUTH );
      break;

   case INPUT_KEY_RIGHT:
      handle_movement( MOBILE_DIR_EAST );
      break;

   case INPUT_KEY_OK:
      if( 0 >= window_modal( state ) ) {
         /* Try to interact with facing mobile. */
         mobile_interact(
            &(state->mobiles[state->player_idx]),
            mobile_get_facing( &(state->mobiles[state->player_idx]), state ),
            &(state->map) );
      } else {
         /* Try to close any windows that are open. */
         window_pop( WINDOW_ID_WELCOME, state );
         window_pop( WINDOW_ID_SCRIPT_SPEAK, state );
      }
      tilemap_refresh_tiles( &(state->map) );
      break;

   case INPUT_KEY_QUIT:
      window_pop( WINDOW_ID_STATUS, state );
      retval = 0;
      goto cleanup;
   }

   mobile_state_animate( state );
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if( 0 >= window_modal( state ) ) {
         /* Pause scripts if modal window is pending. */
         mobile_execute( &(state->mobiles[i]), state );
      }
      if( !state->mobiles[i].active ) {
         continue;
      }
      mobile_animate( &(state->mobiles[i]), &(state->map) );
   }

   /* Scroll the screen by one if the player goes off-screen. */
   if(
      state->mobiles[state->player_idx].coords.x >=
         state->screen_scroll_tx + SCREEN_TW
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x + SCREEN_MAP_W;
      debug_printf( 2, "scrolling screen right to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      state->mobiles[state->player_idx].coords.y >=
         state->screen_scroll_ty + SCREEN_TH
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y + SCREEN_MAP_H;
      debug_printf( 2, "scrolling screen down to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      state->mobiles[state->player_idx].coords.x < state->screen_scroll_tx
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x - SCREEN_MAP_W;
      debug_printf( 2, "scrolling screen left to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      state->mobiles[state->player_idx].coords.y < state->screen_scroll_ty
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y - SCREEN_MAP_H;
      debug_printf( 2, "scrolling screen up to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );
   }

   graphics_loop_end();

cleanup:

   if( NULL != state ) {
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
   }

   graphics_flip( args );
   return retval;
}

