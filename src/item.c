
#include "dsekai.h"

void item_draw( const struct ITEM* i, int16_t screen_x, int16_t screen_y ) {
   /*
   int16_t x_offset = 0,
      y_offset = 0;

   graphics_blit_sprite_at(
      i->sprite,
      0, 0,
      (i->coords.x * SPRITE_W) - screen_x,
      (i->coords.y * SPRITE_H) - screen_y,
      SPRITE_W, SPRITE_H);
   */
}

int8_t item_give_mobile(
   struct ITEM* e, struct MOBILE* m, struct DSEKAI_STATE* state
) {

   /* TODO: Test to make sure would dupe True GID OR incr counter depending on
    *       item type.
    */

   debug_printf( 3, "giving item %d to %s", e->gid, m->name );

   return 1;
}

