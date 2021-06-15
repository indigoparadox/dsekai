
#include "mobile.h"

#include "graphics.h"

uint8_t mobile_walk_start( struct MOBILE* m, int8_t x_mod, int8_t y_mod ) {

   if( m->ty != m->ty_prev || m->tx != m->tx_prev ) {
      /* We're already walking! */
      return 0;
   }

   m->tx += x_mod;
   m->ty += y_mod;
   m->steps = x_mod > 0 || y_mod > 0 ? 1 : -1;

   return 1;
}

void mobile_animate(
   struct MOBILE* m, uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW]
) {
   if(
      (m->tx != m->tx_prev || m->ty != m->ty_prev) &&
      SPRITE_W >= m->steps &&
      (-1 * SPRITE_W) <= m->steps
   ) {
      if( 0 < m->steps ) {
         m->steps++;
      } else {
         m->steps--;
      }
   } else if(
      m->tx != m->tx_prev &&
      (SPRITE_W < m->steps || SPRITE_W * -1 > m->steps)
   ) {
      m->tx_prev = m->tx;
   } else if(
      m->ty != m->ty_prev &&
      (SPRITE_W < m->steps || SPRITE_W * -1 > m->steps)
   ) {
      m->ty_prev = m->ty;
   }
   (*tiles_flags)[m->ty][m->tx] |= TILEMAP_TILE_FLAG_DIRTY;
   if( 0 < m->ty ) {
      (*tiles_flags)[m->ty - 1][m->tx] |= TILEMAP_TILE_FLAG_DIRTY;
   }
   (*tiles_flags)[m->ty_prev][m->tx_prev] |= TILEMAP_TILE_FLAG_DIRTY;
   if( 0 < m->ty_prev ) {
      (*tiles_flags)[m->ty_prev - 1][m->tx_prev] |= TILEMAP_TILE_FLAG_DIRTY;
   }
}

