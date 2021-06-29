
#include <string.h>

#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "item.h"

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

/* ------ */
#elif defined( PLATFORM_WIN16 )
/* ------ */

HINSTANCE g_instance = NULL;

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

   uint8_t running = 1;

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
   window_init();

   while( running ) {
#ifdef SKELETON_LOOP
      if( INPUT_KEY_QUIT == input_poll() ) {
         running = 0;
      }
#else
      running = topdown_loop();

#endif /* SKELETON_LOOP */

#ifdef USE_SOFT_ASSERT
      if( 0 < g_assert_failed_len ) {
         while( running ) {
            if( INPUT_KEY_QUIT == input_poll() ) {
               running = 0;
            }
            WinDrawChars( g_assert_failed, g_assert_failed_len, 10, 20 );
         }
      }
#endif /* USE_SOFT_ASSERT */
   }

   graphics_shutdown();

#ifndef DISABLE_MAIN_PARMS
   return 0;
#endif /* !DISABLE_MAIN_PARMS */
}

