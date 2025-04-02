
#define MAIN_C

#define RETROFLT_C
#define MAUG_C
#define RETROGXC_C
#define RETROANI_C

#include "dsekai.h"

MAUG_MHANDLE g_state_handle = (MAUG_MHANDLE)NULL;

#ifdef PLATFORM_WIN
#include "winstat.h"
#endif /* PLATFORM_WIN */

/* === Main Class Definition === */

int unilayer_main( int argc, char* argv[] ) {

/* === Main Preamble === */

   struct DSEKAI_STATE* state = NULL;
   int16_t retval = 0;
   struct RETROFLAT_ARGS args;

   assert( 0 == TILEMAP_TW % 4 );
   assert( 0 == TILEMAP_TH % 4 );

   /* Setup logging and log some debug items about this build. */

   logging_init();

#if 0
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
#endif

   /* Initialize subsystems. */

   args.title = "dsekai";

   retval = !retroflat_init( argc, argv, &args );
   maug_cleanup_if_not_ok();

   engines_draw_loading_screen();

#if 0
   if( !window_init(
      SCREEN_MAP_X(), SCREEN_MAP_Y(), SCREEN_MAP_W(), SCREEN_MAP_H() )
   ) {
      retval = 1;
      error_printf( "unable to initialize GUI" );
      goto cleanup;
   }
#endif

#if 0
   if( !script_init() ) {
      error_printf( "unable to initialize scripts" );
      retval = 1;
      goto cleanup;
   }
#endif

   /* Setup the engine state. */

   debug_printf( 1, "allocating state..." );
   g_state_handle = maug_malloc( sizeof( struct DSEKAI_STATE ), 1 );
   if( (MAUG_MHANDLE)NULL == g_state_handle ) {
      error_printf( "unable to allocate state!" );
      retval = 1;
      goto cleanup;
   }

   maug_mlock( g_state_handle, state );
   state->version = 1;
   state->engine_state = ENGINE_STATE_OPENING;
   state->menu.menu_id = -1;
   state->menu.highlight_id = -1;
   
   state->map_handle = maug_malloc( 1, sizeof( struct TILEMAP ) );
   if( (MAUG_MHANDLE)NULL == state->map_handle ) {
      error_printf( "unable to allocate tilemap!" );
      retval = 1;
      goto cleanup;
   }
   
   /* TODO: Start small and resize later. */
   state->items_sz = DSEKAI_ITEMS_MAX;
   state->items_handle =
      maug_malloc( DSEKAI_ITEMS_MAX, sizeof( struct ITEM ) );
   if( (MAUG_MHANDLE)NULL == state->items_handle ) {
      error_printf( "unable to allocate items!" );
      retval = 1;
      goto cleanup;
   }

   /* TODO: Start small and resize later. */
   state->mobiles_sz = DSEKAI_MOBILES_MAX;
   state->mobiles_handle =
      maug_malloc( DSEKAI_MOBILES_MAX, sizeof( struct MOBILE ) );
   if( (MAUG_MHANDLE)NULL == state->mobiles_handle ) {
      error_printf( "unable to allocate mobiles!" );
      retval = 1;
      goto cleanup;
   }

   /* TODO: Start small and resize later. */
   state->crops_sz = DSEKAI_CROPS_MAX;
   state->crops_handle =
      maug_malloc( DSEKAI_CROPS_MAX, sizeof( struct CROP_PLOT ) );
   if( (MAUG_MHANDLE)NULL == state->crops_handle ) {
      error_printf( "unable to allocate crops!" );
      retval = 1;
      goto cleanup;
   }

   retroflat_loop( engines_loop_iter, NULL, g_state_handle );

/* === Shutdown === */

#ifndef RETROFLAT_OS_WASM

   maug_mlock( g_state_handle, state );
   if( NULL == state ) {
      error_printf( "unable to lock state" );
      retval = 1;
      goto cleanup;
   }
   if( (MAUG_MHANDLE)NULL != state->map_handle ) {
      maug_mfree( state->map_handle );
   }
   maug_munlock( g_state_handle, state );

   maug_mfree( g_state_handle );

#if 0
   window_shutdown();
   script_shutdown();
#endif
   retroflat_shutdown( retval );
   logging_shutdown();
 
#endif /* !PLATFORM_WASM */

cleanup:

   return retval;
}

