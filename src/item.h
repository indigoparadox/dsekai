
#ifndef ITEM_H
#define ITEM_H

#define ITEMS_MAX 500
#define ITEM_INVENTORY_MAX 50

struct ITEM {
   uint32_t sprite;
   struct TILEMAP_COORDS coords;
   struct ITEM* inventory;
};

void item_draw( const struct ITEM*, int16_t, int16_t );

#endif /* ITEM_H */

