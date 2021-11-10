
#define MAIN_C

#include "dsekai.h"

MEMORY_HANDLE g_state_handle = (MEMORY_HANDLE)NULL;

#ifdef PLATFORM_WIN
#include "winstat.h"
#endif /* PLATFORM_WIN */

#include "tmjson.h"
#include "tmasn.h"
#ifndef RESOURCE_FILE
extern const char gc_map_names[][TILEMAP_NAME_MAX];
extern const struct TILEMAP* gc_map_structs[];
extern const uint8_t gc_map_count;
#endif /* !RESOURCE_FILE */



/* === Main Class Definition === */

unilayer_main() {

/* === Main Preamble === */

   struct DSEKAI_STATE* state = NULL;
   struct GRAPHICS_ARGS graphics_args;
   int retval = 0,
      i = 0;
#ifdef RESOURCE_FILE
   char map_load_path[RESOURCE_PATH_MAX];
#endif /* RESOURCE_FILE */

   platform_init( graphics_args, icon_dsekai );

   assert( 0 == TILEMAP_TW % 4 );
   assert( 0 == TILEMAP_TH % 4 );

   /* Setup logging and log some debug items about this build. */

   logging_init();

   debug_printf( 3,
      "dsekai compiled " __DATE__ __TIME__ ", state size is %lu bytes",
      sizeof( struct DSEKAI_STATE ) );

   debug_printf( 3, "git hash: " DSEKAI_GIT_HASH );

   debug_printf( 3, "using " RESOURCES " resources" );

   error_printf( "error test" );

   debug_printf( 3, "animations cache is %lu bytes",
      sizeof( struct ANIMATION ) * ANIMATE_ANIMATIONS_MAX );

   debug_printf( 3, "initial graphics cache is %lu bytes",
      sizeof( struct GRAPHICS_BITMAP ) * GRAPHICS_CACHE_INITIAL_SZ );

   /* Initialize subsystems. */

   if( !graphics_init( &graphics_args ) ) {
      error_printf( "unable to initialize graphics" );
      retval = 1;
      goto exit;
   }

   if( !input_init() ) {
      retval = 1;
      error_printf( "unable to initialize input" );
      goto exit;
   }

   window_init();
   if( !script_init() ) {
      error_printf( "unable to initialize scripts" );
      retval = 1;
      goto exit;
   }

   /* Setup the engine state. */

   g_state_handle = memory_alloc( sizeof( struct DSEKAI_STATE ), 1 );
   if( (MEMORY_HANDLE)NULL == g_state_handle ) {
      error_printf( "unable to allocate state" );
      retval = 1;
      goto exit;
   }

   state = (struct DSEKAI_STATE*)memory_lock( g_state_handle );
   state->engine_state = ENGINE_STATE_OPENING;
   loop_set( title_loop, g_state_handle, &graphics_args );
   state = (struct DSEKAI_STATE*)memory_unlock( g_state_handle );



/* === Main Loop === */

   while( g_running ) {
      state = (struct DSEKAI_STATE*)memory_lock( g_state_handle );
      if( '\0' != state->warp_to[0] ) {
         /* There's a warp-in map, so unload the current map and load it. */

         for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
            mobile_deinit( &(state->mobiles[i]) );
         }

         tilemap_deinit( &(state->map) );

         if( (MEMORY_HANDLE)NULL != state->engine_state_handle ) {
            memory_free( state->engine_state_handle );
            state->engine_state_handle = NULL;
         }

         graphics_clear_cache();

         animation_stop_all();

#ifdef RESOURCE_FILE
#  ifdef TILEMAP_FMT_JSON
         memory_zero_ptr( (MEMORY_PTR)map_load_path, TILEMAP_NAME_MAX );
         dio_snprintf(
            map_load_path,
            RESOURCE_PATH_MAX,
            /* TODO: ASSETS_PATH broken? */
            ASSETS_PATH "assets/m_%s.json", state->warp_to );
         tilemap_json_load( map_load_path, &(state->map) );
#  elif defined TILEMAP_FMT_ASN
         dio_snprintf(
            map_load_path,
            RESOURCE_PATH_MAX,
            /* TODO: ASSETS_PATH broken? */
            ASSETS_PATH "assets/m_%s.asn", state->warp_to );
         tilemap_asn_load( map_load_path, &(state->map) );
#  else
#     error "No loader defined!"
#  endif
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
               break;
            }
         }

         /* TODO: Handle failure to find map. */

#endif /* RESOURCE_FILE */

         memory_zero_ptr( state->warp_to, TILEMAP_NAME_MAX );

         /* Spawn mobiles. */
         memory_zero_ptr( (MEMORY_PTR)(state->mobiles),
            sizeof( struct MOBILE ) * DSEKAI_MOBILES_MAX );
         mobile_spawns( state );

         /* Setup engine. */
         switch( state->map.engine_type ) {
         case ENGINE_TYPE_TOPDOWN:
            debug_printf( 2, "selecting topdown engine" );
            loop_set( topdown_loop, g_state_handle, &graphics_args );
            break;

         case ENGINE_TYPE_POV:
            debug_printf( 2, "selecting pov engine" );
            loop_set( pov_loop, g_state_handle, &graphics_args );
            break;

         default:
            error_printf( "invalid engine requested: %d",
               state->map.engine_type );
            goto shutdown;
         }

         state->engine_state = ENGINE_STATE_OPENING;
      }
      state = (struct DSEKAI_STATE*)memory_unlock( g_state_handle );

      unilayer_loop_iter();

#ifdef USE_SOFT_ASSERT
      if( 0 < g_assert_failed_len ) {
         while( g_running ) {
            if( INPUT_KEY_QUIT == input_poll() ) {
               g_running = 0;
            }
            WinDrawChars( g_assert_failed, g_assert_failed_len, 10, 20 );
         }
      }
#endif /* USE_SOFT_ASSERT */
   }



/* === Shutdown === */

shutdown:

   state = (struct DSEKAI_STATE*)memory_lock( g_state_handle );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
      retval = 1;
      goto exit;
   }
#ifndef PLATFORM_PALM
   while( state->windows_count > 0 ) {
      window_pop( 0, state );
   }
   memory_free( state->windows_handle );
#endif /* !PLATFORM_PALM */
   state = (struct DSEKAI_STATE*)memory_unlock( g_state_handle );

   memory_free( g_state_handle );

   window_shutdown( NULL );
   script_shutdown();
   graphics_shutdown( &graphics_args );

   logging_shutdown();
 
   platform_shutdown();

exit:

#ifdef DISABLE_MAIN_PARMS
   return;
#else
   return retval;
#endif /* !DISABLE_MAIN_PARMS */
}

