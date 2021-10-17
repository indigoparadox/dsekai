
#ifndef DSEKAI_H
#define DSEKAI_H

/*! \file dsekai.h
 *  \brief Engine state data and main header, used in every engine file.
 *
 *  This file should be used to call all other engine headers, to ensure
 *  dependencies are kept and in order.
 *
 *  Data types and constants that are used in multiple engine headers should
 *  be placed in this file.
 */

#include "config.h"
#include <unilayer.h>

struct DSEKAI_STATE;

#define MOBILE_SCRIPT_STEPS_MAX 16

/*! \brief X/Y coordinates of a tile on the current map. */
struct PACKED TILEMAP_COORDS {
   int32_t x;
   int32_t y;
};

#include "engines.h"
#ifdef USE_JSON_MAPS
#include "json.h"
#endif /* USE_JSON_MAPS */
#include "script.h"
#include "item.h"
#include "tilemap.h"
#include "mobile.h"
#include "window.h"
#include "control.h"

struct WINDOW;

/*! \brief General/shared state of the running engine in memory. */
struct PACKED DSEKAI_STATE {
   MEMORY_HANDLE items;
   uint16_t items_count;

   MEMORY_HANDLE map;

   /*! \brief Array of currently loaded MOBILE objects on this map. */
   MEMORY_HANDLE mobiles;
   /*! \brief Number of active MOBILE objects in DSEKAI_STATE::mobiles. */
   uint16_t mobiles_count; 

   /*! \brief Index of the player MOBILE in DSEKAI_STATE::mobiles. */
   uint16_t player_idx;

   uint8_t input_blocked_countdown;
   uint8_t ani_sprite_countdown;
   uint16_t ani_sprite_x;
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

