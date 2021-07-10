
#ifndef DSTYPES_H
#define DSTYPES_H

#include "config.h"

#include <stddef.h>

#if !defined( offsetof )
#warn "offsetof not defined in stddef.h; using internal version..."
#define offsetof( obj, field ) ((int)(&(((obj*)(0))->field)))
#endif /* !offsetof */

/* ------ */
#ifdef PLATFORM_PALM
/* ------ */

#include <PalmOS.h>
typedef UInt8 uint8_t;
typedef Int8 int8_t;
typedef UInt16 uint16_t;
typedef Int16 int16_t;
typedef UInt32 uint32_t;
typedef Int32 int32_t;

/* ------ */
#elif defined( PLATFORM_DOS )
/* ------ */

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

/* ------ */
#elif defined( PLATFORM_WIN16 ) || defined( PLATFORM_WINCE )
/* ------ */

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

/* ------ */
#else
/* ------ */

#include <stdint.h>

/* ------ */
#endif /* PLATFORM_DOS, PLATFORM_SDL, PLATFORM_PALM, PLATFORM_WIN16 */
/* ------ */

struct TILEMAP_COORDS {
   uint8_t x;
   uint8_t y;
};

struct DSEKAI_STATE;

#include "memory.h"
#include "engines.h"
#include "resource.h"
#include "graphics.h"
#include "input.h"
#include "dio.h"
#include "drc.h"
#ifdef USE_JSON_MAPS
#include "json.h"
#endif /* USE_JSON_MAPS */
#include "item.h"
#include "tilemap.h"
#include "mobile.h"
#include "window.h"
#include "control.h"

struct WINDOW;

struct DSEKAI_STATE {
   int player_idx;
   int initalized;

   struct MOBILE mobiles[MOBILES_MAX];
   uint8_t mobiles_count; 

   struct TILEMAP map;

   int semi_cycles;
   int walk_offset;
   uint8_t input_blocked_countdown;
   int screen_scroll_x;
   int screen_scroll_y;
   int screen_scroll_tx;
   int screen_scroll_ty;
   int screen_scroll_x_tgt;
   int screen_scroll_y_tgt;

   uint8_t window_shown;
   MEMORY_HANDLE windows_handle;
   uint8_t windows_count;
};

#endif /* DSTYPES_H */

