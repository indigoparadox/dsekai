
#ifndef DSTYPES_H
#define DSTYPES_H

#include "config.h"

#include <stddef.h>

/* ------ */
#ifdef USE_PALM
/* ------ */

#include <PalmOS.h>
typedef UInt8 uint8_t;
typedef Int8 int8_t;
typedef UInt16 uint16_t;
typedef Int16 int16_t;
typedef UInt32 uint32_t;
typedef Int32 int32_t;

/* ------ */
#elif defined( USE_DOS )
/* ------ */

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

#define RESOURCE_SPRITE_HEADERS

/* ------ */
#elif defined( USE_WIN16 )
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
#endif /* __DOS__ */
/* ------ */

#endif /* DSTYPES_H */

