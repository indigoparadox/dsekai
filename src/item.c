
#include "dstypes.h"

void item_draw( const struct ITEM* i, int16_t screen_x, int16_t screen_y ) {
   int16_t x_offset = 0,
      y_offset = 0;

   graphics_blit_at(
      i->sprite,
      (i->coords.x * SPRITE_W) - screen_x,
      (i->coords.y * SPRITE_H) - screen_y,
      SPRITE_W, SPRITE_H);
}

