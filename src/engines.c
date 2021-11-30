
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
   while( state->windows_count > 0 ) {
      window_pop( 0, state );
   }

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

   /* Setup engine. */
   switch( state->map.engine_type ) {
   case ENGINE_TYPE_TOPDOWN:
      debug_printf( 2, "selecting topdown engine" );
      unilayer_loop_set( topdown_loop, state_handle );
      break;

   case ENGINE_TYPE_POV:
      debug_printf( 2, "selecting pov engine" );
      unilayer_loop_set( pov_loop, state_handle );
      break;

   default:
      error_printf( "invalid engine requested: %d",
         state->map.engine_type );
      retval = 0;
      goto cleanup;
   }

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

}

int16_t engines_handle_movement( int8_t dir_move, struct DSEKAI_STATE* state ) {
   if( 0 < window_modal( state ) ) {
      return -1;
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

