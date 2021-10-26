
#include "dsekai.h"

#ifdef RESOURCE_FILE

#include "tmjson.h"

#endif /* RESOURCE_FILE */

extern const struct TILEMAP gc_map_field;

#define TOPDOWN_STATE_WELCOME 1

int topdown_draw(
   struct DSEKAI_STATE* state, struct TILEMAP* map, struct MOBILE* mobiles,
   struct GRAPHICS_ARGS* args
) {
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

      tilemap_refresh_tiles( map );
      tilemap_draw( map, state );

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
         mobiles[state->player_idx].coords_prev.x =
            mobiles[state->player_idx].coords.x;
         mobiles[state->player_idx].coords_prev.y =
            mobiles[state->player_idx].coords.y;
         mobiles[state->player_idx].steps_x = 0;
         mobiles[state->player_idx].steps_y = 0;
      }

      /* Keep running. */
      graphics_flip( args );
      return 1;
   }

   tilemap_draw( map, state );

   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      if(
         mobiles[i].coords.x < state->screen_scroll_tx ||
         mobiles[i].coords.y < state->screen_scroll_ty ||
         mobiles[i].coords.x > state->screen_scroll_tx + SCREEN_TW ||
         mobiles[i].coords.y > state->screen_scroll_ty + SCREEN_TH
      ) {
         /* Mobile is off-screen. */
         continue;
      }

      /* Figure out direction to offset steps in. */
      if( mobiles[i].coords_prev.x > mobiles[i].coords.x ) {
         x_offset = SPRITE_W - mobiles[i].steps_x;
         y_offset = 0;
      } else if( mobiles[i].coords_prev.x < mobiles[i].coords.x ) {
         x_offset = (SPRITE_W - mobiles[i].steps_x) * -1;
         y_offset = 0;
      } else if( mobiles[i].coords_prev.y > mobiles[i].coords.y ) {
         x_offset = 0;
         y_offset = SPRITE_H - mobiles[i].steps_y;
      } else if( mobiles[i].coords_prev.y < mobiles[i].coords.y ) {
         x_offset = 0;
         y_offset = (SPRITE_H - mobiles[i].steps_y) * -1;
      } else {
         x_offset = 0;
         y_offset = 0;
      }

      assert( SPRITE_W > x_offset );
      assert( SPRITE_H > y_offset );

      mobile_draw(
         &(mobiles[i]), state,
         ((mobiles[i].coords.x * SPRITE_W) + x_offset)
            - state->screen_scroll_x,
         ((mobiles[i].coords.y * SPRITE_H) + y_offset)
            - state->screen_scroll_y );
      
      assert( NULL != &(mobiles[state->player_idx]) );
   }

   /* Keep running. */
   graphics_flip( args );
   return 1;
}

int topdown_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args ) {
   int i = 0;
   uint8_t in_char = 0;
   static int initialized = 0;
   struct DSEKAI_STATE* state = NULL;
   struct TILEMAP* map = NULL;
   struct MOBILE* mobiles = NULL;
   struct ITEM* items = NULL;
   int retval = 1;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
      retval = 0;
      goto cleanup;
   }

   if( !initialized ) {

      assert( (MEMORY_HANDLE)NULL == state->map );
      state->map = memory_alloc( sizeof( struct TILEMAP ), 1 );
      if( (MEMORY_HANDLE)NULL == state->map ) {
         error_printf( "unable to allocate map" );
         retval = 0;
         goto cleanup;
      }
      map = memory_lock( state->map );
      if( NULL == map ) {
         error_printf( "unable to lock map" );
         retval = 0;
         goto cleanup;
      }

      assert( (MEMORY_HANDLE)NULL == state->mobiles );
      state->mobiles = memory_alloc( sizeof( struct MOBILE ), MOBILES_MAX );
      if( (MEMORY_HANDLE)NULL == state->mobiles ) {
         error_printf( "unable to allocate mobiles" );
         retval = 0;
         goto cleanup;
      }
      mobiles = memory_lock( state->mobiles );
      if( NULL == mobiles ) {
         error_printf( "unable to lock mobiles" );
         retval = 0;
         goto cleanup;
      }

      assert( (MEMORY_HANDLE)NULL == state->items );
      state->items = memory_alloc( sizeof( struct ITEM ), ITEMS_MAX );
      if( (MEMORY_HANDLE)NULL == state->items ) {
         error_printf( "unable to allocate items" );
         retval = 0;
         goto cleanup;
      }
      items = (struct ITEM*)memory_lock( state->items );
      if( NULL == items ) {
         error_printf( "unable to lock items" );
         retval = 0;
         goto cleanup;
      }

#ifdef RESOURCE_FILE
#ifdef TILEMAP_FMT_JSON
      tilemap_json_load( "assets/m_field.json", map );
#elif defined TILEMAP_FMT_ASN
      tilemap_asn_load( "assets/m_field.asn", map );
#else
#error "No loader defined!"
#endif
#else
      debug_printf( 3, "gc_map_field: %s", gc_map_field.name );
      memory_copy_ptr( (MEMORY_PTR)map, (MEMORY_PTR)&gc_map_field,
         sizeof( struct TILEMAP ) );

#endif /* RESOURCE_FILE */

      tilemap_refresh_tiles( map );

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      tilemap_draw( map, state );

      /* Spawn mobiles. */
      for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
         if( 0 == memory_strnlen_ptr(
            map->spawns[i].name, TILEMAP_SPAWN_NAME_SZ )
         ) {
            break;
         }
         mobiles[i].hp = 100;
         mobiles[i].mp = 100;
         mobiles[i].coords.x = map->spawns[i].coords.x;
         mobiles[i].coords.y = map->spawns[i].coords.y;
         mobiles[i].coords_prev.x = map->spawns[i].coords.x;
         mobiles[i].coords_prev.y = map->spawns[i].coords.y;
         mobiles[i].steps_x = 0;
         mobiles[i].steps_y = 0;
         mobiles[i].inventory = NULL;
         mobiles[i].script_id = map->spawns[i].script_id;
         mobiles[i].script_pc = 0;
         mobiles[i].script_next_ms = graphics_get_ms();
         mobiles[i].active = 1;
         resource_assign_id( mobiles[i].sprite, map->spawns[i].type );
         state->mobiles_count++;
         if( 0 == strncmp( "player", map->spawns[i].name, 6 ) ) {
            debug_printf( 2, "player is mobile #%d", i );
            state->player_idx = i;
         }
      }

      window_push(
         WINDOW_ID_STATUS, WINDOW_STATUS_VISIBLE,
         0, (SCREEN_TH * 16), STATUS_WINDOW_W, STATUS_WINDOW_H, 0, state );

#ifndef HIDE_WELCOME_DIALOG
#ifndef PLATFORM_PALM
      state->engine_state = TOPDOWN_STATE_WELCOME;
#ifdef RESOURCE_FILE
      window_prefab_dialog(
         WINDOW_ID_WELCOME, 1, "assets/16x16x4/s_pncess.bmp", state, map );
#else
      window_prefab_dialog(
         WINDOW_ID_WELCOME, 1, s_pncess, state, map );
#endif /* RESOURCE_FILE */
#endif /* PLATFORM_PALM */
#endif /* !HIDE_WELCOME_DIALOG */

      mobiles = memory_unlock( state->mobiles );
      map = memory_unlock( state->map );

      initialized = 1;
   }

   graphics_loop_start();

   in_char = 0;

   map = memory_lock( state->map );
   assert( NULL != map );
   mobiles = memory_lock( state->mobiles );
   assert( NULL != mobiles );

   if( 0 >= window_modal( state ) ) {
      topdown_draw( state, map, mobiles, args );
   }

   window_draw_all( state, map->strings, map->strings_count, map->string_szs );

   if( state->input_blocked_countdown ) {
      state->input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   #define handle_movement( dir_move ) if( 0 < window_modal( state ) ) { break; } mobiles[state->player_idx].dir = dir_move; if( !tilemap_collide( &(mobiles[state->player_idx]), dir_move, map ) && NULL == mobile_get_facing( &(mobiles[state->player_idx]), mobiles, state->mobiles_count ) ) { mobile_walk_start( &(mobiles[state->player_idx]), dir_move ); }

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
            &(mobiles[state->player_idx]),
            mobile_get_facing(
               &(mobiles[state->player_idx]), mobiles, state->mobiles_count ),
            map );
      } else {
         /* Try to close any windows that are open. */
         window_pop( WINDOW_ID_WELCOME, state );
         window_pop( WINDOW_ID_SCRIPT_SPEAK, state );
      }
      tilemap_refresh_tiles( map );
      break;

   case INPUT_KEY_QUIT:
      window_pop( WINDOW_ID_STATUS, state );
      retval = 0;
      goto cleanup;
   }

   mobile_state_animate( state );
   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      if( 0 >= window_modal( state ) ) {
         /* Pause scripts if modal window is pending. */
         mobile_execute( &(mobiles[i]), map, state );
      }
      mobile_animate( &(mobiles[i]), map );
   }

   /* Scroll the screen by one if the player goes off-screen. */
   if(
      mobiles[state->player_idx].coords.x >= state->screen_scroll_tx + SCREEN_TW
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x + SCREEN_MAP_W;
      debug_printf( 1, "scrolling screen right to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      mobiles[state->player_idx].coords.y >= state->screen_scroll_y + SCREEN_TH
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y + SCREEN_MAP_H;
      debug_printf( 1, "scrolling screen down to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      mobiles[state->player_idx].coords.x < state->screen_scroll_tx
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x - SCREEN_MAP_W;
      debug_printf( 1, "scrolling screen left to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      mobiles[state->player_idx].coords.y < state->screen_scroll_ty
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y - SCREEN_MAP_H;
      debug_printf( 1, "scrolling screen up to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );
   }
   mobiles = memory_unlock( state->mobiles );
   map = memory_unlock( state->map );
   state = (struct DSEKAI_STATE*)memory_unlock( state_handle );

   graphics_loop_end();

cleanup:

   if( 0 == retval ) {
      /* We're closing, so deinit. */
      if( NULL != mobiles ) {
         mobiles = memory_unlock( state->mobiles );
      }
      if( NULL != map ) {
         map = memory_unlock( state->map );
      }
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
   struct TILEMAP* map = NULL;
   struct MOBILE* mobiles = NULL;
   struct ITEM* items = NULL;

   if( (MEMORY_HANDLE)NULL == state_handle ) {
      return;
   }

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   if( NULL == state ) {
      return;
   }

   if( (MEMORY_HANDLE)NULL != state->mobiles ) {
      mobiles = memory_lock( state->mobiles );
      if( NULL != mobiles ) {
         for( i = 0 ; state->mobiles_count > i ; i++ ) {
            mobile_deinit( &(mobiles[i]) );
         }
         mobiles = memory_unlock( state->mobiles );
      }
      memory_free( state->mobiles );
      state->mobiles = NULL;
   }

   if( (MEMORY_HANDLE)NULL != state->map ) {
      map = memory_lock( state->map );
      if( NULL != map ) {
         tilemap_deinit( map );
         map = memory_unlock( state->map );
      }
      memory_free( state->map );
      state->map = NULL;
   }

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

