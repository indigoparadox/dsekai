
#define SCRIPT_C
#include "dsekai.h"

uint16_t script_handle_INTERACT(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
   return pc + 1;
}

static uint16_t script_handle_WALK_generic(
   uint16_t pc, struct MOBILE* actor, int16_t mod_x, int16_t mod_y
) {
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
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
   return script_handle_WALK_generic( pc, actor, 0, -1 );
}

uint16_t script_handle_WALK_SOUTH(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
   return script_handle_WALK_generic( pc, actor, 0, 1 );
}

uint16_t script_handle_WALK_EAST(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
   debug_printf( 0, "script: walk east" );
   return script_handle_WALK_generic( pc, actor, 1, 0 );
}

uint16_t script_handle_WALK_WEST(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
   debug_printf( 0, "script: walk west" );
   return script_handle_WALK_generic( pc, actor, -1, 0 );
}

uint16_t script_handle_SLEEP(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
   /* TODO: Base current time on ticks? */
   return pc + 1;
}

uint16_t script_handle_START(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
   /* NOOP */
   return pc + 1;
}

uint16_t script_handle_GOTO(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
   /* NOOP */
   return arg;
}

#define SCRIPT_CB_TABLE_PARSE( idx, name, c ) case c: script->steps[step_idx].action = idx; c_idx++; break;

uint16_t script_parse_str(
   char* script_txt, int16_t script_txt_sz, struct SCRIPT* script
) {
   int c_idx = 0,
      step_idx = 0;

   for( ; script_txt_sz > c_idx ; ) {
      debug_printf( 2, "step %d char: %c", step_idx, script_txt[c_idx] );
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
      script->steps[step_idx].arg = dio_atoi( &(script_txt[c_idx]), 10 );
      debug_printf(
         3, "step %d arg: %d", step_idx, script->steps[step_idx].arg );
      if( 10000 <= script->steps[step_idx].arg ) {
         c_idx += 5;
      } else if( 1000 <= script->steps[step_idx].arg ) {
         c_idx += 4;
      } else if( 100 <= script->steps[step_idx].arg ) {
         c_idx += 3;
      } else if( 10 <= script->steps[step_idx].arg ) {
         c_idx += 2;
      } else {
         c_idx++;
      }

      step_idx++;
   }

   return step_idx;
}

