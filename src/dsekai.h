
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

 /* === Includes === */

#include "config.h"
#include <unilayer.h>

struct DSEKAI_STATE;

#define ENGINE_STATE_OPENING 1
#define ENGINE_STATE_RUNNING 2

#include "tmstruct.h"

#include "engines.h"
#ifdef RESOURCE_FILE
#include "json.h"
#include "asn.h"
#endif /* RESOURCE_FILE */
#include "script.h"
#include "item.h"
#include "tilemap.h"
#include "mobile.h"
#include "window.h"
#include "control.h"

#ifdef TILEMAP_FMT_JSON
#include "tmjson.h"
#endif /* TILEMAP_FMT_JSON */

#ifdef TILEMAP_FMT_ASN
#include "tmasn.h"
#endif /* TILEMAP_FMT_ASN */

#ifdef TILEMAP_FMT_BIN
#include "tmbin.h"
#endif /* TILEMAP_FMT_BIN */

/* === Limits === */

#ifndef DSEKAI_MOBILES_MAX
#define DSEKAI_MOBILES_MAX 20
#endif /* !MOBILES_MAX */

#ifndef DSEKAI_ITEMS_MAX
#define DSEKAI_ITEMS_MAX 40
#endif /* !DSEKAI_ITEMS_MAX */

/* === Structs === */

struct WINDOW;

/*! \brief General/shared state of the running engine in memory. */
struct PACKED DSEKAI_STATE {
   struct ITEM items[DSEKAI_ITEMS_MAX];
   uint16_t items_count;

   struct TILEMAP map;

   /*! \brief Array of currently loaded MOBILE objects on this map. */
   struct MOBILE mobiles[DSEKAI_MOBILES_MAX];
   struct MOBILE player;

   MEMORY_HANDLE engine_state_handle;

   uint8_t input_blocked_countdown;
   uint8_t ani_sprite_countdown;
   uint16_t ani_sprite_x;

   MEMORY_HANDLE windows_handle;
   uint16_t windows_count;

   char warp_to[TILEMAP_NAME_MAX];
   uint8_t warp_to_x;
   uint8_t warp_to_y;

   uint16_t engine_state;

};

#endif /* !DSEKAI_H */

