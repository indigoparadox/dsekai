
#define ENGINES_C
#include "dsekai.h"

#include "tmjson.h"
#include "tmasn.h"

int16_t engines_warp_loop( MEMORY_HANDLE state_handle ) {
   int16_t retval = 1,
      map_retval = 0,
      i = 0;
   struct DSEKAI_STATE* state = NULL;
   struct TILEMAP* t = NULL;
   struct ITEM* items = NULL;
   struct GRAPHICS_RECT loading_sz = { 0, 0, 0, 0 };

   /* On-screen loading indicator. */
   graphics_string_sz( "Loading...", 10, 0, &loading_sz );
   graphics_string_at(
      "Loading...", 10, 
      (SCREEN_MAP_W / 2) - (loading_sz.w / 2),
      (SCREEN_MAP_H / 2) - (loading_sz.h / 2),
      GRAPHICS_COLOR_WHITE, 0 );

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   t = (struct TILEMAP*)memory_lock( state->map_handle );

   /* TODO: Preserve mobiles with type 1. */

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      mobile_deinit( &(state->mobiles[i]) );
   }

   memory_zero_ptr(
      (MEMORY_PTR)&(state->mobiles),
      sizeof( struct MOBILE ) * DSEKAI_MOBILES_MAX );

   /* Set the player's new position. */
   state->player.coords.x = state->warp_to_x;
   state->player.coords.y = state->warp_to_y;
   state->player.coords_prev.x = state->warp_to_x;
   state->player.coords_prev.y = state->warp_to_y;

   /* Close any open windows (e.g. player state). */
   /* TODO: Clean up open windows lingering after engine cleanup. */
   /* while( state->windows_count > 0 ) {
      window_pop( 0, state );
   } */

   /* TODO: Clean up items held by NPC mobiles and items w/ no owners. */
   
   /* TODO: Preserve ownerless items in save for this map. */

   /* TODO: Preserve mobile script states in save for this map. */

   tilemap_deinit( t );

   /* Clean up existing engine-specific data. */
   if( (MEMORY_HANDLE)NULL != state->engine_state_handle ) {
      debug_printf( 2, "cleaning up engine-specific state" );
      memory_free( state->engine_state_handle );
      state->engine_state_handle = (MEMORY_HANDLE)NULL;
   }

   animate_stop_all();

   graphics_clear_cache();

   /* Reload player sprite since cache is gone. */
   state->player.sprite_id =
      graphics_cache_load_bitmap( state->player_sprite );

   /* Reset item sprite IDs since cache is gone. */
   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      items[i].sprite_id = graphics_cache_load_bitmap(
         items[i].sprite );
   }
   items = (struct ITEM*)memory_unlock( state->items_handle );

#ifndef NO_ENGINE_EDITOR
   /* Disable editor. */
   memory_zero_ptr(
      (MEMORY_PTR)&(state->editor), sizeof( struct EDITOR_STATE ) );
#endif /* !NO_ENGINE_EDITOR */

   /* Finished unloading old state, so get ready to load new state if needed. */

   state->engine_type = state->engine_type_change;
   state->engine_state = ENGINE_STATE_OPENING;

   if( '\0' == state->warp_to[0] ) {
      goto cleanup;
   }

   map_retval = tilemap_load( state->warp_to, t );

   if( 0 >= map_retval ) {
      /* Handle failure to find map. */
      error_printf(
         "unable to load map (error %d): %s", map_retval, state->warp_to );
      retval = 0;
      goto cleanup;
   }

   memory_zero_ptr( state->warp_to, TILEMAP_NAME_MAX );

   /* Spawn mobiles. */
   memory_zero_ptr( (MEMORY_PTR)(state->mobiles),
      sizeof( struct MOBILE ) * DSEKAI_MOBILES_MAX );
   mobile_spawns( state, t );

   memory_debug_dump();

cleanup:

   if( NULL != t ) {
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }
   
   if( NULL != state ) {
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
   }

   return retval;
}

void engines_animate_mobiles( struct DSEKAI_STATE* state ) {
   int8_t i = 0;
   struct TILEMAP* t = NULL;

   mobile_state_animate( state );
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if(
         /* Pause scripts if modal window is pending. */
         0 >= window_modal( state ) &&
         /* Pause scripts if screen is scrolling. */
         DSEKAI_FLAG_INPUT_BLOCKED !=
            (DSEKAI_FLAG_INPUT_BLOCKED & state->flags) &&
         /* Pause scripts if menu is open. */
         0 > state->menu.menu_id
      ) {
         mobile_execute( &(state->mobiles[i]), state );
      }
      if(
         MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & state->mobiles[i].flags)
      ) {
         /* Skip animating inactive mobiles. */
         continue;
      }
      t = (struct TILEMAP*)memory_lock( state->map_handle );
      if( NULL != t ) {
         mobile_animate( &(state->mobiles[i]), t );
         t = (struct TILEMAP*)memory_unlock( state->map_handle );
      }
   }
   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL != t ) {
      mobile_animate( &(state->player), t );
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   if(
      /* Pause crops if modal window is pending. */
      0 >= window_modal( state ) &&
      /* Pause crops if screen is scrolling. */
      DSEKAI_FLAG_INPUT_BLOCKED != (DSEKAI_FLAG_INPUT_BLOCKED & state->flags) &&
      /* Pause crops if menu is open. */
      0 > state->menu.menu_id
   ) {
      crop_grow_all( state );
   }

}

int16_t engines_handle_movement(
   int8_t dir_move, struct DSEKAI_STATE* state, struct TILEMAP* t
) {

   if( 0 < window_modal( state ) ) {
      return 1;
   }

   if(
      DSEKAI_FLAG_INPUT_BLOCKED == (DSEKAI_FLAG_INPUT_BLOCKED & state->flags)
   ) {
      return 1;
   }
   
   mobile_set_dir( &(state->player), dir_move );

   if(
      !tilemap_collide( &(state->player), dir_move, t ) &&
      NULL == mobile_get_facing( &(state->player), state )
   ) {
      /* No blocking tiles or mobiles. */
      mobile_walk_start( &(state->player), dir_move );
   }

   return dir_move;
}

int16_t engines_loop_iter( MEMORY_HANDLE state_handle ) {
   uint8_t in_char = 0;
   struct DSEKAI_STATE* state = NULL;
   struct TILEMAP* t = NULL;
   int16_t retval = 1;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
      retval = 0;
      goto cleanup;
   }

   if( ENGINE_STATE_OPENING == state->engine_state ) {
      /* Clear the title screen. */
      graphics_draw_block( 0, 0, SCREEN_W, SCREEN_H, GRAPHICS_COLOR_BLACK );

      retval = gc_engines_setup[state->engine_type]( state );
      if( !retval ) {
         /* Setup failed. */
         goto cleanup;
      }

      /* TODO: Optionally skip title screen. */
   }

   graphics_loop_start();

   /* === Drawing Phase === */

   /* Blank out canvas if required. */
   if(
      0 > state->menu.menu_id &&
      DSEKAI_FLAG_BLANK_FRAME == (state->flags & DSEKAI_FLAG_BLANK_FRAME)
   ) {
      graphics_draw_block(
         0, 0, SCREEN_MAP_W, SCREEN_MAP_H, GRAPHICS_COLOR_BLACK );
   }

   /* Draw background animations before anything else. */
   animate_frame( ANIMATE_FLAG_BG );

   if( 0 <= state->menu.menu_id ) {
      /* Draw the menu. */
      if( MENU_FLAG_DIRTY == (MENU_FLAG_DIRTY & state->menu.flags) ) {
         /* Repaint the screen in the background. */
         gc_engines_draw[state->engine_type]( state );
         t = (struct TILEMAP*)memory_lock( state->map_handle );
         if( NULL != t ) {
            tilemap_refresh_tiles( t );
            t = (struct TILEMAP*)memory_unlock( state->map_handle );
         }

         /* Show the new menu state. */
         gc_menu_renderers[state->menu.menu_id]( state );
         state->menu.flags &= ~MENU_FLAG_DIRTY;
      }

   } else {
      /* Draw the engine. */
      if( 0 >= window_modal( state ) ) {
         gc_engines_draw[state->engine_type]( state );
      }
   }

   window_draw_all();
   
   /* Draw foreground animations after anything else. */
   animate_frame( ANIMATE_FLAG_FG );

   /* === Input Phase === */

   in_char = input_poll();
#ifndef NO_TRANSITIONS
   if( 0 < (state->transition & DSEKAI_TRANSITION_MASK_FRAME) ) {
      engines_draw_transition( state );
   } else
#endif /* !NO_TRANSITIONS */

   if( 0 <= state->menu.menu_id && 0 != in_char ) {
      retval = gc_menu_handlers[state->menu.menu_id]( in_char, state );
   
   } else if(
      /* Only open the menu if no modal windows are open and it's not
      *  blocked.
      */
      0 >= window_modal( state ) &&
      INPUT_KEY_QUIT == in_char &&
      DSEKAI_FLAG_MENU_BLOCKED != (DSEKAI_FLAG_MENU_BLOCKED & state->flags)
   ) {
      if( !state->engine_type ) {
         retval = 0;
      } else {
         menu_open( state );
      }

   } else if( 0 >= window_modal( state ) && 0 != in_char ) {
      retval = gc_engines_input[state->engine_type]( in_char, state );

   } else if( INPUT_KEY_OK == in_char ) {
      /* Try to close any windows that are open. */
      debug_printf( 1, "speech window requests closed by user" );
      window_pop( WINDOW_ID_SCRIPT_SPEAK );
      t = (struct TILEMAP*)memory_lock( state->map_handle );
      if( NULL != t ) {
         tilemap_refresh_tiles( t );
      }
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   /* === Animation Phase === */

   engines_animate_mobiles( state );

   gc_engines_animate[state->engine_type]( state );

   graphics_loop_end();

cleanup:

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

   graphics_flip();

   return retval;
}

#ifndef NO_TITLE

void engines_exit_to_title( struct DSEKAI_STATE* state ) {
   state->engine_type_change = 0;
   memory_zero_ptr( (MEMORY_PTR)&(state->player), sizeof( struct MOBILE ) );
   memory_free( state->items_handle );
   state->items_handle =
      memory_alloc( DSEKAI_ITEMS_MAX, sizeof( struct ITEM ) );
   assert( (MEMORY_HANDLE)NULL != state->items_handle );
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
   case DSEKAI_TRANSITION_TYPE_CURTAIN:
      /* TODO: Opening/closing. */
      graphics_draw_block( 0, 0, trans_w, SCREEN_MAP_H, GRAPHICS_COLOR_BLACK );
      graphics_draw_block(
         SCREEN_MAP_W - trans_w, 0,
         trans_w, 
         SCREEN_MAP_H, GRAPHICS_COLOR_BLACK );
      break;

   /* TODO: Other transition types. */
   }

   (state->transition)--;

#endif /* !NO_TRANSITIONS */

}

