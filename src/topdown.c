
#include "dsekai.h"

#ifdef RESOURCE_FILE

#include "tmjson.h"

#endif /* RESOURCE_FILE */

extern const struct TILEMAP gc_map_field;

#define TOPDOWN_STATE_WELCOME 1

#define gc_stringize( map ) #map

#define engine_mapize_internal( map ) gc_map_ ## map

#define engine_mapize( map ) engine_mapize_internal( map )

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
      tilemap_draw( &(state->map), state );

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

   tilemap_draw( &(state->map), state );

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

      mobile_draw(
         &(state->mobiles[i]), state,
         ((state->mobiles[i].coords.x * SPRITE_W) + x_offset)
            - state->screen_scroll_x,
         ((state->mobiles[i].coords.y * SPRITE_H) + y_offset)
            - state->screen_scroll_y );
   }

   /* Keep running. */
   graphics_flip( args );
   return 1;
}

void topdown_spawns( struct DSEKAI_STATE* state ) {
   int8_t i = 0;
  
   /* TODO: Generalize this for all engines, move to engines.c. */
   /* TODO: Allow spawners to spawn multiples. */
   /* TODO: Separate mobiles[i] from spawns[i]. */

   assert( DSEKAI_MOBILES_MAX == TILEMAP_SPAWNS_MAX );

   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      if( 0 == memory_strnlen_ptr(
         state->map.spawns[i].name, TILEMAP_SPAWN_NAME_SZ )
      ) {
         break;
      }
      state->mobiles[i].hp = 100;
      state->mobiles[i].mp = 100;
      state->mobiles[i].coords.x = state->map.spawns[i].coords.x;
      state->mobiles[i].coords.y = state->map.spawns[i].coords.y;
      state->mobiles[i].coords_prev.x = state->map.spawns[i].coords.x;
      state->mobiles[i].coords_prev.y = state->map.spawns[i].coords.y;
      state->mobiles[i].steps_x = 0;
      state->mobiles[i].steps_y = 0;
      state->mobiles[i].inventory = NULL;
      state->mobiles[i].script_id = state->map.spawns[i].script_id;
      state->mobiles[i].script_pc = 0;
      state->mobiles[i].script_next_ms = graphics_get_ms();
      state->mobiles[i].active = 1;
      resource_assign_id(
         state->mobiles[i].sprite, state->map.spawns[i].type );
      if( 0 == memory_strncmp_ptr( "player", state->map.spawns[i].name, 6 ) ) {
         debug_printf( 2, "player is mobile #%d", i );
         state->player_idx = i;
      }
   }

}

int topdown_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args ) {
   int i = 0;
   uint8_t in_char = 0;
   static int initialized = 0;
   struct DSEKAI_STATE* state = NULL;
   int retval = 1;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
      retval = 0;
      goto cleanup;
   }

   if( !initialized ) {

#ifdef RESOURCE_FILE
#  ifdef TILEMAP_FMT_JSON
      tilemap_json_load( "assets/m_" gc_stringize( ENTRY_MAP ) ".json", &(state->map) );
#  elif defined TILEMAP_FMT_ASN
      tilemap_asn_load( "assets/m_" gc_stringize( ENTRY_MAP ) ".asn", &(state->map) );
#  else
#     error "No loader defined!"
#  endif
#else
      debug_printf( 3, "gc_map_" gc_stringize( ENTRY_MAP ) ": %s",
         engine_mapize( ENTRY_MAP ).name );
      memory_copy_ptr( (MEMORY_PTR)&(state->map),
         (MEMORY_PTR)&engine_mapize( ENTRY_MAP ),
         sizeof( struct TILEMAP ) );

#endif /* RESOURCE_FILE */

      tilemap_refresh_tiles( &(state->map) );

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      tilemap_draw( &(state->map), state );

      /* Spawn mobiles. */
      topdown_spawns( state );

      window_push(
         WINDOW_ID_STATUS, WINDOW_STATUS_VISIBLE,
         0, (SCREEN_TH * 16), STATUS_WINDOW_W, STATUS_WINDOW_H, 0, state );

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

      initialized = 1;
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

cleanup:

   if( 0 == retval ) {
      /* We're closing, so deinit. */
      if( NULL != state ) {
         state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
      }
      topdown_deinit( state_handle );
   }

   graphics_flip( args );
   return retval;
}

void topdown_deinit( MEMORY_HANDLE state_handle ) {
   int i = 0;
   struct DSEKAI_STATE* state = NULL;

   if( (MEMORY_HANDLE)NULL == state_handle ) {
      return;
   }

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   if( NULL == state ) {
      return;
   }

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      mobile_deinit( &(state->mobiles[i]) );
   }

   tilemap_deinit( &(state->map) );

   #if 0
   if( (MEMORY_HANDLE)NULL != state->items ) {
      items = memory_lock( state->items );
      if( NULL != items ) {
         /* items_deinit( items ); */
         items = memory_unlock( state->items );
      }
      memory_free( state->items );
      state->items = NULL;
   }
   #endif

   state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
}

