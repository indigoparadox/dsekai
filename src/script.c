
#define SCRIPT_C
#include "dsekai.h"

uint16_t script_handle_INTERACT(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile
) {
   return pc + 1;
}

uint16_t script_handle_WALK(
   uint16_t pc,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile
) {

   return pc + 1;
}

