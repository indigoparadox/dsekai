
#ifndef ITEM_H
#define ITEM_H

#include "tilemap.h"

#define ITEM_INVENTORY_MAX 50

struct ITEM {
   const GRAPHICS_SPRITE* sprite;
   struct TILEMAP_COORDS coords;
   struct ITEM* inventory;
};

#endif /* ITEM_H */

