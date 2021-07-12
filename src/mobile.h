
#ifndef MOBILE_H
#define MOBILE_H

struct MOBILE {
   uint8_t hp;
   uint8_t mp;
   uint32_t sprite;
   struct TILEMAP_COORDS coords;
   struct TILEMAP_COORDS coords_prev;
   uint8_t steps_x;
   uint8_t steps_y;
   uint8_t facing;
   struct ITEM* inventory;
};

#define MOBILE_FACING_SOUTH   0
#define MOBILE_FACING_NORTH   1
#define MOBILE_FACING_RIGHT   2
#define MOBILE_FACING_LEFT    3

#define MOBILE_IACT_NONE      0
#define MOBILE_IACT_TALK      1
#define MOBILE_IACT_ATTACK    2
#define MOBILE_IACT_SHOP      3

uint8_t mobile_walk_start( struct MOBILE*, int8_t, int8_t );
void mobile_interact( struct MOBILE*, struct MOBILE* );
void mobile_interact_txy( struct MOBILE*, struct MOBILE*, uint16_t, uint16_t );
void mobile_animate( struct MOBILE*, struct TILEMAP* );
void mobile_draw( const struct MOBILE*, int8_t, int16_t, int16_t );
void mobile_deinit( struct MOBILE* );

#endif /* MOBILE_H */

