
#include "tilemap.h"

#include "graphics.h"

void tilemap_draw( const struct TILEMAP* t ) {
   int x = 0,
      y = 0;
   uint8_t tile_id = 0;

   for( y = 0 ; 6 > y ; y++ ) {
      for( x = 0 ; 20 > x ; x++ ) {
         /* Grab the left byte if even or the right if odd. */
         tile_id = t->tiles[y][x / 2];
         if( 0 == x % 2 ) {
            tile_id >>= 4;
         }
         tile_id &= 0x0f;

         /* Blit the tile. */
         graphics_sprite_at( t->tileset[tile_id],
            x * 8, y * 8, t->tileset_colors[tile_id] );
      }
   }
}

