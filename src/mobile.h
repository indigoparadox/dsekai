
#ifndef MOBILE_H
#define MOBILE_H

#include "config.h"
#include "dstypes.h"
#include "graphics.h"
#include "tilemap.h"

struct MOBILE {
   const GRAPHICS_SPRITE* sprite;
   uint8_t hp;
   uint8_t mp;
   uint8_t tx;
   uint8_t tx_prev;
   uint8_t ty;
   uint8_t ty_prev;
   uint8_t steps;
};

uint8_t mobile_walk_start( struct MOBILE*, int8_t, int8_t );
void mobile_animate(
   struct MOBILE* m, uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW] );
void mobile_draw( struct MOBILE*, int8_t );

#endif /* MOBILE_H */

