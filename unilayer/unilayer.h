
#ifndef DSTYPES_H
#define DSTYPES_H

#include "config.h"

#ifdef __GNUC__
#define WARN_UNUSED __attribute__( (warn_unused_result) )
#else
#define WARN_UNUSED
#endif /* __GNUC__ */

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

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
TILEMAP_COORDS {
   int32_t x;
   int32_t y;
};

#include "memory.h"
#include "resource.h"
#include "graphics.h"
#include "input.h"
#include "dio.h"
#include "drc.h"

#endif /* DSTYPES_H */

