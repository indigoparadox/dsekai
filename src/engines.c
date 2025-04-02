
#define ENGINES_C
#include "dsekai.h"

#include "tmjson.h"
#include "tmasn.h"

#define PROFILE_FIELDS( f ) f( draw_animate ) f( draw_engine ) f( draw_menu ) f( draw_windows ) f( animate_engine ) f( animate_mobiles ) f( script_mobiles ) f( grow_crops )
#include "profiler.h"

void engines_draw_loading_screen() {
   size_t str_w = 0;
   size_t str_h = 0;

   /* On-screen loading indicator. */
   retroflat_string_sz( NULL, "Loading...", 10, NULL, &str_w, &str_h, 0 );
   retroflat_string(
      NULL, RETROFLAT_COLOR_WHITE, "Loading...", 10, NULL,
      (SCREEN_MAP_W() / 2) - (str_w / 2),
      (SCREEN_MAP_H() / 2) - (str_h / 2), 0 );
}

int16_t engines_warp_loop( MAUG_MHANDLE state_handle ) {
   int16_t retval = 1,
      map_retval = 0,
      i = 0;
   struct DSEKAI_STATE* state = NULL;

   profiler_print( "ENGINE" );

   debug_printf( 1, "starting warp..." );

   engines_draw_loading_screen();

   maug_mlock( state_handle, state );

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
      maug_mzero( &(state->items[i]), sizeof( struct ITEM ) );
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
   if( (MAUG_MHANDLE)NULL != state->engine_state_handle ) {
      debug_printf( 2, "cleaning up engine-specific state" );
      maug_mfree( state->engine_state_handle );
      state->engine_state_handle = (MAUG_MHANDLE)NULL;
   }

   retroani_stop_all( &(state->animations[0]), DSEKAI_ANIMATIONS_MAX );

   retrogxc_clear_cache();

#ifndef NO_ENGINE_EDITOR
   /* Disable editor. */
   maug_mzero( &(state->editor), sizeof( struct EDITOR_STATE ) );
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

   maug_mzero( state->warp_to, TILEMAP_NAME_MAX );

   /* Spawn mobiles. */
   mobile_spawns( state );

   /* Refresh volatile stuff, like images. */

   /* Reload player sprite since cache is gone. */
   state->player.sprite_cache_id =
      retrogxc_load_bitmap( state->player_sprite_name, 0 );
   maug_cleanup_if_lt( state->player.sprite_cache_id, 0, "%d", -1 );

   /* Reload other mobile sprites if they're on this tilemap. */
   debug_printf( 1, "resetting mobile sprite IDs..." );
   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      if(
         !mobile_is_active( &(state->mobiles[i]) ) ||
         state->mobiles[i].map_gid != state->tilemap->gid
      ) {
         continue;
      }
      state->mobiles[i].sprite_cache_id = retrogxc_load_bitmap(
         state->mobiles[i].sprite_name, 0 );
      maug_cleanup_if_lt( state->mobiles[i].sprite_cache_id, 0, "%d", -1 );
   }

   /* Reset item sprite IDs since cache is gone. */
   debug_printf( 1, "resetting item sprite IDs..." );
   for( i = 0 ; state->items_sz > i ; i++ ) {
      state->items[i].sprite_cache_id = retrogxc_load_bitmap(
         state->items[i].sprite_name, 0 );
      maug_cleanup_if_lt( state->items[i].sprite_cache_id, 0, "%d", -1 );
   }

   /* memory_debug_dump(); */

   debug_printf( 1, "warp complete!" );

cleanup:

   profiler_init();

   if( NULL != state ) {
      engines_state_unlock( state );
      maug_munlock( state_handle, state );
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
      /* mobile_execute( &(state->mobiles[i]), state ); */
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

#if 0
   if(
      ( 0 < window_modal() ) ||
      (DSEKAI_FLAG_INPUT_BLOCKED ==
         (DSEKAI_FLAG_INPUT_BLOCKED & state->flags))
   ) {
      /* System input is blocked right now. */
      dir_move = -1;
      goto cleanup;
   }
#endif

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

void engines_loop_iter( MAUG_MHANDLE state_handle ) {
   int16_t in_char = 0;
   struct DSEKAI_STATE* state = NULL;
   int16_t retval = 1;
   struct RETROFLAT_INPUT input;

   /* TODO: Zero input struct? */

   maug_mlock( state_handle, state );
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
      retroflat_draw_lock( NULL );
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 0, 0,
         retroflat_screen_w(), retroflat_screen_h(),
         RETROFLAT_FLAGS_FILL );
      retroflat_draw_release( NULL );

      retval = gc_engines_setup[state->engine_type]( state );
      if( !retval ) {
         /* Setup failed. */
         goto cleanup;
      }

      /* TODO: Optionally skip title screen. */
   }

   /* === Drawing Phase === */

   profiler_set();

   retroflat_draw_lock( NULL );

   /* Blank out canvas if required. */
   if(
      0 > state->menu.menu_id &&
      DSEKAI_FLAG_BLANK_FRAME == (state->flags & DSEKAI_FLAG_BLANK_FRAME)
   ) {
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 0, 0,
         SCREEN_MAP_W(), SCREEN_MAP_H(), RETROFLAT_FLAGS_FILL );
   }

   /* Draw background animations before anything else. */
   retroani_frame( &(state->animations[0]), DSEKAI_ANIMATIONS_MAX,
      ANIMATE_FLAG_BG );

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
         /* gc_menu_renderers[state->menu.menu_id]( state ); */
         state->menu.flags &= ~MENU_FLAG_DIRTY;
      }

      profiler_incr( draw_menu );
      profiler_set();

   } else {
      /* Draw the engine. */
      /* if( 0 == window_modal() ) { */
         gc_engines_draw[state->engine_type]( state );
      /* } */

      profiler_incr( draw_engine );
      profiler_set();
   }

   /* TODO: Pause weather animations before drawing dialogs. */

   /* window_draw_all(); */

   profiler_incr( draw_windows );
   profiler_set();
   
   /* Draw foreground animations after anything else. */
   retroani_frame( &(state->animations[0]), DSEKAI_ANIMATIONS_MAX,
      ANIMATE_FLAG_FG );

   profiler_incr( draw_animate );

   /* === Input Phase === */

   in_char = retroflat_poll_input( &input );
#ifndef NO_TRANSITIONS
   if( 0 < (state->transition & DSEKAI_TRANSITION_MASK_FRAME) ) {
      engines_draw_transition( state );
   } else
#endif /* !NO_TRANSITIONS */

   /*
   TODO: Retroflat mouse input.
   if( INPUT_CLICK == in_char ) {
      debug_printf( 1, "click x: %d, y: %d", click_x, click_y );
   }
   */

   /* TODO: Configurable keys. */
   if( RETROFLAT_KEY_ESC == in_char ) {
      retval = 0;

#if 0
   } else if( 0 <= state->menu.menu_id && 0 != in_char ) {
      /* retval = gc_menu_handlers[state->menu.menu_id](
         in_char, &input, state ); */
   
   } else if(
      /* Only open the menu if no modal windows are open and it's not
      *  blocked.
      */
      0 == window_modal() &&
      RETROFLAT_KEY_TAB == in_char &&
      DSEKAI_FLAG_MENU_BLOCKED != (DSEKAI_FLAG_MENU_BLOCKED & state->flags)
   ) {
      if( !state->engine_type ) {
         retval = 0;
      } else {
         /* menu_open( state ); */
      }

   } else if( 0 == window_modal() && 0 != in_char ) {
      retval = gc_engines_input[state->engine_type](
         in_char, &input, state );

   } else if( RETROFLAT_KEY_ENTER == in_char ) {
      /* Try to close any windows that are open. */
      debug_printf( 1, "speech window requests closed by user" );
      window_pop( WINDOW_ID_SCRIPT_SPEAK );
      tilemap_refresh_tiles( state->tilemap );
#endif
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
         maug_munlock( state_handle, state );
         engines_warp_loop( state_handle );
      } else {
         maug_munlock( state_handle, state );
      }
   }

   retroflat_draw_release( NULL );

   if( retval ) {
      retroflat_quit( retval );
   }
}

#ifndef NO_TITLE

void engines_exit_to_title( struct DSEKAI_STATE* state ) {
   state->engine_type_change = 0;
   maug_mzero( &(state->player), sizeof( struct MOBILE ) );

   /* TODO: Start small and resize as needed. */
   maug_mfree( state->items_handle );
   state->items_handle =
      maug_malloc( DSEKAI_ITEMS_MAX, sizeof( struct ITEM ) );
   assert( (MAUG_MHANDLE)NULL != state->items_handle );

   /* TODO: Start small and resize as needed. */
   maug_mfree( state->mobiles_handle );
   state->mobiles_handle =
      maug_malloc( DSEKAI_MOBILES_MAX, sizeof( struct MOBILE ) );

   /* TODO: Start small and resize as needed. */
   maug_mfree( state->crops_handle );
   state->crops_handle =
      maug_malloc( DSEKAI_MOBILES_MAX, sizeof( struct MOBILE ) );

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
      (((SCREEN_MAP_W() / TILE_W) / 2 ) & DSEKAI_TRANSITION_MASK_FRAME) + 2;

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
      /*
      TODO: Zoom in retroflat.
      debug_printf( 3, "trans frame: %d", (trans_frame / 2) );
      graphics_set_screen_zoom( (trans_frame / 2) );
      */
      break;
   case DSEKAI_TRANSITION_TYPE_CURTAIN:
      /* TODO: Draw block or clear block? Needs testing. */
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 0, 0,
         trans_w, SCREEN_MAP_W(),
         RETROFLAT_FLAGS_FILL );
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 0, 0,
         trans_w - SCREEN_MAP_W(), trans_w,
         RETROFLAT_FLAGS_FILL );
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
      maug_mlock( state->map_handle, state->tilemap );
   }

   if( NULL == state->mobiles ) {
      maug_mlock( state->mobiles_handle, state->mobiles );
   }

   if( NULL == state->crops ) {
      maug_mlock( state->crops_handle, state->crops );
   }

   if( NULL == state->items ) {
      maug_mlock( state->items_handle, state->items );
   }

   return 1;
}

uint8_t engines_state_unlock( struct DSEKAI_STATE* state ) {
   /* TODO: Safety checks. */

   if( NULL == state ) {
      return 0;
   }

   if( NULL != state->tilemap ) {
      maug_munlock( state->map_handle, state->tilemap );
   }

   if( NULL != state->mobiles ) {
      maug_munlock( state->mobiles_handle, state->mobiles );
   }

   if( NULL != state->crops ) {
      maug_munlock( state->crops_handle, state->crops );
   }

   if( NULL != state->items ) {
      maug_munlock( state->items_handle, state->items );
   }

   return 1;
}

