
#define ENGINES_C
#include "dsekai.h"

#include "tmjson.h"
#include "tmasn.h"

#define PROFILE_FIELDS( f ) f( draw_animate ) f( draw_engine ) f( draw_menu ) f( draw_windows ) f( animate_engine ) f( animate_mobiles ) f( script_mobiles ) f( grow_crops )
#include "profiler.h"

void engines_draw_loading_screen() {
   struct GRAPHICS_RECT loading_sz = { 0, 0, 0, 0 };

   /* On-screen loading indicator. */
   graphics_string_sz( "Loading...", 10, 0, &loading_sz );
   graphics_string_at(
      "Loading...", 10, 
      (SCREEN_MAP_W / 2) - (loading_sz.w / 2),
      (SCREEN_MAP_H / 2) - (loading_sz.h / 2),
      GRAPHICS_COLOR_WHITE, 0 );
}

int16_t engines_warp_loop( MEMORY_HANDLE state_handle ) {
   int16_t retval = 1,
      map_retval = 0,
      i = 0;
   struct DSEKAI_STATE* state = NULL;
   RESOURCE_ID sprite_id;

   profiler_print( "ENGINE" );

   debug_printf( 1, "starting warp..." );

   engines_draw_loading_screen();

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   if( !engines_state_lock( state ) ) {
      goto cleanup;
   }

   /* Unload irrelevant mobiles. */
   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      /* mobile_break_if_last( state->mobiles, i ); */
      if( !mobile_is_active( &(state->mobiles[i]) ) ) {
         continue;
      }

      if( 
         /* Not warping to title. */
         '\0' != state->warp_to[0] &&
         /* Don't de-init special mobiles. */
         MOBILE_TYPE_SPECIAL ==
            (MOBILE_TYPE_MASK & state->mobiles[i].flags)
      ) {
         /* Preserve mobiles with type 1. */
         debug_printf( 1, "preserving tilemap %u special mobile %u",
            state->mobiles[i].map_gid, state->mobiles[i].spawner_gid );
         state->mobiles[i].sprite_cache_id = -1;
         continue;
      }

      mobile_deactivate( &(state->mobiles[i]), state );
   }

   /* Unload irrelevant items. */
   for( i = 0 ; state->items_sz > i ; i++ ) {
      if(
         /* Item is inactive. */
         ITEM_FLAG_ACTIVE != (ITEM_FLAG_ACTIVE & state->items[i].flags) ||
         /* Item owned by "special" owner. */
         0 > state->items[i].owner
      ) {
         continue;
      }
      debug_printf( 1, "unloading item %d owned by %d",
         state->items[i].gid, state->items[i].owner );
      memory_zero_ptr( (MEMORY_PTR)&(state->items[i]), sizeof( struct ITEM ) );
   }

   /* Close any open windows (e.g. player state). */
   /* TODO: Clean up open windows lingering after engine cleanup. */
   /* while( state->windows_count > 0 ) {
      window_pop( 0, state );
   } */

   /* Set the player's new position. */
   state->player.coords[1].x = state->warp_to_x;
   state->player.coords[1].y = state->warp_to_y;
   state->player.coords[0].x = state->warp_to_x;
   state->player.coords[0].y = state->warp_to_y;

   /* TODO: Preserve ownerless items in save for this map. */

   /* TODO: Preserve mobile script states in save for this map. */

   tilemap_deinit( state->tilemap );

   /* Clean up existing engine-specific data. */
   if( (MEMORY_HANDLE)NULL != state->engine_state_handle ) {
      debug_printf( 2, "cleaning up engine-specific state" );
      memory_free( state->engine_state_handle );
      state->engine_state_handle = (MEMORY_HANDLE)NULL;
   }

   animate_stop_all();

   graphics_clear_cache();

#ifndef NO_ENGINE_EDITOR
   /* Disable editor. */
   memory_zero_ptr(
      (MEMORY_PTR)&(state->editor), sizeof( struct EDITOR_STATE ) );
#endif /* !NO_ENGINE_EDITOR */

   /* Finished unloading old state, so get ready to load new state if needed. 
    */

   state->engine_type = state->engine_type_change;
   state->engine_state = ENGINE_STATE_OPENING;

   if( '\0' == state->warp_to[0] ) {
      goto cleanup;
   }

   map_retval = tilemap_load( state->warp_to, state->tilemap );

   if( 0 >= map_retval ) {
      /* Handle failure to find map. */
      error_printf(
         "unable to load map (error %d): %s", map_retval, state->warp_to );
      retval = 0;
      goto cleanup;
   }

   memory_zero_ptr( state->warp_to, TILEMAP_NAME_MAX );

   /* Spawn mobiles. */
   mobile_spawns( state );

   /* Refresh volatile stuff, like images. */

   /* Reload player sprite since cache is gone. */
   resource_id_from_name( &sprite_id, state->player_sprite_name,
      RESOURCE_EXT_GRAPHICS );
   debug_printf( 2, "player sprite ID: " RESOURCE_ID_FMT, sprite_id );
   if( resource_id_valid( sprite_id ) ) {
      state->player.sprite_cache_id = graphics_cache_load_bitmap(
         sprite_id, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   } else {
      error_printf( "invalid player sprite ID!" );
   }

   /* Reload other mobile sprites if they're on this tilemap. */
   debug_printf( 1, "resetting mobile sprite IDs..." );
   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      if(
         !mobile_is_active( &(state->mobiles[i]) ) ||
         state->mobiles[i].map_gid != state->tilemap->gid
      ) {
         continue;
      }
      resource_id_from_name( &sprite_id, state->mobiles[i].sprite_name,
         RESOURCE_EXT_GRAPHICS );
      state->mobiles[i].sprite_cache_id = graphics_cache_load_bitmap(
         sprite_id, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   }

   /* Reset item sprite IDs since cache is gone. */
   debug_printf( 1, "resetting item sprite IDs..." );
   for( i = 0 ; state->items_sz > i ; i++ ) {
      resource_id_from_name( &sprite_id, state->items[i].sprite_name,
         RESOURCE_EXT_GRAPHICS );
      state->items[i].sprite_cache_id = graphics_cache_load_bitmap(
         sprite_id, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   }

   memory_debug_dump();

   debug_printf( 1, "warp complete!" );

cleanup:

   profiler_init();

   if( NULL != state ) {
      engines_state_unlock( state );
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
   }

   return retval;
}

void engines_animate_mobiles( struct DSEKAI_STATE* state ) {
   int16_t i = 0;

   /* TODO: Implement ratio to only animate/execute every X calls, in order
    *       to improve input handling responsiveness.
    */

   mobile_state_animate( state );
   profiler_set();
   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      /* mobile_break_if_last( state->mobiles, i ); */
      if( !mobile_is_active( &(state->mobiles[i]) ) ) {
         continue;
      }
      mobile_execute( &(state->mobiles[i]), state );
   }
   profiler_incr( script_mobiles );

   profiler_set();
   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      /* mobile_break_if_last( state->mobiles, i ); */
      if( !mobile_is_active( &(state->mobiles[i]) ) ) {
         continue;
      }
      mobile_animate( &(state->mobiles[i]), state );
   }
   mobile_animate( &(state->player), state );
   profiler_incr( animate_mobiles );

   profiler_set();
   crop_grow_all( state );
   profiler_incr( grow_crops );

}

int8_t engines_input_movement(
   struct MOBILE* mover, int8_t dir_move,
   struct DSEKAI_STATE* state
) {
   if( !engines_state_lock( state ) ) {
      dir_move = -1;
      goto cleanup;     
   }

   if(
      ( 0 < window_modal() ) ||
      (DSEKAI_FLAG_INPUT_BLOCKED ==
         (DSEKAI_FLAG_INPUT_BLOCKED & state->flags))
   ) {
      /* System input is blocked right now. */
      dir_move = -1;
      goto cleanup;
   }

   /* Face requested dir even if we're blocked. */
   mobile_set_dir( &(state->player), dir_move );

   if(
      0 > pathfind_test_dir(
         mover->coords[1].x, mover->coords[1].y, dir_move, state, state->tilemap )
   ) {
      /* Mobile is blocked. */
      dir_move = -1;
      goto cleanup;
   }

cleanup:

   return dir_move;
}

#ifdef PLATFORM_WASM
void engines_loop_iter( void* state_handle_p ) {
   MEMORY_HANDLE state_handle = (MEMORY_HANDLE)state_handle_p;
#else
int16_t engines_loop_iter( MEMORY_HANDLE state_handle ) {
#endif /* PLATFORM_WASM */
   INPUT_VAL in_char = 0;
   struct DSEKAI_STATE* state = NULL;
   int16_t retval = 1,
      click_x = 0,
      click_y = 0;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
      retval = 0;
      goto cleanup;
   }

   if( !engines_state_lock( state ) ) {
      goto cleanup;
   }

   if( ENGINE_STATE_OPENING == state->engine_state ) {
      /* Clear the title screen. */
      graphics_lock();
      graphics_clear_screen();
      graphics_release();

      retval = gc_engines_setup[state->engine_type]( state );
      if( !retval ) {
         /* Setup failed. */
         goto cleanup;
      }

      /* TODO: Optionally skip title screen. */
   }

   graphics_loop_start();

   /* === Drawing Phase === */

   profiler_set();

   graphics_lock();

   /* Blank out canvas if required. */
   if(
      0 > state->menu.menu_id &&
      DSEKAI_FLAG_BLANK_FRAME == (state->flags & DSEKAI_FLAG_BLANK_FRAME)
   ) {
      graphics_clear_block(
         0, 0, SCREEN_MAP_W, SCREEN_MAP_H );
   }

   /* Draw background animations before anything else. */
   animate_frame( ANIMATE_FLAG_BG );

   profiler_incr( draw_animate );
   profiler_set();

   if( 0 <= state->menu.menu_id ) {
      /* Draw the menu. */
      if( MENU_FLAG_DIRTY == (MENU_FLAG_DIRTY & state->menu.flags) ) {
#ifndef NO_DRAW_ENGINE_BEHIND_MENU
         /* Repaint the screen in the background. */
         gc_engines_draw[state->engine_type]( state );
         tilemap_refresh_tiles( state->tilemap );
#endif /* !NO_DRAW_ENGINE_BEHIND_MENU */

         /* Show the new menu state. */
         gc_menu_renderers[state->menu.menu_id]( state );
         state->menu.flags &= ~MENU_FLAG_DIRTY;
      }

      profiler_incr( draw_menu );
      profiler_set();

   } else {
      /* Draw the engine. */
      if( 0 == window_modal() ) {
         gc_engines_draw[state->engine_type]( state );
      }

      profiler_incr( draw_engine );
      profiler_set();
   }

   /* TODO: Pause weather animations before drawing dialogs. */

   window_draw_all();

   profiler_incr( draw_windows );
   profiler_set();
   
   /* Draw foreground animations after anything else. */
   animate_frame( ANIMATE_FLAG_FG );

   profiler_incr( draw_animate );

   /* === Input Phase === */

   in_char = input_poll( &click_x, &click_y );
#ifndef NO_TRANSITIONS
   if( 0 < (state->transition & DSEKAI_TRANSITION_MASK_FRAME) ) {
      engines_draw_transition( state );
   } else
#endif /* !NO_TRANSITIONS */

   if( INPUT_CLICK == in_char ) {
      debug_printf( 1, "click x: %d, y: %d", click_x, click_y );
   }

   if( g_input_key_quit == in_char ) {
      retval = 0;

   } else if( 0 <= state->menu.menu_id && 0 != in_char ) {
      retval = gc_menu_handlers[state->menu.menu_id](
         in_char, click_x, click_y, state );
   
   } else if(
      /* Only open the menu if no modal windows are open and it's not
      *  blocked.
      */
      0 == window_modal() &&
      g_input_key_menu == in_char &&
      DSEKAI_FLAG_MENU_BLOCKED != (DSEKAI_FLAG_MENU_BLOCKED & state->flags)
   ) {
      if( !state->engine_type ) {
         retval = 0;
      } else {
         menu_open( state );
      }

   } else if( 0 == window_modal() && 0 != in_char ) {
      retval = gc_engines_input[state->engine_type](
         in_char, click_x, click_y, state );

   } else if( g_input_key_ok == in_char ) {
      /* Try to close any windows that are open. */
      debug_printf( 1, "speech window requests closed by user" );
      window_pop( WINDOW_ID_SCRIPT_SPEAK );
      tilemap_refresh_tiles( state->tilemap );
   }

   /* === Animation Phase === */

   if( engines_active( state ) ) {
      engines_animate_mobiles( state );
   }
   profiler_set();
   gc_engines_animate[state->engine_type]( state );
   profiler_incr( animate_engine );

cleanup:

   engines_state_unlock( state );

   if( NULL != state && (
      0 == retval ||
      '\0' != state->warp_to[0] ||
      state->engine_type != state->engine_type_change
   ) ) {
      gc_engines_shutdown[state->engine_type]( state );
   }

   if( NULL != state ) {
      if(
         '\0' != state->warp_to[0] ||
         state->engine_type != state->engine_type_change
      ) {
         /* There's a warp-in map, so unload the current map and load it. */
         state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
         engines_warp_loop( state_handle );
      } else {
         state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
      }
   }

   graphics_release();

   graphics_loop_end();

#ifndef PLATFORM_WASM
   return retval;
#endif /* !PLATFORM_WASM */
}

#ifndef NO_TITLE

void engines_exit_to_title( struct DSEKAI_STATE* state ) {
   state->engine_type_change = 0;
   memory_zero_ptr( (MEMORY_PTR)&(state->player), sizeof( struct MOBILE ) );

   /* TODO: Start small and resize as needed. */
   memory_free( state->items_handle );
   state->items_handle =
      memory_alloc( DSEKAI_ITEMS_MAX, sizeof( struct ITEM ) );
   assert( (MEMORY_HANDLE)NULL != state->items_handle );

   /* TODO: Start small and resize as needed. */
   memory_free( state->mobiles_handle );
   state->mobiles_handle =
      memory_alloc( DSEKAI_MOBILES_MAX, sizeof( struct MOBILE ) );

   /* TODO: Start small and resize as needed. */
   memory_free( state->crops_handle );
   state->crops_handle =
      memory_alloc( DSEKAI_MOBILES_MAX, sizeof( struct MOBILE ) );

   /* TODO: New tilemap? */
}

#endif /* !NO_TITLE */

void engines_set_transition(
   struct DSEKAI_STATE* state, uint8_t trans_type, uint8_t trans_open
) {

#ifndef NO_TRANSITIONS

   /* Assume the transition increments in tiles from the center of the screen,
    * add a frame for setup/cleanup.
    */
   state->transition =
      (((SCREEN_MAP_W / TILE_W) / 2 ) & DSEKAI_TRANSITION_MASK_FRAME) + 2;

   state->transition |= (trans_open & DSEKAI_TRANSITION_DIR_OPEN);
   state->transition |= (trans_type & DSEKAI_TRANSITION_MASK_TYPE);

#endif /* !NO_TRANSITIONS */

}

void engines_draw_transition( struct DSEKAI_STATE* state ) {

#ifndef NO_TRANSITIONS

   uint8_t trans_type = (state->transition & DSEKAI_TRANSITION_MASK_TYPE);
   uint8_t trans_frame = (state->transition & DSEKAI_TRANSITION_MASK_FRAME);
   uint8_t trans_w = TILE_W * (trans_frame - 1);

   switch( trans_type ) {
   case DSEKAI_TRANSITION_TYPE_ZOOM:
      debug_printf( 3, "trans frame: %d", (trans_frame / 2) );
      graphics_set_screen_zoom( (trans_frame / 2) );
      break;
   case DSEKAI_TRANSITION_TYPE_CURTAIN:
      /* TODO: Draw block or clear block? Needs testing. */
      graphics_draw_block( 0, 0, trans_w, SCREEN_MAP_H, GRAPHICS_COLOR_BLACK );
      graphics_draw_block(
         SCREEN_MAP_W - trans_w, 0,
         trans_w, 
         SCREEN_MAP_H, GRAPHICS_COLOR_BLACK );
      break;

   /* TODO: Other transition types. */
   }

   (state->transition)--;
   if( 0 == (state->transition & DSEKAI_TRANSITION_MASK_FRAME) ) {
      state->transition = 0;
   }

#endif /* !NO_TRANSITIONS */

}

uint8_t engines_state_lock( struct DSEKAI_STATE* state ) {
   /* TODO: Safety checks. */

   if( NULL == state ) {
      return 0;
   }

   if( NULL == state->tilemap ) {
      state->tilemap = (struct TILEMAP*)memory_lock( state->map_handle );
   }

   if( NULL == state->mobiles ) {
      state->mobiles = (struct MOBILE*)memory_lock( state->mobiles_handle );
   }

   if( NULL == state->crops ) {
      state->crops = (struct CROP_PLOT*)memory_lock( state->crops_handle );
   }

   if( NULL == state->items ) {
      state->items = (struct ITEM*)memory_lock( state->items_handle );
   }

   return 1;
}

uint8_t engines_state_unlock( struct DSEKAI_STATE* state ) {
   /* TODO: Safety checks. */

   if( NULL == state ) {
      return 0;
   }

   if( NULL != state->tilemap ) {
      state->tilemap = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   if( NULL != state->mobiles ) {
      state->mobiles = (struct MOBILE*)memory_unlock( state->mobiles_handle );
   }

   if( NULL != state->crops ) {
      state->crops = (struct CROP_PLOT*)memory_unlock( state->crops_handle );
   }

   if( NULL != state->items ) {
      state->items = (struct ITEM*)memory_unlock( state->items_handle );
   }

   return 1;
}

