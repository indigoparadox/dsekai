
#include "mobile.h"

const int8_t gc_mobile_step_table_normal_pos[16] = {
   0, 0, 0, 0,       /*  0,  1,  2,  3 */
   3, 3, 3, 3,       /*  4,  5,  6,  7 */
   7, 7, 7, 7,       /*  8,  9, 10, 11 */
   11, 11, 11, 11    /* 12, 13, 14, 15 */
};

uint8_t mobile_walk_start( struct MOBILE* m, int8_t x_mod, int8_t y_mod ) {

   if(
      m->coords.y != m->coords_prev.y ||
      m->coords.x != m->coords_prev.x
   ) {
      /* We're already walking! */
      return 0;
   }

   m->coords.x += x_mod;
   m->coords.y += y_mod;
   m->steps_x = gc_mobile_step_table_normal_pos[SPRITE_W - 1];
   m->steps_y = gc_mobile_step_table_normal_pos[SPRITE_H - 1];

   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   return 1;
}

void mobile_animate(
   struct MOBILE* m, uint8_t* tiles_flags, uint16_t tiles_flags_w,
   uint16_t tiles_flags_h
) {
   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   /* If the mobile is walking, advance its steps. */
   m->steps_x = gc_mobile_step_table_normal_pos[m->steps_x];
   m->steps_y = gc_mobile_step_table_normal_pos[m->steps_y];
   
   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   /* Leave a trail of dirty tiles. */
   tiles_flags[(m->coords.y * tiles_flags_w) + m->coords.x] |= TILEMAP_TILE_FLAG_DIRTY;
   tiles_flags[(m->coords_prev.y * tiles_flags_w) + m->coords_prev.x] |= TILEMAP_TILE_FLAG_DIRTY;

   if(
      m->coords.x != m->coords_prev.x ||
      m->coords.y != m->coords_prev.y
   ) {
      if( 0 == m->steps_x ) {
         m->coords_prev.x = m->coords.x;
      }
      if( 0 == m->steps_y ) {
         m->coords_prev.y = m->coords.y;
      }
   }
}

void mobile_draw(
   const struct MOBILE* m,
   int8_t walk_offset, int16_t screen_x, int16_t screen_y
) {
   int16_t x_offset = 0,
      y_offset = 0;

   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   if( m->coords_prev.x > m->coords.x ) {
      x_offset = SPRITE_W - m->steps_x;
   } else if( m->coords_prev.x < m->coords.x ) {
      x_offset = (SPRITE_W - m->steps_y) * -1;
   } else if( m->coords_prev.y > m->coords.y ) {
      y_offset = SPRITE_H - m->steps_y;
   } else if( m->coords_prev.y < m->coords.y ) {
      y_offset = (SPRITE_H - m->steps_x) * -1;
   }

   assert( SPRITE_W > x_offset );
   assert( SPRITE_H > y_offset );

   graphics_blit_at(
      &(m->sprite),
      ((m->coords.x * SPRITE_W) + x_offset) - screen_x,
      (((m->coords.y * SPRITE_H) + y_offset + walk_offset ) - screen_y),
      /* Chop off bottom 2px of sprite to fit it within bounds accounting
         for walking offset. */
      SPRITE_W, SPRITE_H - 2 );
}

void mobile_deinit( struct MOBILE* m ) {
   if( NULL != m ) {
      return;
   }
   graphics_unload_bitmap( &(m->sprite) );
}

