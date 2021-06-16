
#include "tilemap.h"

#include "dstypes.h"
#include "graphics.h"

void tilemap_draw(
   const struct TILEMAP* t, uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW]
) {
   int x = 0,
      y = 0;
   uint8_t tile_id = 0;

   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         if( !((*tiles_flags)[y][x] & TILEMAP_TILE_FLAG_DIRTY) ) {
            continue;
         }

         (*tiles_flags)[y][x] &= ~TILEMAP_TILE_FLAG_DIRTY;

         /* Grab the left byte if even or the right if odd. */
         tile_id = tilemap_get_tile_id( t, x, y );

         /* Blit the tile. */
         graphics_tile_at( (*t->tileset)[tile_id], x * TILE_W, y * TILE_H );
      }
   }
}

uint8_t tilemap_collide( const struct TILEMAP* t, uint8_t x, uint8_t y ) {
   uint8_t tile_id = 0;

   tile_id = tilemap_get_tile_id( t, x, y );
   if( (*t->tileset_flags)[tile_id] & (uint8_t)TILEMAP_TILESET_FLAG_BLOCK ) {
      return 1;
   }
   return 0;
}

