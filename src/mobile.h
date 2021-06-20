
#ifndef MOBILE_H
#define MOBILE_H

#include "dstypes.h"
#include "graphics.h"
#include "tilemap.h"

struct ITEM;

struct MOBILE {
   const GRAPHICS_SPRITE* sprite;
   uint8_t hp;
   uint8_t mp;
   struct TILEMAP_COORDS coords;
   struct TILEMAP_COORDS coords_prev;
   uint8_t steps;
   struct ITEM* inventory;
};

uint8_t mobile_walk_start( struct MOBILE*, int8_t, int8_t );
void mobile_animate(
   struct MOBILE* m, uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW] );
void mobile_draw( const struct MOBILE*, int8_t, int16_t, int16_t );

#endif /* MOBILE_H */

