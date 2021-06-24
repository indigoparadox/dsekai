
#include <string.h>

#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "item.h"

int main( int argc, char* argv[] ) {
   uint8_t running = 1;
   uint8_t tiles_flags[TILEMAP_TH][TILEMAP_TW];
   struct MOBILE mobiles[MOBILES_MAX];
   /* struct ITEM items[ITEMS_MAX]; */
   int mobiles_count = 0;

   graphics_init();
   window_init();

   memset( tiles_flags, 0x01, TILEMAP_TH * TILEMAP_TW );
   memset( mobiles, 0x0, sizeof( struct MOBILE ) * MOBILES_MAX );

   while( running ) {
      running = topdown_loop( mobiles, &mobiles_count, &tiles_flags );
   }

   graphics_shutdown();

   return 0;
}

