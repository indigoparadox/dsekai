
#ifndef ITEM_H
#define ITEM_H

struct PACKED ITEM {
   RESOURCE_ID sprite;
   struct TILEMAP_COORDS coords;
   struct ITEM* inventory;
   struct ITEM* next;
};

void item_draw( const struct ITEM*, int16_t, int16_t );

#endif /* ITEM_H */

