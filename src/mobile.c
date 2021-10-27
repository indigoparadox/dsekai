
#define MOBILE_C
#include "dsekai.h"

/* TODO: Facing/x_mod/y_mod lookup tables for DIR constants. */

uint8_t mobile_walk_start( struct MOBILE* m, uint8_t dir ) {

   if(
      m->coords.y != m->coords_prev.y ||
      m->coords.x != m->coords_prev.x
   ) {
      /* We're already walking! */
      return 0;
   }

   m->coords.x += gc_mobile_x_offsets[dir];
   m->coords.y += gc_mobile_y_offsets[dir];
   m->steps_x = gc_mobile_step_table_normal_pos[SPRITE_W - 1];
   m->steps_y = gc_mobile_step_table_normal_pos[SPRITE_H - 1];

   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   return 1;
}

struct MOBILE* mobile_get_facing(
   struct MOBILE* m, struct DSEKAI_STATE* state
) {
   int i = 0;

   assert( 0 <= m->dir );
   assert( 4 > m->dir );

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if( &(state->mobiles[i]) == m ) {
         /* Don't compare to self. */
         continue;
      }

      if(
         state->mobiles[i].active && (
            state->mobiles[i].coords.x ==
               m->coords.x + gc_mobile_x_offsets[m->dir] &&
            state->mobiles[i].coords.y ==
               m->coords.y + gc_mobile_y_offsets[m->dir]
         )
      ) {
         return &(state->mobiles[i]);
      }
   }

   return NULL;
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

struct MOBILE* mobile_interact(
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP* t
) {

   if( 
      NULL == actee ||
      0 > actee->script_id ||
      actee->script_id >= t->scripts_count
   ) {
      /* Invalid script. */
      return NULL;
   }

   if(
      actor->coords.y != actor->coords_prev.y ||
      actor->coords.x != actor->coords_prev.x
   ) {
      /* Reject interactions while walking so we don't break script sync. */
      return 0;
   }

   /* TODO: Workshop this. */

   /* Store mobile previous PC for return. */
   actee->script_pc_prev = actee->script_pc;
   actee->script_pc = script_goto_label(
      actee->script_pc, &(t->scripts[actee->script_id]),
      SCRIPT_ACTION_INTERACT, 0 );
   actee->script_next_ms = graphics_get_ms();

   return actee;
}

void mobile_execute(
   struct MOBILE* m, struct TILEMAP* t, struct DSEKAI_STATE* state
) {
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
      debug_printf( 0,
         "mobile sleeping: %u waiting for %u", graphics_get_ms(),
         m->script_next_ms );
      /* Mobile still sleeping. */
      return;
   }

   script = &(t->scripts[m->script_id]);
   step = &(script->steps[m->script_pc]);

   debug_printf( 0, "%u ms: script_exec: script %d, step %d (%d)",
      graphics_get_ms(), m->script_id, m->script_pc, step->action );

   m->script_pc = gc_script_handlers[step->action](
      m->script_pc, script, t, m, NULL, &(m->coords), state, step->arg );
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
   struct MOBILE* m, const struct DSEKAI_STATE* state,
   int16_t screen_x, int16_t screen_y
) {
   if( !m->active ) {
      return;
   }

   graphics_blit_at(
      m->sprite,
      state->ani_sprite_x,
      m->dir * SPRITE_H,
      screen_x, screen_y,
      SPRITE_W, SPRITE_H );
}

void mobile_deinit( struct MOBILE* m ) {
   if( NULL != m ) {
      return;
   }
}

