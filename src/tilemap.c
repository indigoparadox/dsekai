
#include "tilemap.h"

void tilemap_draw(
   const struct TILEMAP* t, uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW],
   uint16_t screen_x, uint16_t screen_y, uint8_t force
) {
   int x = 0,
      y = 0;
   uint8_t tile_id = 0;
   uint16_t viewport_tx1 = 0,
      viewport_ty1 = 0,
      viewport_tx2 = 0,
      viewport_ty2 = 0;

   viewport_tx1 = screen_x / TILE_W;
   viewport_ty1 = screen_y / TILE_H;
   viewport_tx2 = TILEMAP_TW > viewport_tx1 + SCREEN_TW ?
      viewport_tx1 + SCREEN_TW : viewport_tx1 + (TILEMAP_TW - viewport_tx1);
   viewport_ty2 = TILEMAP_TH > viewport_ty1 + SCREEN_TH ?
      viewport_ty1 + SCREEN_TH : viewport_tx1 + (TILEMAP_TH - viewport_ty1);

   assert( viewport_tx2 <= TILEMAP_TW );
   assert( viewport_ty2 <= TILEMAP_TH );

   for( y = viewport_ty1 ; viewport_ty2 > y ; y++ ) {
      for( x = viewport_tx1 ; viewport_tx2 > x ; x++ ) {
#ifndef IGNORE_DIRTY
         if(
            !force &&
            !((*tiles_flags)[y][x] & TILEMAP_TILE_FLAG_DIRTY)
         ) {
            continue;
         }
#endif /* !IGNORE_DIRTY */

         (*tiles_flags)[y][x] &= ~TILEMAP_TILE_FLAG_DIRTY;

         /* Grab the left byte if even or the right if odd. */
         tile_id = tilemap_get_tile_id( t, x, y );

         /* Blit the tile. */
         graphics_tile_at(
            (*t->tileset)[tile_id],
            (x * TILE_W) - screen_x,
            (y * TILE_H) - screen_y );
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

