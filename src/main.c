
#include <string.h>

#define MAIN_C

#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "item.h"

uint8_t g_running = 1;

#ifdef USE_SOFT_ASSERT
char g_assert_failed[256];
int g_assert_failed_len;
#endif /* USE_SOFT_ASSERT */

/* ------ */
#ifdef PLATFORM_MAC7
/* ------ */

void main() {
#define DISABLE_MAIN_PARMS

/* ------ */
#elif defined( PLATFORM_PALM )
/* ------ */

UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags ) {

   if( cmd == sysAppLaunchCmdNormalLaunch ) {

/* ------ */
#elif defined( PLATFORM_WIN16 )
/* ------ */

HINSTANCE g_instance = NULL;
HWND g_window = NULL;

int PASCAL WinMain(
   HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow
) {

/* ------ */
#else
/* ------ */

int main( int argc, char* argv[] ) {

/* ------ */
#endif /* PLATFORM_PALM, PLATFORM_MAC7, PLATFORM_WIN16 */
/* ------ */

#ifdef PLATFORM_WIN16
   MSG msg;
   int msg_retval = 0;

   g_instance = hInstance;

   if( hPrevInstance ) {
      error_printf( "previous instance detected" );
      return 1;
   }
#endif /* PLATFORM_WIN16 */

#ifdef LOG_TO_FILE
   g_log_file = fopen( LOG_FILE_NAME, "w" );
#endif /* LOG_TO_FILE */

   if( !graphics_init() ) {
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

#ifdef PLATFORM_WIN16
   ShowWindow( g_window, nCmdShow );
#endif /* PLATFORM_WIN16 */

   window_init();

   while( g_running ) {
#ifdef PLATFORM_WIN16
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
      g_running = topdown_loop();

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

   graphics_shutdown();

#ifdef LOG_TO_FILE
   fclose( g_log_file );
#endif /* LOG_TO_FILE */

#ifndef DISABLE_MAIN_PARMS
   return 0;
#endif /* !DISABLE_MAIN_PARMS */

#ifdef PLATFORM_PALM
   }
#endif /* PLATFORM_PALM */
}

