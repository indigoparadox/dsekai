
#define SCRIPT_C
#include "dsekai.h"

/* For time and rand. */
#include <stdlib.h>

#if defined( SCREEN_W ) && defined( SCREEN_H )
/* We're running in the engine and not an external tool. */
#define SCRIPT_HAS_GFX
#endif /* SCREEN_W && SCREEN_H */

uint16_t script_handle_NOOP(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
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
         debug_printf( 0, "jump to type %d, #%d, pc %d",
            label_type, label_id, i );
         return i;
      }
   }

   /* Freeze. */
   error_printf( "invalid goto label type %d, #%d", label_type, label_id );
   return pc;
}

uint16_t script_handle_INTERACT(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   /* NOOP: This is just a label to jump to with fancy GOTO mechanism. */
   return pc + 1;
}

static uint16_t script_handle_WALK_generic(
   uint16_t pc, struct MOBILE* actor, struct TILEMAP* t, uint8_t dir
) {
   /* TODO: Handle blockage. */

   if(
      /* Actor is currently walking. */
      (actor->coords.y != actor->coords_prev.y ||
         actor->coords.x != actor->coords_prev.x) &&
      /* Actor will be done walking on next tick. */
      (0 == gc_mobile_step_table_normal_pos[actor->steps_x] &&
         0 == gc_mobile_step_table_normal_pos[actor->steps_y])
   ) {
      debug_printf( 1, "scripted mobile done walking" );
      /* PC only incremends when walk is complete, so that this check
       * continues until next step.
       */
      return pc + 1;

   } else if(
      actor->coords.y != actor->coords_prev.y ||
      actor->coords.x != actor->coords_prev.x
   ) {
      /* Actor is already walking, don't start or advance PC. */   

   } else if(
      tilemap_collide( actor, dir, t )
      /* TODO: Need access to mobiles list. */
      /* ||
      mobile_collide(
         &(mobiles[state->player_idx]),
         MOBILE_DIR_EAST, mobiles, state->mobiles_count ) */
   ) {
      /* Actor would collide. */

   } else {
      debug_printf( 1, "scripted mobile starting walking" );
      /* actor->dir = dir; */
      mobile_walk_start( actor, dir );
   }

   return pc;
}

uint16_t script_handle_WALK_NORTH(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   return script_handle_WALK_generic( pc, actor, t, MOBILE_DIR_NORTH );
}

uint16_t script_handle_WALK_SOUTH(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   return script_handle_WALK_generic( pc, actor, t, MOBILE_DIR_SOUTH );
}

uint16_t script_handle_WALK_EAST(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   debug_printf( 0, "script: walk east" );
   return script_handle_WALK_generic( pc, actor, t, MOBILE_DIR_EAST );
}

uint16_t script_handle_WALK_WEST(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   debug_printf( 0, "script: walk west" );
   return script_handle_WALK_generic( pc, actor, t, MOBILE_DIR_WEST );
}

uint16_t script_handle_SLEEP(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   actor->script_next_ms = graphics_get_ms() + (arg * 1000);
   return pc + 1;
}

uint16_t script_handle_START(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   /* NOOP: This is just a label to jump to with GOTO mechanism. */
   return pc + 1;
}

uint16_t script_handle_GOTO(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   mobile_stack_push( actor, pc );
   return script_goto_label( pc, script, SCRIPT_ACTION_START, arg );
}

uint16_t script_handle_RETURN(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   if( 0 <= arg && SCRIPT_STEPS_MAX > arg ) {
      return arg;
   }
   /* Freeze. */
   error_printf( "script frozen due to invalid return pc" );
   return pc;
}

uint16_t script_handle_GLOBAL_GET(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   if( arg >= 0 && SCRIPT_GLOBALS_MAX > arg ) {
      mobile_stack_push( actor, g_script_globals[arg] );
   }
   return pc + 1;
}

uint16_t script_handle_GLOBAL_SET(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   if( arg >= 0 && SCRIPT_GLOBALS_MAX > arg ) {
      g_script_globals[arg] = mobile_stack_pop( actor );
   }
   return pc + 1;
}

uint16_t script_handle_SPEAK(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
#ifdef DEPTH_VGA
   window_prefab_dialog( WINDOW_ID_SCRIPT_SPEAK, arg, actor->sprite, state,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_MAGENTA );
#elif defined( DEPTH_CGA )
   window_prefab_dialog( WINDOW_ID_SCRIPT_SPEAK, arg, actor->sprite, state,
      GRAPHICS_COLOR_BLACK, GRAPHICS_COLOR_MAGENTA );
#elif defined( DEPTH_MONO )
   /* TODO: Verify this looks OK. */
   window_prefab_dialog( WINDOW_ID_SCRIPT_SPEAK, arg, actor->sprite, state,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_MAGENTA );
#endif /* SCRIPT_HAS_GFX */
   return pc + 1;
}

uint16_t script_handle_FACE(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   actor->dir = arg;
   return pc + 1;
}

uint16_t script_handle_WARP(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   const char* warp_map = NULL;

   warp_map = strpool_get( t->strpool, arg, NULL );

   if( NULL == warp_map ) {
      error_printf( "invalid warp map string index: %d", arg );
      return pc;
   }

   debug_printf( 2, "warp requested to: %s", warp_map );
   memory_zero_ptr( (MEMORY_PTR)(state->warp_to), TILEMAP_NAME_MAX );
   memory_strncpy_ptr( state->warp_to, warp_map, TILEMAP_NAME_MAX );

   state->warp_to_y = mobile_stack_pop( actor );
   state->warp_to_x = mobile_stack_pop( actor );

   /* Freeze */
   return pc;
}

uint16_t script_handle_ANIMATE(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   int16_t a_x = 0,
      a_y = 0,
      a_w = 0,
      a_h = 0;

   a_h = mobile_stack_pop( actor ) * TILE_H;
   a_w = mobile_stack_pop( actor ) * TILE_W;
   a_y = mobile_stack_pop( actor ) * TILE_H;
   a_x = mobile_stack_pop( actor ) * TILE_W;

#ifdef SCRIPT_HAS_GFX
   animate_create(
      arg, ANIMATE_FLAG_SCRIPT, SCREEN_MAP_X + a_x, SCREEN_MAP_Y + a_y, a_w, a_h
   );
#endif /* SCRIPT_HAS_GFX */

   return pc + 1;
}

uint16_t script_handle_PUSH(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   mobile_stack_push( actor, arg );

   return pc + 1;
}

uint16_t script_handle_POP(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   mobile_stack_pop( actor );

   return pc + 1;
}

uint16_t script_handle_EQUAL_JUMP(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   int8_t val1 = 0,
      val2 = 0;

   val2 = mobile_stack_pop( actor );
   val1 = mobile_stack_pop( actor );

   return val1 == val2 ? 
      script_goto_label( pc, script, SCRIPT_ACTION_START, arg ) : /* TRUE */
      pc + 1;                                                     /* FALSE */
}

uint16_t script_handle_GREATER_JUMP(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   int8_t val1 = 0,
      val2 = 0;

   val2 = mobile_stack_pop( actor );
   val1 = mobile_stack_pop( actor );

   return val1 > val2 ? 
      script_goto_label( pc, script, SCRIPT_ACTION_START, arg ) : /* TRUE */
      pc + 1;                                                     /* FALSE */
}

uint16_t script_handle_ADD(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   int8_t val1 = 0,
      val2 = 0;

   val2 = mobile_stack_pop( actor );
   val1 = mobile_stack_pop( actor );

   mobile_stack_push( actor, val1 + val2 );

   return pc + 1;
}

uint16_t script_handle_SUBTRACT(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   int8_t val1 = 0,
      val2 = 0;

   val2 = mobile_stack_pop( actor );
   val1 = mobile_stack_pop( actor );

   mobile_stack_push( actor, val1 - val2 );

   return pc + 1;
}

uint16_t script_handle_LESSER_JUMP(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   int8_t val1 = 0,
      val2 = 0;

   val2 = mobile_stack_pop( actor );
   val1 = mobile_stack_pop( actor );

   return val1 < val2 ? 
      script_goto_label( pc, script, SCRIPT_ACTION_START, arg ) : /* TRUE */
      pc + 1;                                                     /* FALSE */
}

uint16_t script_handle_ITEM_GIVE(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   int8_t i = 0;

   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      if( t->items[i].gid == arg ) {
         item_give_mobile( &(t->items[i]), &(state->player), state );
         return pc + 1;
      }
   }
   
   error_printf( "unable to give: invalid item GID: %d", arg );

   return pc + 1;
}

uint16_t script_handle_ITEM_TAKE(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   /* TODO: Push onto the stack how many taken. */

   return pc + 1;
}

uint16_t script_handle_DIE(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   /* Trigger pre-death blinking effect. */
   actor->hp = -10;

   return pc + 1;
}

#define SCRIPT_CB_TABLE_PARSE( idx, name, c ) case c: script->steps[script->steps_count].action = idx; c_idx++; break;

uint16_t script_parse_str(
   int script_idx,
   char* script_txt, int16_t script_txt_sz, struct SCRIPT* script
) {
   int c_idx = 0;

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

uint8_t script_init() {
   memory_zero_ptr( (MEMORY_PTR)&g_script_globals, sizeof( g_script_globals ) );
   return 1;
}

void script_shutdown() {

}

