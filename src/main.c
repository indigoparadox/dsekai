
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
   int16_t retval = 0;

   platform_init( icon_dsekai );

   assert( 0 == TILEMAP_TW % 4 );
   assert( 0 == TILEMAP_TH % 4 );

   /* Setup logging and log some debug items about this build. */

   logging_init();

   debug_printf( 3, "dsekai compiled " __DATE__ __TIME__ );
   debug_printf( 3, "git hash: " DSEKAI_GIT_HASH );
   debug_printf( 3, "using " RESOURCES " resources" );

   error_printf( "error test" );

   size_printf( 3, "state", sizeof( struct DSEKAI_STATE ) );
   assert( sizeof( struct DSEKAI_STATE ) < 16384 );
   size_printf( 3, "tilemap", sizeof( struct TILEMAP ) );
   assert( sizeof( struct TILEMAP ) < 16384 );
   
   size_multi_printf(
      3, "mobile", sizeof( struct MOBILE ), DSEKAI_MOBILES_MAX );
   size_multi_printf( 3, "item", sizeof( struct ITEM ), DSEKAI_ITEMS_MAX );
   size_multi_printf(
      3, "crop", sizeof( struct CROP_PLOT ), DSEKAI_CROPS_MAX );
   assert( sizeof( struct TILEMAP ) < 16384 );

#ifndef NO_ANIMATE
   size_printf( 3, "animations cache",
      sizeof( struct ANIMATION ) * ANIMATE_ANIMATIONS_MAX );
   assert( sizeof( struct ANIMATION ) * ANIMATE_ANIMATIONS_MAX < 16384 );
#endif /* !NO_ANIMATE */

   /* Initialize subsystems. */

   if( !memory_init() ) {
      error_printf( "unable to initialize memory" );
      retval = 1;
      goto exit;
   }

   if( !graphics_init() ) {
      error_printf( "unable to initialize graphics" );
      retval = 1;
      goto exit;
   }

   engines_draw_loading_screen();

   if( !input_init() ) {
      retval = 1;
      error_printf( "unable to initialize input" );
      goto exit;
   }

   if( !window_init( SCREEN_MAP_W, SCREEN_MAP_H ) ) {
      retval = 1;
      error_printf( "unable to initialize GUI" );
      goto exit;
   }

   if( !script_init() ) {
      error_printf( "unable to initialize scripts" );
      retval = 1;
      goto exit;
   }

   /* Setup the engine state. */

   debug_printf( 1, "allocating state..." );
   g_state_handle = memory_alloc( sizeof( struct DSEKAI_STATE ), 1 );
   if( (MEMORY_HANDLE)NULL == g_state_handle ) {
      error_printf( "unable to allocate state!" );
      retval = 1;
      goto exit;
   }

   state = (struct DSEKAI_STATE*)memory_lock( g_state_handle );
   state->version = 1;
   state->engine_state = ENGINE_STATE_OPENING;
   state->menu.menu_id = -1;
   state->menu.highlight_id = -1;
   state->map_handle = memory_alloc( 1, sizeof( struct TILEMAP ) );
   if( (MEMORY_HANDLE)NULL == state->map_handle ) {
      error_printf( "unable to allocate tilemap!" );
      retval = 1;
      goto exit;
   }
   state->items_handle =
      memory_alloc( DSEKAI_ITEMS_MAX, sizeof( struct ITEM ) );
   if( (MEMORY_HANDLE)NULL == state->items_handle ) {
      error_printf( "unable to allocate items!" );
      retval = 1;
      goto exit;
   }
   unilayer_loop_set( engines_loop_iter, g_state_handle );

   /*
   state->warp_to[0] = 'f';
   state->warp_to[1] = 'i';
   state->warp_to[2] = 'e';
   state->warp_to[3] = 'l';
   state->warp_to[4] = 'd';
   */

   state = (struct DSEKAI_STATE*)memory_unlock( g_state_handle );



/* === Main Loop === */

#ifndef PLATFORM_WASM

   while( 0 < g_running ) {
      unilayer_loop_iter();

#ifdef USE_SOFT_ASSERT
      if( 0 < g_assert_failed_len ) {
         while( g_running ) {
            if( g_input_key_quit == input_poll( NULL, NULL ) ) {
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
      retval = 1;
      goto exit;
   }
   if( (MEMORY_HANDLE)NULL != state->map_handle ) {
      memory_free( state->map_handle );
   }
   state = (struct DSEKAI_STATE*)memory_unlock( g_state_handle );

   memory_free( g_state_handle );

   window_shutdown();
   script_shutdown();
   input_shutdown();
   graphics_shutdown();

   logging_shutdown();
 
   platform_shutdown();

#endif /* !PLATFORM_WASM */

exit:

#ifdef DISABLE_MAIN_PARMS
   return;
#else
   return retval;
#endif /* !DISABLE_MAIN_PARMS */
}

