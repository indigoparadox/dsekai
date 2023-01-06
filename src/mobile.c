
#define MOBILE_C
#include "dsekai.h"

uint8_t mobile_walk_start( struct MOBILE* m, uint8_t dir ) {

   if( mobile_is_walking( m ) ) {
      /* We're already walking! */
      return 0;
   }

   m->coords.x += gc_mobile_x_offsets[dir];
   m->coords.y += gc_mobile_y_offsets[dir];
#ifdef NO_SMOOTH_WALK
   m->steps_remaining = 0; /* Just take one "step" on next tick. */
#else
   /* Use lookup table to determine first step size. */
   m->steps_remaining = gc_mobile_step_table_normal_pos[SPRITE_W - 1];
#endif /* NO_SMOOTH_WALK */

   assert( SPRITE_W > m->steps_remaining && SPRITE_H > m->steps_remaining );

   return 1;
}

MOBILE_GID mobile_get_facing(
   uint8_t x, uint8_t y, uint8_t dir,
   struct TILEMAP* t, struct DSEKAI_STATE* state
) {
   int16_t i = 0;
   struct MOBILE* mobiles = NULL;
   MOBILE_GID gid_out = MOBILE_GID_NONE;

   /* TODO: Implement resource gathering here, maybe? */

   mobiles = memory_lock( state->mobiles_handle );
   if( NULL == mobiles ) {
      error_printf( "could not lock mobiles handle" );
      goto cleanup;
   }

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      /* TODO: Compact mobiles list, maybe on zone? */
      /* mobile_break_if_last( state->mobiles, i ); */

      if(
         MOBILE_FLAG_ACTIVE == (MOBILE_FLAG_ACTIVE & mobiles[i].flags) &&
         mobiles[i].map_gid == t->gid &&
         mobiles[i].coords.x == x + gc_mobile_x_offsets[dir] &&
         mobiles[i].coords.y == y + gc_mobile_y_offsets[dir]
      ) {
         /* Found an active facing mobile on the same tilemap. */
         gid_out = mobile_get_gid( &(mobiles[i]) );
         goto cleanup;
      }
   }

   /* This mobile is facing the player. */
   if(
      state->player.coords.x == x + gc_mobile_x_offsets[dir] &&
      state->player.coords.y == y + gc_mobile_y_offsets[dir]
   ) {
      gid_out = MOBILE_GID_PLAYER;
   }

cleanup:

   if( NULL != mobiles ) {
      mobiles = memory_unlock( state->mobiles_handle );
   }

   return gid_out;
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

   script_trace_printf( 1, "mobile %u:%u pushed: %d",
      m->map_gid, m->spawner_gid, v );
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

   script_trace_printf( 1, "mobile %u:%u \"%s\" popped: %d",
      m->map_gid, m->spawner_gid, m->name, retval );

   return retval;
}

struct MOBILE* mobile_interact(
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP* t
) {
   struct SCRIPT* script = NULL;
   struct SCRIPT_STEP* step = NULL;

   /* Rule out invalid mobile or script. */
   if( 
      /* No interactee. */
      NULL == actee ||
      /* Inactive mobile slot. */
      (MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & actee->flags)) ||
      /* Dead interactee. */
      0 >= (actee->mp_hp & MOBILE_HP_MASK) ||
      /* No actee script. */
      0 > actee->script_id ||
      /* Actee interaction disabled. */
      (MOBILE_FLAG_DISABLED == (MOBILE_FLAG_DISABLED & actee->flags)) ||
      /* Invalid actee script. */
      actee->script_id >= TILEMAP_SCRIPTS_MAX ||
      /* Invalid actee script previous PC. */
      1 > actee->script_pc ||
      /* Invalid actee script PC. */
      SCRIPT_STEPS_MAX <= actee->script_pc
   ) {
      return NULL;
   }

   /* Grab the current script step to check for special cases. */
   script = &(t->scripts[actee->script_id]);
   step = &(script->steps[actee->script_pc]);

   /* Rule out special circumstances. */
   /* TODO: Is this a bit too severe for blocking ALL interaction during
    *       WALK?
    */
   if(
      /* Special case: Actor is executing a multi-cycle WALK instruction. */
      SCRIPT_ACTION_WALK == step->action ||
      /* Actee is walking. */
      actor->coords.y != actor->coords_prev.y ||
      /* Actee is walking. */
      actor->coords.x != actor->coords_prev.x
   ) {
      return NULL;
   }

   /* Grab the previous script step to check for special cases. */
   script = &(t->scripts[actee->script_id]);
   step = &(script->steps[actee->script_pc - 1]);

   /* Rule out special circumstances. */
   if(
      /* Special case: Previous step was a DISABLE 0. */
      (SCRIPT_ACTION_DISABLE == step->action && 0 == step->arg)
   ) {
      /* Inactive mobile or invalid script. */
      return NULL;
   }

   script_trace_printf( 1, "mobile %u:%u \"%s\" interacted at pc %d, icount %d",
      actee->map_gid, actee->spawner_gid, actee->name,
      actee->script_pc, mobile_get_icount( actee ) );

   /* Push actee previous PC for return. */
   mobile_stack_push( actee, actee->script_pc );

   /* Set actee's pc to the GOTO for interaction and make actee active NOW. */
   actee->script_pc = script_goto_label(
      actee->script_pc, &(t->scripts[actee->script_id]),
      SCRIPT_ACTION_INTERACT, mobile_get_icount( actee ) );
   actee->script_wait_frames = 0;

   /* Lock so that the actee can't be double-interacted with. */
   actee->flags |= MOBILE_FLAG_DISABLED;

   script_trace_printf( 1, "mobile %u:%u \"%s\" interaction disabled",
      actee->map_gid, actee->spawner_gid, actee->name );

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
      script_trace_printf( 1,
         "mobile %u:%u \"%s\" sleeping: waiting for %d more frames",
         m->map_gid, m->spawner_gid, m->name, m->script_wait_frames );
      /* Mobile still sleeping. */
      goto cleanup;
   }

   script = &(t->scripts[m->script_id]);
   step = &(script->steps[m->script_pc]);

   /* Arg processing. */
   if( SCRIPT_ARG_STACK_I == step->arg ) {
      /* Stack arg with interaction count. */
      arg = mobile_stack_pop( m );
      mobile_incr_icount( m, 1 );

   } else if( SCRIPT_ARG_STACK == step->arg ) {
      /* Stack arg. */
      arg = mobile_stack_pop( m );

   } else if( SCRIPT_ARG_STACK_P == step->arg ) {
      /* Stack arg w/ passthrough. */
      arg = mobile_stack_pop( m );
      push_arg_after = 1;

   } else if( SCRIPT_ARG_RANDOM == step->arg ) {
      /* Random arg. */
      arg = graphics_get_random( 0, SCRIPT_STACK_MAX );

   } else if( SCRIPT_ARG_FOLLOW == step->arg ) {
      /* Pathfinding dir arg. */
      arg = pathfind_start(
         m, state->player.coords.x, state->player.coords.y, 3, state, t,
         PATHFIND_FLAGS_TGT_OCCUPIED );
      if( 0 > arg ) {
         pathfind_trace_printf( 1, "mobile %u:%u \"%s\" pathfinding blocked",
            m->map_gid, m->spawner_gid, m->name );
         goto cleanup;
      }
      script_trace_printf( 0, "follow arg became %d", arg );
   } else {
      /* Literal/immediate arg. */
      arg = step->arg;
   }

   script_trace_printf(
      1, "mobile %u:%u \"%s\" script_exec: script %d, step %d (%d)",
      m->map_gid, m->spawner_gid, m->name,
      m->script_id, m->script_pc, step->action );

   /* Execute script action callback. */
   m->script_pc = gc_script_handlers[step->action](
      m->script_pc, script, t, m, NULL, &(m->coords), state, arg );

   /* Complete stack arg passthrough. */
   if( push_arg_after ) {
      mobile_stack_push( m, arg );
   }

cleanup:
   if( NULL != t ) {
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }
}

void mobile_deactivate( struct MOBILE* m, struct DSEKAI_STATE* state ) {
   
   debug_printf( 1, "de-initializing mobile tilemap: %d spawner: %d",
      m->map_gid, m->spawner_gid );

   /* Zero out the mobile to avoid weirdness later, but keep
    * MOBILE_FLAG_NOT_LAST flag. */
   memory_zero_ptr( (MEMORY_PTR)m, sizeof( struct MOBILE ) );
   m->flags |= MOBILE_FLAG_NOT_LAST;
}

void mobile_animate( struct MOBILE* m, struct DSEKAI_STATE* state ) {
   struct TILEMAP* t = NULL;

   assert( SPRITE_W > m->steps_remaining && SPRITE_H > m->steps_remaining );

#ifdef NO_SMOOTH_WALK
   /* We're taking our single "step." */
   m->steps_remaining = 0;
#else
   /* If the mobile is walking, advance its steps. */
   m->steps_remaining = gc_mobile_step_table_normal_pos[m->steps_remaining];
#endif /* NO_SMOOTH_WALK */

   assert( SPRITE_W > m->steps_remaining && SPRITE_H > m->steps_remaining );

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
      if( 0 == m->steps_remaining ) {
         m->coords_prev.x = m->coords.x;
         m->coords_prev.y = m->coords.y;
      }
   }

   /* Use negative HP for blinking effect on death, or remove if we're up to 
    * -1.
    */
   if( -1 == m->mp_hp ) {
      mobile_deactivate( m, state );
   } else if( 0 > m->mp_hp ) {
      m->mp_hp++;
   }
}

struct MOBILE* mobile_spawn_single(
   uint16_t flags, struct DSEKAI_STATE* state, struct MOBILE* mobiles,
   int16_t mobiles_sz
) {
   int16_t i = 0;
   struct MOBILE* mobile_out = NULL;

   if( MOBILE_FLAG_PLAYER == (MOBILE_FLAG_PLAYER & flags) ) {
      /* Check to see if player already exists. */
      if(
         MOBILE_FLAG_ACTIVE == (MOBILE_FLAG_ACTIVE & state->player.flags)
      ) {
         error_printf( "player mobile already in use!" );
         /* Skip setup. */
         goto cleanup;
      }
      
      /* Player doesn't exist, so assign it and config below. */
      mobile_out = &(state->player);
   } else {
      for( i = 0 ; mobiles_sz > i ; i++ ) {
         if( mobile_is_active( &(mobiles[i]) ) ) {
            continue;
         }

         mobile_out = &(mobiles[i]);
         break;
      }

      if( i >= mobiles_sz ) {
         /* TODO: Resize mobiles. */
         error_printf( "no available mobiles to spawn!" );
         goto cleanup;
      }
   }

   mobile_out->mp_hp = 100;
   mobile_out->steps_remaining = 0;
   mobile_out->script_pc = 0;
   mobile_out->script_wait_frames = 0;
   if( MOBILE_FLAG_NOT_LAST == (MOBILE_FLAG_NOT_LAST & mobile_out->flags) ) {
      debug_printf( 1, "reusing non-last mobile slot %d", i );
   }
   mobile_out->flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_out->map_gid = MOBILE_MAP_GID_ALL;
   
cleanup:

   return mobile_out;
}

int16_t mobile_spawner_match(
   struct TILEMAP_SPAWN* spawner, struct TILEMAP* t, struct MOBILE* mobiles,
   uint16_t mobiles_sz
) {
   int16_t i = 0;
   RESOURCE_ID ss_id;

   /* See if the mobile was spawned on a previous visit to this tilemap. */
   debug_printf( 1, "search tilemap %d, spawner %d", t->gid, spawner->gid );
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      /* mobile_break_if_last( mobiles, i ); */
      if( MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & mobiles[i].flags) ) {
         continue;
      }
      debug_printf( 0, "sgid m: %u vs s: %u",
         mobiles[i].spawner_gid, spawner->gid );
      debug_printf( 0, "mgid m: %u vs s: %u",
         mobiles[i].map_gid, t->gid );
      if(
         MOBILE_FLAG_ACTIVE == (MOBILE_FLAG_ACTIVE & mobiles[i].flags) &&
         mobiles[i].spawner_gid == spawner->gid &&
         mobiles[i].map_gid == t->gid
      ) {
         /* This mobile was spawned already, so update volatile stuff
            * and move on.
            */
         resource_id_from_name( &ss_id, spawner->sprite_name,
            RESOURCE_EXT_GRAPHICS );
         mobiles[i].sprite_cache_id = graphics_cache_load_bitmap(
            ss_id, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
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
   struct MOBILE* mobiles = NULL;
   struct MOBILE* mobile_iter = NULL;
   RESOURCE_ID sprite_id;

   mobiles = memory_lock( state->mobiles_handle );
   if( NULL == mobiles ) {
      error_printf( "could not lock mobiles handle" );
      goto cleanup;
   }

   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      /* If the spawner has no name, skip it. */
      if( 0 == memory_strnlen_ptr(
         t->spawns[i].name, TILEMAP_SPAWN_NAME_SZ )
      ) {
         continue;
      }

      /* See if the mobile was spawned on a previous visit to this tilemap. */
      match = mobile_spawner_match(
         &(t->spawns[i]), t, mobiles, state->mobiles_sz );
      if( 0 <= match ) {
         debug_printf( 1,
            "found existing spawn for tilemap %u spawner %u; skipping",
            t->gid, t->spawns[i].gid );
         continue;
      }

      /* Select the mobile slot or player slot to spawn to. */
      mobile_iter = mobile_spawn_single( t->spawns[i].flags, state,
         mobiles, state->mobiles_sz );
      if( NULL == mobile_iter ) {
         error_printf( "coult not spawn mobile for tilemap %u spawner %u",
            t->gid, t->spawns[i].gid );
         continue;
      }

      debug_printf( 1, "spawning mobile for tilemap %u spawner %u",
         t->gid, t->spawns[i].gid );

      /* Assign the rest of the properties from the spawner. */
      mobile_iter->coords.x = t->spawns[i].coords.x;
      mobile_iter->coords.y = t->spawns[i].coords.y;
      mobile_iter->coords_prev.x = t->spawns[i].coords.x;
      mobile_iter->coords_prev.y = t->spawns[i].coords.y;
      mobile_iter->script_id = t->spawns[i].script_id;
      mobile_iter->ascii = t->spawns[i].ascii;
      mobile_iter->flags |= t->spawns[i].flags;
      mobile_iter->spawner_gid = t->spawns[i].gid;
      if( MOBILE_FLAG_PLAYER != (t->spawns[i].flags & MOBILE_FLAG_PLAYER) ) {
         /* The player is on all tilemaps, but other mobiles limited to one. */
         mobile_iter->map_gid = t->gid;
      } else {
         /* Save player sprite for tilemap transitions. */
         memory_strncpy_ptr(
            state->player_sprite_name, t->spawns[i].sprite_name,
            RESOURCE_NAME_MAX );
      }
      resource_id_from_name( &sprite_id, t->spawns[i].sprite_name,
         RESOURCE_EXT_GRAPHICS );
      mobile_iter->sprite_cache_id = graphics_cache_load_bitmap( 
         sprite_id, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   }

cleanup:

   return;
}

