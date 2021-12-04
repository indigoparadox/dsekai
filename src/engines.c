
#define ENGINES_C
#include "dsekai.h"

#include "tmjson.h"
#include "tmasn.h"
#ifndef RESOURCE_FILE
extern const char gc_map_names[][TILEMAP_NAME_MAX];
extern const struct TILEMAP* gc_map_structs[];
extern const uint8_t gc_map_count;
#endif /* !RESOURCE_FILE */

int16_t engines_warp_loop( MEMORY_HANDLE state_handle ) {
   int16_t retval = 1,
      map_retval = 0,
      i = 0;
   struct DSEKAI_STATE* state = NULL;
#ifdef RESOURCE_FILE
   char map_load_path[RESOURCE_PATH_MAX];
#endif /* RESOURCE_FILE */

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      mobile_deinit( &(state->mobiles[i]) );
   }

   /* Set the player's new position. */
   state->player.coords.x = state->warp_to_x;
   state->player.coords.y = state->warp_to_y;
   state->player.coords_prev.x = state->warp_to_x;
   state->player.coords_prev.y = state->warp_to_y;

   /* Close any open windows (e.g. player state). */
   /* while( state->windows_count > 0 ) {
      window_pop( 0, state );
   } */

   /* TODO: Clean up items held by NPC mobiles and items w/ no owners. */
   /* TODO: Preserve ownerless items in save for this map. */

   tilemap_deinit( &(state->map) );

   /* Clean up existing engine-specific data. */
   if( (MEMORY_HANDLE)NULL != state->engine_state_handle ) {
      memory_free( state->engine_state_handle );
      state->engine_state_handle = NULL;
   }

   graphics_clear_cache();

   animate_stop_all();

#ifdef RESOURCE_FILE
#  ifdef TILEMAP_FMT_JSON
   memory_zero_ptr( (MEMORY_PTR)map_load_path, TILEMAP_NAME_MAX );
   dio_snprintf(
      map_load_path,
      RESOURCE_PATH_MAX,
      ASSETS_PATH "m_%s.json", state->warp_to );
   map_retval = tilemap_json_load( map_load_path, &(state->map) );
#  elif defined TILEMAP_FMT_ASN
   dio_snprintf(
      map_load_path,
      RESOURCE_PATH_MAX,
      ASSETS_PATH "m_%s.asn", state->warp_to );
   map_retval = tilemap_asn_load( map_load_path, &(state->map) );
#  else
#     error "No loader defined!"
#  endif

   if( 0 >= map_retval ) {

   }

#else
   for( i = 0 ; gc_map_count > i ; i++ ) {
      if( 0 == memory_strncmp_ptr(
         gc_map_names[i], state->warp_to, TILEMAP_NAME_MAX
      ) ) {
         /* debug_printf( 3, "gc_map_%s: %s",
            gc_map_names[i], engine_mapize( ENTRY_MAP ).name ); */
         memory_copy_ptr( (MEMORY_PTR)&(state->map),
            (MEMORY_PTR)gc_map_structs[i],
            sizeof( struct TILEMAP ) );
         map_retval = 1;
         break;
      }
   }

#endif /* RESOURCE_FILE */

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
   mobile_spawns( state );

   state->engine_state = ENGINE_STATE_OPENING;

cleanup:
   
   if( NULL != state ) {
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
   }

   return retval;
}

void engines_animate_mobiles( struct DSEKAI_STATE* state ) {
   int8_t i = 0;

   mobile_state_animate( state );
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if(
         /* Pause scripts if modal window is pending. */
         0 >= window_modal( state ) ||
         /* Pause scripts if screen is scrolling. */
         DSEKAI_FLAG_INPUT_BLOCKED == (DSEKAI_FLAG_INPUT_BLOCKED & state->flags)
      ) {
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

}

int16_t engines_handle_movement( int8_t dir_move, struct DSEKAI_STATE* state ) {
   if( 0 < window_modal( state ) ) {
      return 1;
   }

   if(
      DSEKAI_FLAG_INPUT_BLOCKED == (DSEKAI_FLAG_INPUT_BLOCKED & state->flags)
   ) {
      return 1;
   }
   
   state->player.dir = dir_move;

   if(
      !tilemap_collide( &(state->player), dir_move, &(state->map) ) &&
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

      retval = gc_engines_setup[state->map.engine_type]( state );
      if( !retval ) {
         /* Setup failed. */
         goto cleanup;
      }
   }

   graphics_loop_start();

   /* === Drawing Phase === */

   if(
      DSEKAI_FLAG_ANIMATIONS_BG == (DSEKAI_FLAG_ANIMATIONS_BG & state->flags)
   ) {
      /* Draw animations before anything else. */
      animate_frame();
   }

   if( 0 <= state->menu.menu_id ) {
      /* Draw the menu. */
      if( MENU_FLAG_DIRTY == (MENU_FLAG_DIRTY & state->menu.flags) ) {
         /* Repaint the screen in the background. */
         gc_engines_draw[state->map.engine_type]( state );
         tilemap_refresh_tiles( &(state->map) );

         /* Show the new menu state. */
         gc_menu_renderers[state->menu.menu_id]( state );
         state->menu.flags &= ~MENU_FLAG_DIRTY;
      }

   } else {
      /* Draw the engine. */
      if( 0 >= window_modal( state ) ) {
         gc_engines_draw[state->map.engine_type]( state );
      }
   }

   window_draw_all( state );
   
   if(
      DSEKAI_FLAG_ANIMATIONS_BG != (DSEKAI_FLAG_ANIMATIONS_BG & state->flags)
   ) {
      /* Draw animations after anything else. */
      animate_frame();
   }

   /* === Input Phase === */

   in_char = input_poll();
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
      if( !state->map.engine_type ) {
         retval = 0;
      } else {
         menu_open( state );
      }

   } else if( 0 >= window_modal( state ) && 0 != in_char ) {
      retval = gc_engines_input[state->map.engine_type]( in_char, state );

   } else if( INPUT_KEY_OK == in_char ) {
      /* Try to close any windows that are open. */
      debug_printf( 1, "speech window requests closed by user" );
      window_pop( WINDOW_ID_SCRIPT_SPEAK, state );
      tilemap_refresh_tiles( &(state->map) );
   }

   /* === Animation Phase === */

   engines_animate_mobiles( state );

   gc_engines_animate[state->map.engine_type]( state );

   graphics_loop_end();

cleanup:

   if( 0 == retval && NULL != state ) {
      gc_engines_shutdown[state->map.engine_type]( state );
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

