
#ifndef DSTYPES_H
#define DSTYPES_H

#if defined( USE_DOS ) || defined( USE_PALM )
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

