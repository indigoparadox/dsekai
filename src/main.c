
#define MAIN_C

#include "dsekai.h"

MEMORY_HANDLE g_state_handle = (MEMORY_HANDLE)NULL;

#ifdef PLATFORM_WIN
#include "winstat.h"
#endif /* PLATFORM_WIN */



/* === Main Class Definition === */

unilayer_main() {

/* === Main Preamble === */

   struct DSEKAI_STATE* state = NULL;
   struct GRAPHICS_ARGS graphics_args;
   int retval = 0;

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

   /*
   state->warp_to[0] = 'f';
   state->warp_to[1] = 'i';
   state->warp_to[2] = 'e';
   state->warp_to[3] = 'l';
   state->warp_to[4] = 'd';
   */

   state = (struct DSEKAI_STATE*)memory_unlock( g_state_handle );



/* === Main Loop === */

#ifdef PLATFORM_WASM

   emscripten_set_main_loop_arg

#else

   while( g_running ) {
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

#endif /* PLATFORM_WASM */


/* === Shutdown === */

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

   window_shutdown();
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

