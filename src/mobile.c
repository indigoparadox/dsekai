
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

void mobile_animate( struct MOBILE* m ) {
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
}

