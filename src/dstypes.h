
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

#define RESOURCE_SPRITE_HEADERS

/* ------ */
#elif defined( PLATFORM_WIN16 )
/* ------ */

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

#define RESOURCE_SPRITE_HEADERS

/* ------ */
#else
/* ------ */

#include <stdint.h>

#define RESOURCE_SPRITE_HEADERS

/* ------ */
#endif /* PLATFORM_DOS, PLATFORM_SDL, PLATFORM_PALM, PLATFORM_WIN16 */
/* ------ */

#endif /* DSTYPES_H */

