
#include <string.h>

#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "item.h"

#ifdef DISABLE_MAIN_PARMS
void
#else
int
#endif /* DISABLE_MAIN_PARMS */
main(
#ifndef DISABLE_MAIN_PARMS
   int argc, char* argv[]
#endif /* !DISABLE_MAIN_PARMS */
) {
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
   }

   graphics_shutdown();

#ifndef DISABLE_MAIN_PARMS
   return 0;
#endif /* !DISABLE_MAIN_PARMS */
}

