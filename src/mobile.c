
#define MOBILE_C
#include "dsekai.h"

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
   int8_t i = 0;

   assert( 4 > m->dir );

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if( &(state->mobiles[i]) == m ) {
         /* Don't compare to self. */
         continue;
      }

      if(
         (
            MOBILE_FLAG_ACTIVE ==
               (MOBILE_FLAG_ACTIVE & state->mobiles[i].flags)
            ) && (
               state->mobiles[i].coords.x ==
                  m->coords.x + gc_mobile_x_offsets[m->dir] &&
               state->mobiles[i].coords.y ==
                  m->coords.y + gc_mobile_y_offsets[m->dir]
         )
      ) {
         return &(state->mobiles[i]);
      }
   }

   if(
      m != &(state->player) &&
      state->player.coords.x ==
         m->coords.x + gc_mobile_x_offsets[m->dir] &&
      state->player.coords.y ==
         m->coords.y + gc_mobile_y_offsets[m->dir]
   ) {
      return &(state->player);
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
      state->ani_sprite_x = SPRITE_W;
   } else {
      state->ani_sprite_x = 0;
   }
}

void mobile_stack_push( struct MOBILE* m, int8_t v ) {
   uint8_t i = 0;

   /* Shift the stack values out of the way for a new one. */
   for( i = SCRIPT_STACK_DEPTH - 1 ; 0 < i ; i-- ) {
      m->script_stack[i] = m->script_stack[i - 1];
   }

   /* Push the value. */
   m->script_stack[0] = v;

   debug_printf( 0, "mobile pushed: %d", v );
}

int8_t mobile_stack_pop( struct MOBILE* m ) {
   uint8_t i = 0;
   int8_t retval = 0;

   retval = m->script_stack[0];

   /* Pull the stack values down on top of the one we popped. */
   for( i = 0 ; SCRIPT_STACK_DEPTH - 1 > i ; i++ ) {
      m->script_stack[i] = m->script_stack[i + 1];
   }
   /* Zero out the former deepest stack slot. */
   m->script_stack[SCRIPT_STACK_DEPTH - 1] = 0;

   debug_printf( 0, "mobile popped: %d", retval );

   return retval;
}

struct MOBILE* mobile_interact(
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP* t
) {

   if( 
      NULL == actee ||
      (MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & actee->flags)) ||
      0 >= actee->hp ||
      0 > actee->script_id ||
      actee->script_id >= TILEMAP_SCRIPTS_MAX
   ) {
      /* Inactive mobile or invalid script. */
      return NULL;
   }

   if(
      actor->coords.y != actor->coords_prev.y ||
      actor->coords.x != actor->coords_prev.x
   ) {
      /* Reject interactions while walking so we don't break script sync. */
      return 0;
   }

   /* Push actee previous PC for return. */
   mobile_stack_push( actee, actee->script_pc );

   /* Set actee's pc to the GOTO for interaction and make actee active NOW. */
   actee->script_pc = script_goto_label(
      actee->script_pc, &(t->scripts[actee->script_id]),
      SCRIPT_ACTION_INTERACT, 0 );
   actee->script_next_ms = graphics_get_ms();

   return actee;
}

void mobile_execute( struct MOBILE* m, struct DSEKAI_STATE* state ) {
   struct SCRIPT* script = NULL;
   struct SCRIPT_STEP* step = NULL;
   int16_t arg = 0;
   struct TILEMAP* map = NULL;

   if(
      (MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & m->flags)) ||
      0 > m->script_id ||
      m->script_id >= TILEMAP_SCRIPTS_MAX
   ) {
      /* Mobile inactive or invalid script. */
      return;
   }

   if( graphics_get_ms() < m->script_next_ms ) {
      debug_printf( 0,
         "mobile \"%s\" sleeping: %u waiting for %u", m->name,
         graphics_get_ms(), m->script_next_ms );
      /* Mobile still sleeping. */
      return;
   }

   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
      error_printf( "could not lock tilemap" );
      return;
   }

   script = &(map->scripts[m->script_id]);
   step = &(script->steps[m->script_pc]);

   if( SCRIPT_ARG_STACK == step->arg ) {
      arg = mobile_stack_pop( m );
   } else {
      arg = step->arg;
   }

   debug_printf( 0, "%u ms: script_exec: script %d, step %d (%d)",
      graphics_get_ms(), m->script_id, m->script_pc, step->action );

   m->script_pc = gc_script_handlers[step->action](
      m->script_pc, script, map, m, NULL, &(m->coords), state, arg );

   map = (struct TILEMAP*)memory_unlock( state->map_handle );
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

   /* Sync up prev and current coords if no steps left. */
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

   /* Use negative HP for blinking effect on death, or remove if we're up to -1.
    */
   if( -1 == m->hp ) {
      m->flags &= ~MOBILE_FLAG_ACTIVE;
   } else if( 0 > m->hp ) {
      m->hp++;
   }
}

void mobile_deinit( struct MOBILE* m ) {
   if( NULL != m ) {
      return;
   }
}

struct MOBILE* mobile_spawn_npc( struct DSEKAI_STATE* state, uint8_t player ) {
   int8_t i = 0;
   struct MOBILE* mobile_out = NULL;

   if( player ) {
      if(
         MOBILE_FLAG_ACTIVE == (MOBILE_FLAG_ACTIVE & state->player.flags)
      ) {
         error_printf( "player mobile already in use!" );
         return NULL;
      }
      
      mobile_out = &(state->player);
   } else {
      for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
         if(
            MOBILE_FLAG_ACTIVE == (MOBILE_FLAG_ACTIVE & state->mobiles[i].flags)
         ) {
            continue;
         }

         mobile_out = &(state->mobiles[i]);
         break;
      }

      if( i >= DSEKAI_MOBILES_MAX ) {
         error_printf( "no available mobiles to spawn!" );
         return NULL;
      }
   }

   mobile_out->hp = 100;
   mobile_out->mp = 100;
   mobile_out->steps_x = 0;
   mobile_out->steps_y = 0;
   mobile_out->script_pc = 0;
   mobile_out->script_next_ms = graphics_get_ms();
   mobile_out->flags = MOBILE_FLAG_ACTIVE;
   mobile_out->spawned_ms = graphics_get_ms();
   
   return mobile_out;
}

void mobile_spawns( struct DSEKAI_STATE* state, struct TILEMAP* map ) {
   int8_t i = 0;
   uint8_t player = 0;
   struct MOBILE* mobile_iter = NULL;

   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      /* If the spawner has no name, skip it. */
      if( 0 == memory_strnlen_ptr(
         map->spawns[i].name, TILEMAP_SPAWN_NAME_SZ )
      ) {
         continue;
      }

      if( 0 == memory_strncmp_ptr( "player", map->spawns[i].name, 6 ) ) {
         player = 1;
      } else {
         player = 0;
      }

      mobile_iter = mobile_spawn_npc( state, player );
      if( NULL == mobile_iter ) {
         continue;
      }

      /* Assign the rest of the properties from the spawner. */
      mobile_iter->name = map->spawns[i].name;
      mobile_iter->coords.x = map->spawns[i].coords.x;
      mobile_iter->coords.y = map->spawns[i].coords.y;
      mobile_iter->coords_prev.x = map->spawns[i].coords.x;
      mobile_iter->coords_prev.y = map->spawns[i].coords.y;
      mobile_iter->script_id = map->spawns[i].script_id;
      resource_assign_id( mobile_iter->sprite, map->spawns[i].type );
   }
}

