
#ifndef ITEM_H
#define ITEM_H

#include "tilemap.h"

#define ITEMS_MAX 500
#define ITEM_INVENTORY_MAX 50

struct ITEM {
   struct GRAPHICS_BITMAP* sprite;
   struct TILEMAP_COORDS coords;
   struct ITEM* inventory;
};

void item_draw( const struct ITEM*, int16_t, int16_t );

#endif /* ITEM_H */

