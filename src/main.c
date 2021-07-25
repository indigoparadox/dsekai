
#define MAIN_C

#include "dsekai.h"

uint8_t g_running = 1;
MEMORY_HANDLE g_state_handle = NULL;

#ifdef PLATFORM_WIN
HINSTANCE g_instance = NULL;
HWND g_window = NULL;
#endif /* PLATFORM_WIN */

#ifdef USE_SOFT_ASSERT
char g_assert_failed[256];
int g_assert_failed_len;
#endif /* USE_SOFT_ASSERT */



/* === Main Class Definition === */

/* ------ */
#if defined( PLATFORM_PALM )
/* ------ */

UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags ) {

/* ------ */
#elif defined( PLATFORM_WIN )
/* ------ */

int PLATFORM_API WinMain(
   HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow
) {

/* ------ */
#elif defined( DISABLE_MAIN_PARMS )
/* ------ */

void main() {

/* ------ */
#else
/* ------ */

int main( int argc, char* argv[] ) {

/* ------ */
#endif /* PLATFORM_PALM, PLATFORM_MAC7, PLATFORM_WIN */
/* ------ */



/* === Main Preamble === */

   struct DSEKAI_STATE* state = NULL;
   struct GRAPHICS_ARGS graphics_args;

#ifdef PLATFORM_PALM
   if( cmd == sysAppLaunchCmdNormalLaunch ) {

#elif defined( PLATFORM_WIN )
   MSG msg;
   int msg_retval = 0;

   g_instance = hInstance;
   graphics_args.cmd_show = nCmdShow;

   if( hPrevInstance ) {
      error_printf( "previous instance detected" );
      return 1;
   }

#endif /* PLATFORM_WIN */

   assert( 0 == TILEMAP_TW % 4 );
   assert( 0 == TILEMAP_TH % 4 );



/* === Subsystem Initialization === */

#ifdef LOG_TO_FILE
   g_log_file = platform_fopen( LOG_FILE_NAME, "w" );
#endif /* LOG_TO_FILE */

   debug_printf( 3,
      "dsekai compiled " __DATE__ __TIME__ ", state size is %lu bytes",
      sizeof( struct DSEKAI_STATE ) );

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
   if( NULL == g_state_handle ) {
      error_printf( "unable to allocate state" );
#ifdef DISABLE_MAIN_PARMS
      return;
#else
      return 1;
#endif /* DISABLE_MAIN_PARMS */
   }



/* === Main Loop === */

   while( g_running ) {
#if defined( PLATFORM_WIN )
      /* In Windows, this stuff is handled by the message processor. */
      msg_retval = GetMessage( &msg, NULL, 0, 0 );
      if( 0 >= msg_retval ) {
         g_running = 0;
         break;
      }

      TranslateMessage( &msg );
      DispatchMessage( &msg );
#elif defined( SKELETON_LOOP )
      if( INPUT_KEY_QUIT == input_poll() ) {
         g_running = 0;
      }
#else
      g_running = topdown_loop( g_state_handle );
      graphics_flip( &graphics_args );

#endif /* SKELETON_LOOP */

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
   graphics_shutdown();

#ifdef LOG_TO_FILE
   platform_fclose( g_log_file );
#endif /* LOG_TO_FILE */

#ifndef DISABLE_MAIN_PARMS
   return 0;
#endif /* !DISABLE_MAIN_PARMS */

#ifdef PLATFORM_PALM
   }
#endif /* PLATFORM_PALM */
}

