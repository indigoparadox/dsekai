
#include "mobile.h"

#include "graphics.h"

const int8_t gc_mobile_step_table_normal_pos[16] = {
   0, 4, 4, 4, 8, 8, 8, 8,
   12, 12, 12, 12, 15, 15, 15, 16
};

uint8_t mobile_walk_start( struct MOBILE* m, int8_t x_mod, int8_t y_mod ) {

   if( m->ty != m->ty_prev || m->tx != m->tx_prev ) {
      /* We're already walking! */
      return 0;
   }

   m->tx += x_mod;
   m->ty += y_mod;
   m->steps = gc_mobile_step_table_normal_pos[1];

   return 1;
}

void mobile_animate(
   struct MOBILE* m, uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW]
) {
   if(
      (m->tx != m->tx_prev || m->ty != m->ty_prev) &&
      SPRITE_W > m->steps && (-1 * SPRITE_W) <= m->steps
   ) {
      if( 0 < m->steps ) {
         m->steps = gc_mobile_step_table_normal_pos[m->steps];
      } else if( 0 > m->steps ) {
         m->steps = -1 * gc_mobile_step_table_normal_pos[m->steps];
      }
   } else if(
      m->tx != m->tx_prev &&
      ((SPRITE_W - 1) < m->steps || (SPRITE_W - 1) * -1 > m->steps)
   ) {
      m->tx_prev = m->tx;
   } else if(
      m->ty != m->ty_prev &&
      ((SPRITE_H - 1) < m->steps || (SPRITE_H - 1) * -1 > m->steps)
   ) {
      m->ty_prev = m->ty;
   }
   (*tiles_flags)[m->ty][m->tx] |= TILEMAP_TILE_FLAG_DIRTY;
   (*tiles_flags)[m->ty_prev][m->tx_prev] |= TILEMAP_TILE_FLAG_DIRTY;
}

void mobile_draw( struct MOBILE* m, int8_t walk_offset ) {
   int16_t x_offset = 0,
      y_offset = 0;

   if( m->tx_prev > m->tx ) {
      x_offset = SPRITE_W - m->steps;
   } else if( m->tx_prev < m->tx ) {
      x_offset = (SPRITE_W - m->steps) * -1;
   } else if( m->ty_prev > m->ty ) {
      y_offset = SPRITE_H - m->steps;
   } else if( m->ty_prev < m->ty ) {
      y_offset = (SPRITE_H - m->steps) * -1;
   }

   /* Note that sprite is drawn at prev_x/y + steps, NOT current x/y. */
   graphics_sprite_at(
      m->sprite,
      (m->tx * SPRITE_W) + x_offset,
      (m->ty * SPRITE_H) + y_offset + walk_offset );
}

