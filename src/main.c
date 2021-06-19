
#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"

extern const GRAPHICS_SPRITE gc_sprite_princess;
extern const GRAPHICS_SPRITE gc_sprite_robe;

int main( int argc, char* argv[] ) {
   uint32_t i = 0, j = 0, x = 0, y = 0;
   uint8_t running = 1,
      window_shown = 0;
   uint8_t tiles_flags[TILEMAP_TH][TILEMAP_TW];
   struct MOBILE player = {
      &gc_sprite_robe,
      100,
      100,
      {3, 4},
      {3, 4},
      SPRITE_W
   };
   struct MOBILE mobiles[MOBILES_MAX];
   int mobiles_count = 0;

   graphics_init();
   window_init();

   memset( &tiles_flags, 0x01, TILEMAP_TH * TILEMAP_TW );
   memset( mobiles, 0x0, sizeof( struct MOBILE ) * MOBILES_MAX );

   mobiles[0].sprite = &gc_sprite_princess;
   mobiles[0].hp = 100;
   mobiles[0].mp = 100;
   mobiles[0].coords.x = 5;
   mobiles[0].coords.y = 5;
   mobiles[0].coords_prev.x = 5;
   mobiles[0].coords_prev.y = 5;
   mobiles[0].steps = SPRITE_W;
   mobiles_count++;

   while( running ) {

      running = loop_topdown( &player, mobiles, &mobiles_count, &tiles_flags );
 
   }

   graphics_shutdown();

   return 0;
}

