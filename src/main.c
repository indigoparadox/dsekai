
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

   platform_init( graphics_args, icon_dsekai );

   assert( 0 == TILEMAP_TW % 4 );
   assert( 0 == TILEMAP_TH % 4 );

   logging_init();

   debug_printf( 3,
      "dsekai compiled " __DATE__ __TIME__ ", state size is %lu bytes",
      sizeof( struct DSEKAI_STATE ) );

   debug_printf( 3, "using " RESOURCES " resources" );

   error_printf( "error test" );

   debug_printf( 3, "map size is %lu bytes", sizeof( struct TILEMAP ) );

   debug_printf( 3, "mobiles size is %lu bytes",
      sizeof( struct MOBILE ) * MOBILES_MAX );

   debug_printf( 3, "items size is %lu bytes",
      sizeof( struct ITEM ) * ITEMS_MAX );

   if( !graphics_init( &graphics_args ) ) {
      error_printf( "unable to initialize graphics" );
#ifdef DISABLE_MAIN_PARMS
      return;
#else
      return 1;
#endif /* DISABLE_MAIN_PARMS */
   }

   if( !input_init() ) {
      error_printf( "unable to initialize input" );
   }

   window_init();

   g_state_handle = memory_alloc( sizeof( struct DSEKAI_STATE ), 1 );
   if( (MEMORY_HANDLE)NULL == g_state_handle ) {
      error_printf( "unable to allocate state" );
#ifdef DISABLE_MAIN_PARMS
      return;
#else
      return 1;
#endif /* DISABLE_MAIN_PARMS */
   }



/* === Main Loop === */

   loop_set( topdown_loop, g_state_handle, &graphics_args );

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



/* === Shutdown === */

   state = (struct DSEKAI_STATE*)memory_lock( g_state_handle );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
#ifdef DISABLE_MAIN_PARMS
      return;
#else
      return 1;
#endif /* DISABLE_MAIN_PARMS */
   }
   while( state->windows_count > 0 ) {
      window_pop( 0, state );
   }
   memory_free( state->windows_handle );
   state = (struct DSEKAI_STATE*)memory_unlock( g_state_handle );

   memory_free( g_state_handle );

   window_shutdown( NULL );
   graphics_shutdown( &graphics_args );

   logging_shutdown();
 
   platform_shutdown();

#ifndef DISABLE_MAIN_PARMS
   return 0;
#endif /* !DISABLE_MAIN_PARMS */
}

