
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
   struct MOBILE* m, struct TILEMAP* t, struct DSEKAI_STATE* state
) {
   int16_t i = 0;

   /* TODO: Implement resource gathering here, maybe? */

   assert( 4 > mobile_get_dir( m ) );

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if( &(state->mobiles[i]) == m ) {
         /* Don't compare to self. */
         continue;
      }

      if(
         MOBILE_FLAG_ACTIVE ==
            (MOBILE_FLAG_ACTIVE & state->mobiles[i].flags) &&
         state->mobiles[i].map_gid == t->gid &&
         state->mobiles[i].coords.x ==
            m->coords.x + gc_mobile_x_offsets[mobile_get_dir( m )] &&
         state->mobiles[i].coords.y ==
            m->coords.y + gc_mobile_y_offsets[mobile_get_dir( m )]
      ) {
         /* Found an active facing mobile on the same tilemap. */
         return &(state->mobiles[i]);
      }
   }

   /* This mobile is facing the player. */
   if(
      m != &(state->player) &&
      state->player.coords.x ==
         m->coords.x + gc_mobile_x_offsets[mobile_get_dir( m )] &&
      state->player.coords.y ==
         m->coords.y + gc_mobile_y_offsets[mobile_get_dir( m )]
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
   int16_t i = 0;

   /* Shift the stack values out of the way for a new one. */
   for( i = SCRIPT_STACK_DEPTH - 1 ; 0 < i ; i-- ) {
      m->script_stack[i] = m->script_stack[i - 1];
   }

   /* Push the value. */
   m->script_stack[0] = v;

#ifdef SCRIPT_TRACE
   debug_printf( 1, "mobile %u:%u \"%s\" pushed: %d",
      m->map_gid, m->spawner_gid, m->name, v );
#endif /* SCRIPT_TRACE */
}

int8_t mobile_stack_pop( struct MOBILE* m ) {
   int16_t i = 0;
   int8_t retval = 0;

   retval = m->script_stack[0];

   /* Pull the stack values down on top of the one we popped. */
   for( i = 0 ; SCRIPT_STACK_DEPTH - 1 > i ; i++ ) {
      m->script_stack[i] = m->script_stack[i + 1];
   }
   /* Zero out the former deepest stack slot. */
   m->script_stack[SCRIPT_STACK_DEPTH - 1] = 0;

#ifdef SCRIPT_TRACE
   debug_printf( 1, "mobile %u:%u \"%s\" popped: %d",
      m->map_gid, m->spawner_gid, m->name, retval );
#endif /* SCRIPT_TRACE */

   return retval;
}

struct MOBILE* mobile_interact(
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP* t
) {

   /* TODO: Block more than one interaction per instruction cycle. */

   if( 
      NULL == actee ||
      (MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & actee->flags)) ||
      0 >= (actee->mp_hp & MOBILE_HP_MASK) ||
      0 > actee->script_id ||
      actee->script_id >= TILEMAP_SCRIPTS_MAX ||
      (MOBILE_FLAG_DISABLED == (MOBILE_FLAG_DISABLED & actee->flags))
   ) {
      /* Inactive mobile or invalid script. */
      return NULL;
   }

   if(
      actor->coords.y != actor->coords_prev.y ||
      actor->coords.x != actor->coords_prev.x
   ) {
      /* Reject interactions while walking so we don't break script sync. */
      return NULL;
   }

   /* Push actee previous PC for return. */
   mobile_stack_push( actee, actee->script_pc );

   debug_printf( 1, "mobile interacted at icount: %d",
      mobile_get_icount( actee ) );

   /* Set actee's pc to the GOTO for interaction and make actee active NOW. */
   actee->script_pc = script_goto_label(
      actee->script_pc, &(t->scripts[actee->script_id]),
      SCRIPT_ACTION_INTERACT, mobile_get_icount( actee ) );
   actee->script_wait_frames = 0;

   return actee;
}

void mobile_execute( struct MOBILE* m, struct DSEKAI_STATE* state ) {
   struct SCRIPT* script = NULL;
   struct SCRIPT_STEP* step = NULL;
   int16_t arg = 0;
   uint8_t push_arg_after = 0;
   struct TILEMAP* t = NULL;

   /* Lock tilemap. */
   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == t ) {
      error_printf( "could not lock tilemap" );
      goto cleanup;
   }

   /* Check if we should execute. */
   if(
      (MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & m->flags)) ||
      m->map_gid != t->gid ||
      0 > m->script_id ||
      m->script_id >= TILEMAP_SCRIPTS_MAX
   ) {
      /* Mobile inactive or invalid script. */
      goto cleanup;
   }

   /* Check for sleeping. */
   if( 0 < m->script_wait_frames ) {
      m->script_wait_frames--;
#ifdef SCRIPT_TRACE
      debug_printf( 1,
         "mobile %u:%u \"%s\" sleeping: waiting for %d more frames",
         m->map_gid, m->spawner_gid, m->name, m->script_wait_frames );
#endif /* SCRIPT_TRACE */
      /* Mobile still sleeping. */
      goto cleanup;
   }

   script = &(t->scripts[m->script_id]);
   step = &(script->steps[m->script_pc]);

   if( SCRIPT_ARG_STACK_I == step->arg ) {
      arg = mobile_stack_pop( m );
      mobile_incr_icount( m, 1 );
   } else if( SCRIPT_ARG_STACK == step->arg ) {
      arg = mobile_stack_pop( m );
   } else if( SCRIPT_ARG_STACK_P == step->arg ) {
      arg = mobile_stack_pop( m );
      push_arg_after = 1;
   } else if( SCRIPT_ARG_RANDOM == step->arg ) {
      arg = graphics_get_random( 0, SCRIPT_STACK_MAX );
   } else if( SCRIPT_ARG_FOLLOW == step->arg ) {
      arg = mobile_pathfind(
         m, state->player.coords.x, state->player.coords.y, t, state );
      if( MOBILE_ERROR_BLOCKED == arg ) {
#ifdef SCRIPT_TRACE
         debug_printf( 1, "mobile %u:%u \"%s\" pathfinding blocked",
            m->map_gid, m->spawner_gid, m->name );
#endif /* SCRIPT_TRACE */
      }
   } else {
      arg = step->arg;
   }

#ifdef SCRIPT_TRACE
   debug_printf( 1, "mobile %u:%u \"%s\" script_exec: script %d, step %d (%d)",
      m->map_gid, m->spawner_gid, m->name,
      m->script_id, m->script_pc, step->action );
#endif /* SCRIPT_TRACE */

   m->script_pc = gc_script_handlers[step->action](
      m->script_pc, script, t, m, NULL, &(m->coords), state, arg );

   if( push_arg_after ) {
      mobile_stack_push( m, arg );
   }

cleanup:
   if( NULL != t ) {
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }
}

void mobile_animate( struct MOBILE* m, struct DSEKAI_STATE* state ) {
   struct TILEMAP* t = NULL;

   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   /* If the mobile is walking, advance its steps. */
   m->steps_x = gc_mobile_step_table_normal_pos[m->steps_x];
   m->steps_y = gc_mobile_step_table_normal_pos[m->steps_y];
   
   assert( SPRITE_W > m->steps_x );
   assert( SPRITE_H > m->steps_y );

   /* TODO: Can we move this out? */
   t = (struct TILEMAP*)memory_lock( state->map_handle );
   /* Leave a trail of dirty tiles. */
   t->tiles_flags[(m->coords.y * TILEMAP_TW) + m->coords.x] |=
      TILEMAP_TILE_FLAG_DIRTY;
   t->tiles_flags[(m->coords_prev.y * TILEMAP_TW) + m->coords_prev.x] |=
      TILEMAP_TILE_FLAG_DIRTY;
   t = (struct TILEMAP*)memory_unlock( state->map_handle );

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

   /* Use negative HP for blinking effect on death, or remove if we're up to 
    * -1.
    */
   if( -1 == m->mp_hp ) {
      m->flags &= ~MOBILE_FLAG_ACTIVE;
   } else if( 0 > m->mp_hp ) {
      m->mp_hp++;
   }
}

void mobile_deinit( struct MOBILE* m ) {
   if( NULL == m ) {
      return;
   }

   debug_printf( 1, "de-initializing mobile tilemap: %d spawner: %d",
      m->map_gid, m->spawner_gid );
   memory_zero_ptr( (MEMORY_PTR)m, sizeof( struct MOBILE ) );
}

struct MOBILE* mobile_spawn_single(
   uint16_t flags, struct DSEKAI_STATE* state
) {
   int16_t i = 0;
   struct MOBILE* mobile_out = NULL;

   if( MOBILE_FLAG_PLAYER == (MOBILE_FLAG_PLAYER & flags) ) {
      /* Check to see if player already exists. */
      if(
         MOBILE_FLAG_ACTIVE == (MOBILE_FLAG_ACTIVE & state->player.flags)
      ) {
         error_printf( "player mobile already in use!" );
         return NULL;
      }
      
      /* Player doesn't exist, so assign it and config below. */
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

   mobile_out->mp_hp = 100;
   mobile_out->steps_x = 0;
   mobile_out->steps_y = 0;
   mobile_out->script_pc = 0;
   mobile_out->script_wait_frames = 0;
   mobile_out->flags = MOBILE_FLAG_ACTIVE;
   mobile_out->map_gid = MOBILE_MAP_GID_ALL;
   
   return mobile_out;
}

int16_t mobile_spawner_match(
   struct TILEMAP_SPAWN* spawner, struct TILEMAP* t, struct MOBILE* mobiles
) {
   int16_t i = 0;

   /* See if the mobile was spawned on a previous visit to this tilemap. */
   debug_printf( 1, "search tilemap %d, spawner %d", t->gid, spawner->gid );
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if(
         MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & mobiles[i].flags)
      ) {
         continue;
      }
      debug_printf( 1, "sgid m: %u vs s: %u",
         mobiles[i].spawner_gid, spawner->gid );
      debug_printf( 1, "mgid m: %u vs s: %u",
         mobiles[i].map_gid, t->gid );
      if(
         MOBILE_FLAG_ACTIVE == (MOBILE_FLAG_ACTIVE & mobiles[i].flags) &&
         mobiles[i].spawner_gid == spawner->gid &&
         mobiles[i].map_gid == t->gid
      ) {
         /* This mobile was spawned already, so update volatile stuff
            * and move on.
            */
         mobiles[i].sprite_id = graphics_cache_load_bitmap( spawner->sprite );
         mobiles[i].name = spawner->name;
         goto cleanup;
      }
   }

   i = -1;

cleanup:

   return i;
}

void mobile_spawns( struct TILEMAP* t, struct DSEKAI_STATE* state ) {
   int16_t i = 0,
      match = 0;
   struct MOBILE* mobile_iter = NULL;

   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      /* If the spawner has no name, skip it. */
      if( 0 == memory_strnlen_ptr(
         t->spawns[i].name, TILEMAP_SPAWN_NAME_SZ )
      ) {
         continue;
      }

      /* See if the mobile was spawned on a previous visit to this tilemap. */
      match = mobile_spawner_match( &(t->spawns[i]), t, state->mobiles );
      if( 0 <= match ) {
         debug_printf( 1,
            "found existing spawn for tilemap %u spawner %u; skipping",
            t->gid, t->spawns[i].gid );
         continue;
      }

      /* Select the mobile slot or player slot to spawn to. */
      mobile_iter = mobile_spawn_single( t->spawns[i].flags, state );
      if( NULL == mobile_iter ) {
         error_printf( "coult not spawn mobile for tilemap %u spawner %u",
            t->gid, t->spawns[i].gid );
         continue;
      }

      debug_printf( 1, "spawning mobile for tilemap %u spawner %u",
         t->gid, t->spawns[i].gid );

      /* Assign the rest of the properties from the spawner. */
      mobile_iter->name = t->spawns[i].name;
      mobile_iter->coords.x = t->spawns[i].coords.x;
      mobile_iter->coords.y = t->spawns[i].coords.y;
      mobile_iter->coords_prev.x = t->spawns[i].coords.x;
      mobile_iter->coords_prev.y = t->spawns[i].coords.y;
      mobile_iter->script_id = t->spawns[i].script_id;
      mobile_iter->flags |= t->spawns[i].flags;
      mobile_iter->spawner_gid = t->spawns[i].gid;
      if( MOBILE_FLAG_PLAYER != (t->spawns[i].flags & MOBILE_FLAG_PLAYER) ) {
         /* The player is on all tilemaps, but other mobiles limited to one. */
         mobile_iter->map_gid = t->gid;
      } else {
         /* Save player sprite for tilemap transitions. */
         resource_assign_id( state->player_sprite, t->spawns[i].sprite );
      }
      mobile_iter->sprite_id =
         graphics_cache_load_bitmap( t->spawns[i].sprite );
   }
}

int8_t mobile_pathfind(
   struct MOBILE* m, uint8_t x_tgt, uint8_t y_tgt, struct TILEMAP* t,
   struct DSEKAI_STATE* state
) {
   /* TODO: Implement pathfinding. */
   return MOBILE_ERROR_BLOCKED;
}

