
#define MOBILE_C
#include "dsekai.h"

uint8_t mobile_walk_start( struct MOBILE* m, uint8_t dir ) {

   if( mobile_is_walking( m ) ) {
      /* We're already walking! */
      return 0;
   }

   m->coords[1].x += gc_mobile_x_offsets[dir];
   m->coords[1].y += gc_mobile_y_offsets[dir];
#ifdef NO_SMOOTH_WALK
   m->steps_remaining = 0; /* Just take one "step" on next tick. */
#else
   /* Use lookup table to determine first step size. */
   m->steps_remaining = gc_mobile_step_table_normal_pos[SPRITE_W - 1];
#endif /* NO_SMOOTH_WALK */

   assert( SPRITE_W > m->steps_remaining && SPRITE_H > m->steps_remaining );

   return 1;
}

struct MOBILE* mobile_get_facing(
   uint8_t x, uint8_t y, uint8_t dir, struct DSEKAI_STATE* state
) {
   int16_t i = 0;
   struct MOBILE* mobile_out = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;
   }

   /* TODO: Implement resource gathering here, maybe? */

   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      /* TODO: Compact mobiles list, maybe on zone? */
      /* mobile_break_if_last( state->mobiles, i ); */

      if(
         mobile_is_active( &(state->mobiles[i]) ) &&
         state->mobiles[i].map_gid == state->tilemap->gid &&
         state->mobiles[i].coords[1].x == x + gc_mobile_x_offsets[dir] &&
         state->mobiles[i].coords[1].y == y + gc_mobile_y_offsets[dir]
      ) {
         /* Found an active facing mobile on the same tilemap. */
         mobile_out = &(state->mobiles[i]);
         goto cleanup;
      }
   }

   /* This mobile is facing the player. */
   if(
      state->player.coords[1].x == x + gc_mobile_x_offsets[dir] &&
      state->player.coords[1].y == y + gc_mobile_y_offsets[dir]
   ) {
      mobile_out = &(state->player);
   }

cleanup:

   return mobile_out;
}

struct MOBILE* mobile_from_gid( MOBILE_GID m_gid, struct DSEKAI_STATE* state ) {
   int16_t i = 0;
   struct MOBILE* mobile_out = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;
   }

   /* Special case: NULL GID */
   if( MOBILE_GID_NONE == m_gid ) {
      goto cleanup;
   }

   /* Special case: Player GID */
   if( MOBILE_GID_PLAYER == m_gid ) {
      mobile_out = &(state->player);
      goto cleanup;
   }

   /* Hunt for the GID manually. */
   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      if( mobile_get_gid( &(state->mobiles[i]) ) == m_gid ) {
         mobile_out = &(state->mobiles[i]);
         goto cleanup;
      }
   }
   
cleanup:

   return mobile_out;
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
   struct MOBILE* actor, struct MOBILE* actee, struct DSEKAI_STATE* state
) {
   struct SCRIPT* script = NULL;
   struct SCRIPT_STEP* step = NULL;

   if( !engines_state_lock( state ) ) {
      actee = NULL;
      goto cleanup;
   }

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
      actee = NULL;
      goto cleanup;
   }

   /* Grab the current script step to check for special cases. */
   script = &(state->tilemap->scripts[actee->script_id]);
   step = &(script->steps[actee->script_pc]);

   /* Rule out special circumstances. */
   /* TODO: Is this a bit too severe for blocking ALL interaction during
    *       WALK?
    */
   /* Special case: Actor is executing a multi-cycle WALK instruction. */
   if( SCRIPT_ACTION_WALK == step->action || mobile_is_walking( actor ) ) {
      actee = NULL;
      goto cleanup;
   }

   /* Grab the previous script step to check for special cases. */
   script = &(state->tilemap->scripts[actee->script_id]);
   step = &(script->steps[actee->script_pc - 1]);

   /* Rule out special circumstances. */
   if(
      /* Special case: Previous step was a DISABLE 0. */
      (SCRIPT_ACTION_DISABLE == step->action && 0 == step->arg)
   ) {
      /* Inactive mobile or invalid script. */
      actee = NULL;
      goto cleanup;
   }

   script_trace_printf( 1, "mobile %u:%u \"%s\" interacted at pc %d, icount %d",
      actee->map_gid, actee->spawner_gid, actee->name,
      actee->script_pc, mobile_get_icount( actee ) );

   /* Push actee previous PC for return. */
   mobile_stack_push( actee, actee->script_pc );

   /* Set actee's pc to the GOTO for interaction and make actee active NOW. */
   actee->script_pc = script_goto_label(
      actee->script_pc, &(state->tilemap->scripts[actee->script_id]),
      SCRIPT_ACTION_INTERACT, mobile_get_icount( actee ) );
   actee->script_wait_frames = 0;

   /* Lock so that the actee can't be double-interacted with. */
   actee->flags |= MOBILE_FLAG_DISABLED;

   script_trace_printf( 1, "mobile %u:%u \"%s\" interaction disabled",
      actee->map_gid, actee->spawner_gid, actee->name );

cleanup:

   return actee;
}

void mobile_execute( struct MOBILE* m, struct DSEKAI_STATE* state ) {
   struct SCRIPT* script = NULL;
   struct SCRIPT_STEP* step = NULL;
   int16_t arg = 0;
   uint8_t push_arg_after = 0;
   struct TILEMAP* t = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Check if we should execute. */
   if(
      (MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & m->flags)) ||
      m->map_gid != state->tilemap->gid ||
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

   script = &(state->tilemap->scripts[m->script_id]);
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
         m, state->player.coords[1].x, state->player.coords[1].y, 3,
         PATHFIND_FLAGS_TGT_OCCUPIED, state );
      if( 0 > arg ) {
         pathfind_trace_printf( 1,
            "mobile " MOBILE_GID_FMT " pathfinding blocked",
            mobile_get_gid_fmt( m )  );
         goto cleanup;
      }
      script_trace_printf( 0, "follow arg became %d", arg );
   } else {
      /* Literal/immediate arg. */
      arg = step->arg;
   }

   script_trace_printf(
      1, "mobile " MOBILE_GID_FMT " script_exec: script %d, step %d (%d)",
      mobile_get_gid_fmt( m ),
      m->script_id, m->script_pc, step->action );

   /* Execute script action callback. */
   m->script_pc = gc_script_handlers[step->action](
      m->script_pc, arg, script, m, NULL, &(m->coords[1]), state );

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

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   assert( SPRITE_W > m->steps_remaining && SPRITE_H > m->steps_remaining );

#ifdef NO_SMOOTH_WALK
   /* We're taking our single "step." */
   m->steps_remaining = 0;
#else
   /* If the mobile is walking, advance its steps. */
   m->steps_remaining = gc_mobile_step_table_normal_pos[m->steps_remaining];
#endif /* NO_SMOOTH_WALK */

   assert( SPRITE_W > m->steps_remaining && SPRITE_H > m->steps_remaining );

   /* Leave a trail of dirty tiles. */
   state->tilemap->tiles_flags[
      (mobile_get_ty( m ) * TILEMAP_TW) + mobile_get_tx( m )] |=
         TILEMAP_TILE_FLAG_DIRTY;
   state->tilemap->tiles_flags[
      (m->coords[0].y * TILEMAP_TW) + m->coords[0].x] |=
         TILEMAP_TILE_FLAG_DIRTY;

   /* Sync up prev and current coords if no steps left. */
   /* TODO: Implement queue, bring down next steps. */
   if( mobile_is_walking( m ) &&  0 == m->steps_remaining ) {
      m->coords[0].x = mobile_get_tx( m );
      m->coords[0].y = mobile_get_ty( m );
   }

   /* Use negative HP for blinking effect on death, or remove if we're up to 
    * -1.
    */
   if( -1 == m->mp_hp ) {
      mobile_deactivate( m, state );
   } else if( 0 > m->mp_hp ) {
      m->mp_hp++;
   }

cleanup:

   return;
}

struct MOBILE* mobile_spawn_single(
   uint16_t flags, struct DSEKAI_STATE* state
) {
   int16_t i = 0;
   struct MOBILE* mobile_out = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

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
      for( i = 0 ; state->mobiles_sz > i ; i++ ) {
         if( mobile_is_active( &(state->mobiles[i]) ) ) {
            continue;
         }

         mobile_out = &(state->mobiles[i]);
         break;
      }

      if( i >= state->mobiles_sz ) {
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
   struct TILEMAP_SPAWN* spawner, struct DSEKAI_STATE* state
) {
   int16_t i = -1;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* See if the mobile was spawned on a previous visit to this tilemap. */
   debug_printf( 1, "search tilemap %d, spawner %d",
      state->tilemap->gid, spawner->gid );
   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      /* mobile_break_if_last( mobiles, i ); */
      if( !mobile_is_active( &(state->mobiles[i]) ) ) {
         continue;
      }
      debug_printf( 0, "sgid m: %u vs s: %u",
         state->mobiles[i].spawner_gid, spawner->gid );
      debug_printf( 0, "mgid m: %u vs s: %u",
         state->mobiles[i].map_gid, state->tilemap->gid );
      if(
         mobile_is_active( &(state->mobiles[i]) ) &&
         state->mobiles[i].spawner_gid == spawner->gid &&
         state->mobiles[i].map_gid == state->tilemap->gid
      ) {
         /* This mobile was spawned already, so update volatile stuff
            * and move on.
            */
         debug_printf( 1, "found already spawned mobile " MOBILE_GID_FMT,
            mobile_get_gid_fmt( &(state->mobiles[i]) ) );
         goto cleanup;
      }
   }

   /* Reset back to not-found condition if cleanup above not triggered. */
   i = -1;

cleanup:

   return i;
}

void mobile_spawns( struct DSEKAI_STATE* state ) {
   int16_t i = 0,
      match = 0;
   struct MOBILE* mobile_iter = NULL;
   RESOURCE_ID sprite_id;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      /* If the spawner has no name, skip it. */
      if( 0 == memory_strnlen_ptr(
         state->tilemap->spawns[i].name, TILEMAP_SPAWN_NAME_SZ )
      ) {
         continue;
      }

      /* See if the mobile was spawned on a previous visit to this tilemap. */
      match = mobile_spawner_match( &(state->tilemap->spawns[i]), state );
      if( 0 <= match ) {
         debug_printf( 1,
            "found existing spawn for tilemap %u spawner %u; skipping",
            state->tilemap->gid, state->tilemap->spawns[i].gid );
         continue;
      }

      /* Select the mobile slot or player slot to spawn to. */
      mobile_iter = mobile_spawn_single(
         state->tilemap->spawns[i].flags, state );
      if( NULL == mobile_iter ) {
         error_printf( "coult not spawn mobile for tilemap %u spawner %u",
            state->tilemap->gid, state->tilemap->spawns[i].gid );
         continue;
      }

      debug_printf( 1, "spawning mobile for tilemap %u spawner %u",
         state->tilemap->gid, state->tilemap->spawns[i].gid );

      /* Assign the rest of the properties from the spawner. */
      mobile_iter->coords[1].x = state->tilemap->spawns[i].coords.x;
      mobile_iter->coords[1].y = state->tilemap->spawns[i].coords.y;
      mobile_iter->coords[0].x = state->tilemap->spawns[i].coords.x;
      mobile_iter->coords[0].y = state->tilemap->spawns[i].coords.y;
      mobile_iter->script_id = state->tilemap->spawns[i].script_id;
      mobile_iter->ascii = state->tilemap->spawns[i].ascii;
      mobile_iter->flags |= state->tilemap->spawns[i].flags;
      mobile_iter->spawner_gid = state->tilemap->spawns[i].gid;
      memory_strncpy_ptr(
         mobile_iter->name, state->tilemap->spawns[i].name,
         TILEMAP_SPAWN_NAME_SZ );
      memory_strncpy_ptr(
         mobile_iter->sprite_name, state->tilemap->spawns[i].sprite_name,
         RESOURCE_NAME_MAX );
      if( MOBILE_FLAG_PLAYER != (state->tilemap->spawns[i].flags & MOBILE_FLAG_PLAYER) ) {
         /* The player is on all tilemaps, but other mobiles limited to one. */
         mobile_iter->map_gid = state->tilemap->gid;
      } else {
         /* Save player sprite for tilemap transitions. */
         memory_strncpy_ptr(
            state->player_sprite_name, state->tilemap->spawns[i].sprite_name,
            RESOURCE_NAME_MAX );
      }
      resource_id_from_name( &sprite_id, state->tilemap->spawns[i].sprite_name,
         RESOURCE_EXT_GRAPHICS );
      mobile_iter->sprite_cache_id = graphics_cache_load_bitmap( 
         sprite_id, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   }

cleanup:

   return;
}

