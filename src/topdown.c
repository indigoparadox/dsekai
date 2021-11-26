
#include "dsekai.h"

/**
 * \brief Draw currently on-screen portion of a TILEMAP.
 * \param state ::MEMORY_PTR to the current engine state, used to determine
 *              what is currently on-screen.
 */
void topdown_draw_tilemap( struct DSEKAI_STATE* state ) {
   int x = 0,
      y = 0,
      i = 0;
   uint8_t tile_id = 0;
   uint16_t
      tile_px = 0, /* Tile on-screen X in pixels. */
      tile_py = 0, /* Tile on-screen Y in pixels. */
      viewport_tx2 = 0,
      viewport_ty2 = 0;
   struct TOPDOWN_STATE* gstate = NULL;

   gstate = (struct TOPDOWN_STATE*)memory_lock( state->engine_state_handle );

   /* Tile-indexed rectangle of on-screen tiles. */
   viewport_tx2 = gstate->screen_scroll_tx + SCREEN_TW;
   viewport_ty2 = gstate->screen_scroll_ty + SCREEN_TH;

   assert( viewport_tx2 <= TILEMAP_TW );
   assert( viewport_ty2 <= TILEMAP_TH );

   /* Iterate over only on-screen tiles. */
   for( y = gstate->screen_scroll_ty ; viewport_ty2 > y ; y++ ) {
      for( x = gstate->screen_scroll_tx ; viewport_tx2 > x ; x++ ) {

         tile_px = (x * TILE_W) - gstate->screen_scroll_x;
         tile_py = (y * TILE_H) - gstate->screen_scroll_y;

         /* Mark as dirty any on-screen tiles under an animation. */
         for( i = 0 ; ANIMATE_ANIMATIONS_MAX > i ; i++ ) {

            if(
               ANIMATE_FLAG_ACTIVE ==
                  (g_animations[i].flags & ANIMATE_FLAG_ACTIVE) &&
               (g_animations[i].x - SCREEN_MAP_X)
                  <= tile_px &&
               (g_animations[i].x - SCREEN_MAP_X) + g_animations[i].w
                  > tile_px &&
               (g_animations[i].y - SCREEN_MAP_Y)
                  <= tile_py &&
               (g_animations[i].y - SCREEN_MAP_Y) + g_animations[i].h
                  > tile_py
            ) {
               state->map.tiles_flags[(y * TILEMAP_TW) + x] |=
                  TILEMAP_TILE_FLAG_DIRTY;
            }
         }

#ifndef IGNORE_DIRTY
         if(
            !(state->map.tiles_flags[(y * TILEMAP_TW) + x] &
               TILEMAP_TILE_FLAG_DIRTY)
         ) {
            continue;
         }
#endif /* !IGNORE_DIRTY */

         /* Sanity checks. */
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
         graphics_blit_tile_at(
            state->map.tileset[tile_id].image,
            0, 0,
            SCREEN_MAP_X + tile_px, SCREEN_MAP_Y + tile_py,
            TILE_W, TILE_H );
      }
   }

   gstate = (struct TOPDOWN_STATE*)memory_unlock( state->engine_state_handle );
}

static void topdown_draw_mobile(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate, struct MOBILE* m
) {
   int16_t
      x_offset = 0,
      y_offset = 0;

   if(
      /* Don't draw inactive mobiles. */
      (MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & m->flags)) ||
      /* Pre-death blinking effect (skip negative even frames). */
      (0 > m->hp && 0 == m->hp % 2)
   ) {
      return;
   }

   if(
      m->coords.x < gstate->screen_scroll_tx ||
      m->coords.y < gstate->screen_scroll_ty ||
      m->coords.x > gstate->screen_scroll_tx + SCREEN_TW ||
      m->coords.y > gstate->screen_scroll_ty + SCREEN_TH
   ) {
      /* Mobile is off-screen. */
      return;
   }

   /* Figure out direction to offset steps in. */
   if( m->coords_prev.x > m->coords.x ) {
      x_offset = SPRITE_W - m->steps_x;
      y_offset = 0;
   } else if(
      m->coords_prev.x < m->coords.x
   ) {
      x_offset = (SPRITE_W - m->steps_x) * -1;
      y_offset = 0;
   } else if(
      m->coords_prev.y > m->coords.y
   ) {
      x_offset = 0;
      y_offset = SPRITE_H - m->steps_y;
   } else if(
      m->coords_prev.y < m->coords.y
   ) {
      x_offset = 0;
      y_offset = (SPRITE_H - m->steps_y) * -1;
   } else {
      x_offset = 0;
      y_offset = 0;
   }

   assert( SPRITE_W > x_offset );
   assert( SPRITE_H > y_offset );

   if( MOBILE_FLAG_ACTIVE == (MOBILE_FLAG_ACTIVE & m->flags) ) {
      /* Blit the mobile's current sprite/frame. */
      graphics_blit_sprite_at(
         m->sprite,
         state->ani_sprite_x,
         m->dir * SPRITE_H,
         SCREEN_MAP_X + (((m->coords.x * SPRITE_W) + x_offset)
            - gstate->screen_scroll_x),
         SCREEN_MAP_Y + (((m->coords.y * SPRITE_H) + y_offset)
            - gstate->screen_scroll_y),
         SPRITE_W, SPRITE_H );
   }
}

int topdown_draw( struct DSEKAI_STATE* state ) {
   struct TOPDOWN_STATE* gstate = NULL;
   int in_char = 0,
      i = 0,
      retval = 1;

   gstate = (struct TOPDOWN_STATE*)memory_lock( state->engine_state_handle );

   /*
   If the screen is scrolling, prioritize that before accepting more
   commands or drawing mobiles.
   */
   if(
      gstate->screen_scroll_x_tgt != gstate->screen_scroll_x ||
      gstate->screen_scroll_y_tgt != gstate->screen_scroll_y 
   ) {
      if( gstate->screen_scroll_x_tgt > gstate->screen_scroll_x ) {
         gstate->screen_scroll_x += TILE_W;
         gstate->screen_scroll_tx = gstate->screen_scroll_x / TILE_W;
      }
      if( gstate->screen_scroll_y_tgt > gstate->screen_scroll_y ) {
         gstate->screen_scroll_y += TILE_H;
         gstate->screen_scroll_ty = gstate->screen_scroll_y / TILE_H;
      }
      if( gstate->screen_scroll_x_tgt < gstate->screen_scroll_x ) {
         gstate->screen_scroll_x -= TILE_W;
         gstate->screen_scroll_tx = gstate->screen_scroll_x / TILE_W;
      }
      if( gstate->screen_scroll_y_tgt < gstate->screen_scroll_y ) {
         gstate->screen_scroll_y -= TILE_H;
         gstate->screen_scroll_ty = gstate->screen_scroll_y / TILE_H;
      }

      state->input_blocked_countdown = INPUT_BLOCK_DELAY;

      assert( 0 <= gstate->screen_scroll_x );
      assert( 0 <= gstate->screen_scroll_tx );

      tilemap_refresh_tiles( &(state->map) );
      topdown_draw_tilemap( state );

      /* Drain input. */
      in_char = input_poll();
      if( INPUT_KEY_QUIT == in_char ) {
         graphics_flip();
         retval = 0;
         goto cleanup;
      }

      if(
         gstate->screen_scroll_y == gstate->screen_scroll_y_tgt &&
         gstate->screen_scroll_x == gstate->screen_scroll_x_tgt
      ) {
         /* Screen scroll finished. */
         state->player.coords_prev.x =
            state->player.coords.x;
         state->player.coords_prev.y =
            state->player.coords.y;
         state->player.steps_x = 0;
         state->player.steps_y = 0;
      }

      /* Keep running. */
      graphics_flip();
      retval = 1;
      goto cleanup;
   }

   topdown_draw_tilemap( state );

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      topdown_draw_mobile( state, gstate, &(state->mobiles[i]) );
   }
   topdown_draw_mobile( state, gstate, &(state->player) );

   animate_frame();

   /* Keep running. */
   graphics_flip();

cleanup:

   gstate = (struct TOPDOWN_STATE*)memory_unlock( state->engine_state_handle );
   return retval;
}

void topdown_focus_player( struct DSEKAI_STATE* state ) {
   struct TOPDOWN_STATE* gstate = NULL;
   int player_x_px = 0,
      player_y_px = 0;

   gstate = (struct TOPDOWN_STATE*)memory_lock( state->engine_state_handle );

   player_x_px = state->player.coords.x * TILE_W;
   player_y_px = state->player.coords.y * TILE_H;

   gstate->screen_scroll_x_tgt = (player_x_px / SCREEN_MAP_W) * SCREEN_MAP_W;
   gstate->screen_scroll_x = (player_x_px / SCREEN_MAP_W) * SCREEN_MAP_W;
   gstate->screen_scroll_tx = gstate->screen_scroll_x / TILE_W;

   gstate->screen_scroll_y_tgt = (player_y_px / SCREEN_MAP_H) * SCREEN_MAP_H;
   gstate->screen_scroll_y = (player_y_px / SCREEN_MAP_H) * SCREEN_MAP_H;
   gstate->screen_scroll_ty = gstate->screen_scroll_y / TILE_H;

   debug_printf( 2, "player x, y: %d, %d (%d, %d)", 
      state->player.coords.x,
      state->player.coords.y,
      player_x_px, player_y_px );

   gstate = (struct TOPDOWN_STATE*)memory_unlock( state->engine_state_handle );
}

int topdown_loop( MEMORY_HANDLE state_handle ) {
   int i = 0;
   uint8_t in_char = 0;
   struct DSEKAI_STATE* state = NULL;
   int retval = 1;
   struct TOPDOWN_STATE* gstate = NULL;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
      retval = 0;
      goto cleanup;
   }

   if( ENGINE_STATE_OPENING == state->engine_state ) {

      assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
      state->engine_state_handle = memory_alloc(
            sizeof( struct TOPDOWN_STATE ), 1 );

      /* Clear the title screen. */
      graphics_draw_block( 0, 0, SCREEN_W, SCREEN_H, GRAPHICS_COLOR_BLACK );

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      topdown_focus_player( state );
      tilemap_refresh_tiles( &(state->map) );
      topdown_draw_tilemap( state );

      window_push(
         WINDOW_ID_STATUS, WINDOW_STATUS_VISIBLE,
         0, (SCREEN_TH * SPRITE_H), STATUS_WINDOW_W, STATUS_WINDOW_H, 0,
         state );

      /* Force reset the weather to start the animation. */
      tilemap_set_weather( &(state->map), state->map.weather );

      state->engine_state = ENGINE_STATE_RUNNING;
   }

   gstate = (struct TOPDOWN_STATE*)memory_lock( state->engine_state_handle );

   graphics_loop_start();

   in_char = 0;

   if( 0 >= window_modal( state ) ) {
      topdown_draw( state );
   }

   window_draw_all( state );

   animate_frame();

   if( state->input_blocked_countdown ) {
      state->input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   #define handle_movement( dir_move ) if( 0 < window_modal( state ) ) { break; } state->player.dir = dir_move; if( !tilemap_collide( &(state->player), dir_move, &(state->map) ) && NULL == mobile_get_facing( &(state->player), state ) ) { mobile_walk_start( &(state->player), dir_move ); }

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
            &(state->player),
            mobile_get_facing( &(state->player), state ),
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
      graphics_loop_end();
      goto cleanup;
   }

   mobile_state_animate( state );
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if( 0 >= window_modal( state ) ) {
         /* Pause scripts if modal window is pending. */
         mobile_execute( &(state->mobiles[i]), state );
      }
      if(
         MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & state->mobiles[i].flags)
      ) {
         /* Skip animating inactive mobiles. */
         continue;
      }
      mobile_animate( &(state->mobiles[i]), &(state->map) );
   }
   mobile_animate( &(state->player), &(state->map) );

   /* Scroll the screen by one if the player goes off-screen. */
   if(
      state->player.coords.x >=
         gstate->screen_scroll_tx + SCREEN_TW
   ) {
      gstate->screen_scroll_x_tgt = gstate->screen_scroll_x + SCREEN_MAP_W;
      debug_printf( 2, "scrolling screen right to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords.y >=
         gstate->screen_scroll_ty + SCREEN_TH
   ) {
      gstate->screen_scroll_y_tgt = gstate->screen_scroll_y + SCREEN_MAP_H;
      debug_printf( 2, "scrolling screen down to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords.x < gstate->screen_scroll_tx
   ) {
      gstate->screen_scroll_x_tgt = gstate->screen_scroll_x - SCREEN_MAP_W;
      debug_printf( 2, "scrolling screen left to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords.y < gstate->screen_scroll_ty
   ) {
      gstate->screen_scroll_y_tgt = gstate->screen_scroll_y - SCREEN_MAP_H;
      debug_printf( 2, "scrolling screen up to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );
   }

   graphics_loop_end();

cleanup:

   if( NULL != gstate ) {
      assert( NULL != state );
      gstate = (struct TOPDOWN_STATE*)memory_unlock(
         state->engine_state_handle );
   }

   if( NULL != state ) {
      if( '\0' != state->warp_to[0] ) {
         /* There's a warp-in map, so unload the current map and load it. */
         state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
         engines_warp_loop( state_handle );
      } else {
         state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
      }
   }

   graphics_flip();

   return retval;
}

