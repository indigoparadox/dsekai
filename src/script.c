
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
   return script_handle_WALK_generic( pc, actor, 1, 0 );
}

uint16_t script_handle_WALK_WEST(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   int16_t arg
) {
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

