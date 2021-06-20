
#ifndef DSTYPES_H
#define DSTYPES_H

#include "config.h"

#ifdef USE_PALM

#include <PalmOS.h>
typedef UInt8 uint8_t;
typedef Int8 int8_t;
typedef UInt16 uint16_t;
typedef Int16 int16_t;
typedef UInt32 uint32_t;
typedef Int32 int32_t;

#elif defined( USE_DOS )

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

#define RESOURCE_SPRITE_HEADERS

#else

#include <stdint.h>

#define RESOURCE_SPRITE_HEADERS

#endif /* __DOS__ */

typedef struct GRAPHICS_MASK {uint8_t bits[16]; } GRAPHICS_MASK;
#ifdef RESOURCE_SPRITE_HEADERS
typedef struct GRAPHICS_SPRITE { SPRITE_TYPE bits[SPRITE_H]; } GRAPHICS_SPRITE;
typedef struct GRAPHICS_TILE { TILE_TYPE bits[TILE_H]; } GRAPHICS_TILE;
typedef struct GRAPHICS_BITMAP { uint8_t bits[1]; } GRAPHICS_BITMAP;
#else
typedef uint16_t GRAPHICS_SPRITE;
typedef uint16_t GRAPHICS_TILE;
typedef uint16_t GRAPHICS_BITMAP;
#endif /* RESOURCE_HEADERS */
typedef struct GRAPHICS_PATTERN {
   PATTERN_TYPE bits[PATTERN_H]; } GRAPHICS_PATTERN;

#endif /* DSTYPES_H */

