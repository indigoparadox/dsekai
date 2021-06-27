
#include <string.h>

#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "item.h"

int main( int argc, char* argv[] ) {
   uint8_t running = 1;

   graphics_init();
   window_init();

   while( running ) {
      running = topdown_loop();
   }

   graphics_shutdown();

   return 0;
}

