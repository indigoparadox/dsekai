
#ifndef MOBILE_H
#define MOBILE_H

#include "dstypes.h"
#include "graphics.h"
#include "tilemap.h"

struct ITEM;

struct MOBILE {
   struct GRAPHICS_BITMAP sprite;
   uint8_t hp;
   uint8_t mp;
   struct TILEMAP_COORDS coords;
   struct TILEMAP_COORDS coords_prev;
   uint8_t steps_x;
   uint8_t steps_y;
   struct ITEM* inventory;
};

uint8_t mobile_walk_start( struct MOBILE*, int8_t, int8_t );
#ifndef ANCIENT_C
void mobile_animate( struct MOBILE* m, uint8_t*, uint16_t, uint16_t );
#endif /* ANCIENT_C */
void mobile_draw( const struct MOBILE*, int8_t, int16_t, int16_t );

#endif /* MOBILE_H */

