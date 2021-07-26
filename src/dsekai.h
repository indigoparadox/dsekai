
#ifndef DSEKAI_H
#define DSEKAI_H

#include "config.h"
#include <unilayer.h>

struct DSEKAI_STATE;

#include "engines.h"
#ifdef USE_JSON_MAPS
#include "json.h"
#endif /* USE_JSON_MAPS */
#include "item.h"
#include "tilemap.h"
#include "mobile.h"
#include "window.h"
#include "control.h"

struct WINDOW;

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
DSEKAI_STATE {
   uint32_t version;

   MEMORY_HANDLE items;
   uint16_t items_count;

   MEMORY_HANDLE map;
   MEMORY_HANDLE mobiles;
   uint16_t mobiles_count; 
   uint16_t player_idx;

   uint8_t walk_offset;
   uint8_t input_blocked_countdown;
   uint8_t reserved;
   int32_t screen_scroll_x;
   int32_t screen_scroll_y;
   int32_t screen_scroll_tx;
   int32_t screen_scroll_ty;
   int32_t screen_scroll_x_tgt;
   int32_t screen_scroll_y_tgt;

   MEMORY_HANDLE windows_handle;
   uint16_t windows_count;

   uint16_t engine_state;

};

#endif /* !DSEKAI_H */

