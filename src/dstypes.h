
#ifndef DSTYPES_H
#define DSTYPES_H


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

#define NULL (void*)0
#else
#include <stdint.h>
#endif /* __DOS__ */

#endif /* DSTYPES_H */

