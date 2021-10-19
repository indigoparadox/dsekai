
#define SCRIPT_C
#include "dsekai.h"

#if defined( SCREEN_W ) && defined( SCREEN_H )
/* We're running in the engine and not an external tool. */
#define SCRIPT_HAS_GFX
#endif /* SCREEN_W && SCREEN_H */

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
   uint16_t pc, struct MOBILE* actor, int16_t mod_x, int16_t mod_y
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
      return pc + 1;

   } else if(
      actor->coords.y != actor->coords_prev.y ||
      actor->coords.x != actor->coords_prev.x
   ) {
      /* Actor is already walking, don't start or advance PC. */   

   } else {
      debug_printf( 1, "scripted mobile starting walking" );
      mobile_walk_start( actor, mod_x, mod_y );
   }

   return pc;
}

uint16_t script_handle_WALK_NORTH(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   return script_handle_WALK_generic( pc, actor, 0, -1 );
}

uint16_t script_handle_WALK_SOUTH(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   return script_handle_WALK_generic( pc, actor, 0, 1 );
}

uint16_t script_handle_WALK_EAST(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   debug_printf( 0, "script: walk east" );
   return script_handle_WALK_generic( pc, actor, 1, 0 );
}

uint16_t script_handle_WALK_WEST(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   debug_printf( 0, "script: walk west" );
   return script_handle_WALK_generic( pc, actor, -1, 0 );
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
   actor->script_pc_prev = actor->script_pc;
   return script_goto_label( pc, script, SCRIPT_ACTION_START, arg );
}

uint16_t script_handle_RETURN(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
   return actor->script_pc_prev;
}

uint16_t script_handle_SPEAK(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg
) {
#ifdef SCRIPT_HAS_GFX
   window_prefab_dialog(
      WINDOW_ID_SCRIPT_SPEAK, arg, actor->sprite, state, t );
#endif /* SCRIPT_HAS_GFX */
   return pc + 1;
}

#define SCRIPT_CB_TABLE_PARSE( idx, name, c ) case c: script->steps[script->steps_count].action = idx; c_idx++; break;

uint16_t script_parse_str(
   char* script_txt, int16_t script_txt_sz, struct SCRIPT* script
) {
   int c_idx = 0;

   for( ; script_txt_sz > c_idx ; ) {
      debug_printf( 2, "step %d char: %c",
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
         3, "step %d arg: %d",
         script->steps_count, script->steps[script->steps_count].arg );
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

