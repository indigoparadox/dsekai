
#define MOBILE_C
#include "dsekai.h"

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

   if( 1 == x_mod ) {
      m->facing = MOBILE_FACING_EAST;
   } else if( 1 == y_mod ) {
      m->facing = MOBILE_FACING_SOUTH;
   } else if( -1 == x_mod ) {
      m->facing = MOBILE_FACING_WEST;
   } else if( -1 == y_mod ) {
      m->facing = MOBILE_FACING_NORTH;
   }

   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   return 1;
}

void mobile_state_animate( struct DSEKAI_STATE* state ) {
   if( ANI_SPRITE_COUNTDOWN_MAX > state->ani_sprite_countdown ) {
      state->ani_sprite_countdown++;
      return;
   }
   state->ani_sprite_countdown = 0;

   if( 0 == state->ani_sprite_x ) {
      state->ani_sprite_x = 16;
   } else {
      state->ani_sprite_x = 0;
   }
}

void mobile_execute( struct MOBILE* m, struct TILEMAP* t ) {
   struct SCRIPT* script = NULL;
   struct SCRIPT_STEP* step = NULL;

   if(
      0 > m->script_id ||
      m->script_id >= t->scripts_count
   ) {
      /* Invalid script. */
      return;
   }

   if( graphics_get_ms() < m->script_next_ms ) {
      debug_printf( 3, "mobile sleeping: %d waiting for %d", graphics_get_ms(),
         m->script_next_ms );
      /* Mobile still sleeping. */
      return;
   }

   script = &(t->scripts[m->script_id]);
   step = &(script->steps[m->script_pc]);

   debug_printf( 3, "script_exec: script %d, step %d (%d)",
      m->script_id, m->script_pc, step->action );

   m->script_pc = gc_script_handlers[step->action](
      m->script_pc, m, NULL, &(m->coords), step->arg );
}

void mobile_animate( struct MOBILE* m, struct TILEMAP* t ) {
   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   /* If the mobile is walking, advance its steps. */
   m->steps_x = gc_mobile_step_table_normal_pos[m->steps_x];
   m->steps_y = gc_mobile_step_table_normal_pos[m->steps_y];
   
   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   /* Leave a trail of dirty tiles. */
   t->tiles_flags[(m->coords.y * TILEMAP_TW) + m->coords.x] |=
      TILEMAP_TILE_FLAG_DIRTY;
   t->tiles_flags[(m->coords_prev.y * TILEMAP_TW) + m->coords_prev.x] |=
      TILEMAP_TILE_FLAG_DIRTY;

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
   const struct MOBILE* m, const struct DSEKAI_STATE* state,
   int16_t screen_x, int16_t screen_y
) {
   int16_t x_offset = 0,
      y_offset = 0;

   if( !m->active ) {
      return;
   }

   graphics_blit_at(
      m->sprite,
      state->ani_sprite_x,
      m->facing * SPRITE_H,
      screen_x, screen_y,
      SPRITE_W, SPRITE_H );
}

void mobile_deinit( struct MOBILE* m ) {
   if( NULL != m ) {
      return;
   }
}

