
#define SCRIPT_C
#include "dsekai.h"

/* For time and rand. */
#include <stdlib.h>

#if defined( SCREEN_W ) && defined( SCREEN_H )
/* We're running in the engine and not an external tool. */
#define SCRIPT_HAS_GFX
#endif /* SCREEN_W && SCREEN_H */

#ifndef NO_SCRIPT_HANDLERS

uint16_t script_handle_NOOP(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   return pc;
}

uint16_t script_goto_label(
   uint16_t pc, struct SCRIPT* script, uint16_t label_type, uint16_t label_id
) {
   uint16_t i = 0;

   for( i = 0 ; script->steps_count > i ; i++ ) {
      if(
         label_type == script->steps[i].action &&
         label_id == script->steps[i].arg
      ) {
         /* Jump to this START instruction's position. */
         script_trace_printf( 0, "jump to type %d, #%d, pc %d",
            label_type, label_id, i );
         return i;
      }
   }

   /* Freeze. */
   error_printf( "invalid goto label type %d, #%d", label_type, label_id );
   return pc;
}

uint16_t script_handle_SHAKE(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
#ifndef NO_GUI
   g_screen_flags |= (GRAPHICS_FLAG_SHAKING_MASK & arg);
#endif /* !NO_GUI */
   return pc + 1;
}

uint16_t script_handle_INTERACT(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   /* NOOP: This is just a label to jump to with fancy GOTO mechanism. */
   return pc + 1;
}

uint16_t script_handle_WALK(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   int8_t dir_raw = 0,
      dir = 0;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   if(
      /* Actor is currently walking. */
      (actor->coords[1].y != actor->coords[0].y ||
         actor->coords[1].x != actor->coords[0].x)
#ifndef NO_SMOOTH_WALK
      /* Actor will be done walking on next tick. */
      && (0 == gc_mobile_step_table_normal_pos[actor->steps_remaining])
#endif /* !NO_SMOOTH_WALK */
   ) {
      script_trace_printf( 1, "mobile %u:%u done walking",
         actor->map_gid, actor->spawner_gid );

      /* Pop the dir, since the walk is complete. */
      mobile_stack_pop( actor );

      /* PC only incremends when walk is complete, so that this check
       * continues until next step.
       */
      return pc + 1;

   } else if(
      actor->coords[1].y != actor->coords[0].y ||
      actor->coords[1].x != actor->coords[0].x
   ) {
      /* Actor is already walking, don't start or advance PC. */   
      script_trace_printf( 1, "mobile %u:%u already walking",
         actor->map_gid, actor->spawner_gid );

   } else {
      /* Start trying to walk. */
      dir_raw = mobile_stack_pop( actor );
      dir = dir_raw % 4;

      script_trace_printf( 1, "mobile %u:%u popped %d, became %d",
         actor->map_gid, actor->spawner_gid, dir_raw, dir );

      /* Handle terrain blockage by skipping. */
      if(
         TILEMAP_ERROR_BLOCKED ==
         tilemap_collide(
            actor->coords[1].x, actor->coords[1].y, dir, state->tilemap )
      ) {
         /* Actor would collide, so just skip this walk attempt. */
         script_trace_printf( 1, "mobile %u:%u collided with terrain",
            actor->map_gid, actor->spawner_gid );
         return pc + 1;
      }

      /* If actor would collide with a mobile, don't try to walk, but keep
       * the PC the same and push the dir back to retry later.
       */
      if(
         NULL == mobile_get_facing(
            actor->coords[1].x, actor->coords[1].y, mobile_get_dir( actor ), state )
      ) {
         script_trace_printf( 1, "mobile %u:%u starting walking in dir %d",
            actor->map_gid, actor->spawner_gid, dir );
         mobile_walk_start( actor, dir );
      }

      /* Push the dir back until walk is complete. */
      mobile_stack_push( actor, dir_raw );
   }

cleanup:

   return pc;
}

uint16_t script_handle_SLEEP(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   actor->script_wait_frames = arg;
   return pc + 1;
}

uint16_t script_handle_START(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   /* NOOP: This is just a label to jump to with GOTO mechanism. */
   return pc + 1;
}

uint16_t script_handle_GOTO(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   mobile_stack_push( actor, pc + 1 );
   return script_goto_label( pc, script, SCRIPT_ACTION_START, arg );
}

uint16_t script_handle_RETURN(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   if( 0 <= arg && SCRIPT_STEPS_MAX > arg ) {
      script_trace_printf( 1, "mobile %u:%u at pc %d, returning to pc %d",
         actor->map_gid, actor->spawner_gid, pc, arg );
      return arg;
   }
   /* Freeze. */
   error_printf(
      "mobile %u:%u script frozen at pc %d: invalid return pc %d",
      actor->map_gid, actor->spawner_gid,
      pc, arg );
   return pc;
}

uint16_t script_handle_GGET(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   if( arg >= 0 && SCRIPT_GLOBALS_MAX > arg ) {
      mobile_stack_push( actor, g_script_globals[arg] );
   }
   return pc + 1;
}

uint16_t script_handle_GSET(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   if( arg >= 0 && SCRIPT_GLOBALS_MAX > arg ) {
      g_script_globals[arg] = mobile_stack_pop( actor );
   }
   return pc + 1;
}

uint16_t script_handle_SPEAK(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {

#if defined( DEPTH_VGA ) || defined( DEPTH_CGA ) || defined( DEPTH_MONO )
#ifndef NO_GUI
   const char* str_ptr = NULL;
   uint8_t dir_flag = 0;
   
   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   str_ptr = strpool_get( state->tilemap->strpool, arg, NULL );

   /* Translate mobile dir flag into window dir flag. */
   dir_flag = mobile_get_dir( actor );
   dir_flag <<= 4;

   window_prefab_dialog(
      str_ptr, mobile_get_sprite( actor ), dir_flag,
      WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );

cleanup:
#endif /* !NO_GUI */
#endif /* SCRIPT_HAS_GFX */

   return pc + 1;
}

uint16_t script_handle_FACE(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   mobile_set_dir( actor, (arg % 4) );
   return pc + 1;
}

uint16_t script_handle_WARP(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   const char* warp_map = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   warp_map = strpool_get( state->tilemap->strpool, arg, NULL );

   if( NULL == warp_map ) {
      error_printf( "invalid warp map string index: %d", arg );
      return pc;
   }

   script_trace_printf( 0, "mobile %u:%u warp requested to: %s", 
      actor->map_gid, actor->spawner_gid, warp_map );

   memory_zero_ptr( (MEMORY_PTR)(state->warp_to), TILEMAP_NAME_MAX );
   memory_strncpy_ptr( state->warp_to, warp_map, TILEMAP_NAME_MAX );

   state->warp_to_y = mobile_stack_pop( actor );
   state->warp_to_x = mobile_stack_pop( actor );

cleanup:

   /* Freeze */
   return pc;
}

uint16_t script_handle_ANIM(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
#ifdef SCRIPT_HAS_GFX
#  ifndef NO_ANIMATE
   int16_t a_x = 0,
      a_y = 0,
      a_w = 0,
      a_h = 0;

   a_h = mobile_stack_pop( actor ) * TILE_H;
   a_w = mobile_stack_pop( actor ) * TILE_W;
   a_y = mobile_stack_pop( actor ) * TILE_H;
   a_x = mobile_stack_pop( actor ) * TILE_W;

   animate_create(
      arg, ANIMATE_FLAG_SCRIPT | ANIMATE_FLAG_FG,
      SCREEN_MAP_X + a_x, SCREEN_MAP_Y + a_y, a_w, a_h
   );
#  endif /* !NO_ANIMATE */
#endif /* SCRIPT_HAS_GFX */

   return pc + 1;
}

uint16_t script_handle_PUSH(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   mobile_stack_push( actor, arg );

   return pc + 1;
}

uint16_t script_handle_POP(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   mobile_stack_pop( actor );

   return pc + 1;
}

uint16_t script_handle_EQJMP(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   int8_t val1 = 0,
      val2 = 0;

   val1 = mobile_stack_pop( actor );
   val2 = mobile_stack_pop( actor );

   if( val1 == val2 ) {
      /* TRUE */
      mobile_stack_push( actor, pc + 1 );
      return script_goto_label( pc, script, SCRIPT_ACTION_START, arg );
   } else {
      /* FALSE */
      return pc + 1;
   }
}

uint16_t script_handle_GTJMP(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   int8_t val1 = 0,
      val2 = 0;

   val2 = mobile_stack_pop( actor );
   val1 = mobile_stack_pop( actor );

   mobile_stack_push( actor, pc + 1 );

   if( val1 > val2 ) {
      /* TRUE */
      mobile_stack_push( actor, pc + 1 );
      return script_goto_label( pc, script, SCRIPT_ACTION_START, arg );
   } else {
      /* FALSE */
      return pc + 1;
   }
}

uint16_t script_handle_ADD(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   int8_t stack_val = 0;

   stack_val = mobile_stack_pop( actor );

   mobile_stack_push( actor, stack_val + arg );

   return pc + 1;
}

uint16_t script_handle_SUB(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   int8_t stack_val = 0;

   stack_val = mobile_stack_pop( actor );

   mobile_stack_push( actor, stack_val - arg );

   return pc + 1;
}

uint16_t script_handle_LTJMP(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   int8_t val1 = 0,
      val2 = 0;

   val1 = mobile_stack_pop( actor );
   val2 = mobile_stack_pop( actor );

   if( val1 < val2 ) {
      /* TRUE */
      mobile_stack_push( actor, pc + 1 );
      return script_goto_label( pc, script, SCRIPT_ACTION_START, arg );
   } else {
      /* FALSE */
      return pc + 1;
   }
}

uint16_t script_handle_GIVE(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   int16_t e_idx = 0;
   int8_t items_pushed = 1;

   /* TODO: Split out command to add item to this entity's inventory,
      *       then this "give" can be finite/fail.
      */
   /* TODO: Rework hook API to pass actor/actee IDs so we can pass that
      *       as the owner to item_create_from_template().
      */
   e_idx = item_stack_or_add( arg, 0, state );
   if( 0 > e_idx ) {
      error_printf( "unable to give: invalid item GID: %d", arg );
      items_pushed = 0;
   }

   e_idx = item_give_mobile( e_idx, ITEM_OWNER_PLAYER, state );
   if( 0 > e_idx ) {
      error_printf( "unable to give: error %d", e_idx );
      items_pushed = 0;
   }

   /* Push number given onto the stack. */
   mobile_stack_push( actor, items_pushed );

   return pc + 1;
}

uint16_t script_handle_TAKE(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   /* TODO: Push onto the stack how many taken. */
   mobile_stack_push( actor, 0 );

   return pc + 1;
}

uint16_t script_handle_DIE(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   /* Trigger pre-death blinking effect. */
   actor->mp_hp = -10;

   return pc + 1;
}

uint16_t script_handle_DISABLE(
   uint16_t pc, int16_t arg, struct SCRIPT* script,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state
) {
   if( 0 == arg ) {
      script_trace_printf( 1, "mobile %u:%u enabling interaction",
         actor->map_gid, actor->spawner_gid );
      actor->flags &= ~MOBILE_FLAG_DISABLED;
   } else {
      script_trace_printf( 1, "mobile %u:%u disabling interaction",
         actor->map_gid, actor->spawner_gid );
      actor->flags |= MOBILE_FLAG_DISABLED;
   }
   return pc + 1;
}

#endif /* NO_SCRIPT_HANDLERS */

#if 0

#define SCRIPT_CB_TABLE_PARSE( idx, name, c ) case c: script->steps[script->steps_count].action = idx; c_idx++; break;

uint16_t script_parse_str(
   int8_t script_idx,
   char* script_txt, int16_t script_txt_sz, struct SCRIPT* script
) {
   int16_t c_idx = 0;

   /* TODO: Link compiler directly to JSON parser and ultimately remove this
    *       function entirely!
    */

   debug_printf( 2, "parsing script %d: %s", script_idx, script_txt );

   for( ; script_txt_sz > c_idx ; ) {
      debug_printf( 1, "step %d char: %c",
         script->steps_count, script_txt[c_idx] );
      switch( script_txt[c_idx] ) {
         SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_PARSE )
         default:
            debug_printf( 2, "unknown script char: %c", script_txt[c_idx] );
            break;
      }
      if( c_idx >= script_txt_sz ) {
         break;
      }

      /* TODO: Skip missing args. */
      script->steps[script->steps_count].arg =
         dio_atoi( &(script_txt[c_idx]), 10 );
      debug_printf(
         2, "script: %d, step: %d, action: %d, arg: %d",
         script_idx,
         script->steps_count,
         script->steps[script->steps_count].action,
         script->steps[script->steps_count].arg );
      if( 10000 <= script->steps[script->steps_count].arg ) {
         c_idx += 5;
      } else if( 1000 <= script->steps[script->steps_count].arg ) {
         c_idx += 4;
      } else if( 100 <= script->steps[script->steps_count].arg ) {
         c_idx += 3;
      } else if( 10 <= script->steps[script->steps_count].arg ) {
         c_idx += 2;
      } else {
         c_idx++;
      }

      script->steps_count++;
   }

   return script->steps_count;
}

#endif

uint8_t script_init() {
   memory_zero_ptr( (MEMORY_PTR)&g_script_globals, sizeof( g_script_globals ) );
   return 1;
}

void script_shutdown() {

}

