
#ifndef CGA_H
#define CGA_H

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../../src/dstypes.h"
#include "../../src/memory.h"
#include "../../src/dio.h"

struct CGA_HEADER {
   char type[4];
   uint32_t version;
   uint32_t width;
   uint32_t height;
   uint32_t bpp;
   uint32_t plane1_offset;
   uint32_t plane1_sz;
   uint32_t plane2_offset;
   uint32_t plane2_sz;
   uint32_t palette;
   uint32_t endian;
   uint32_t reserved1;
   uint32_t reserved2;
   uint32_t reserved3;
   uint32_t reserved4;
};

#ifndef NO_CGA_FUNCTIONS

struct CONVERT_GRID;
struct CONVERT_OPTIONS;

int cga_write( struct DIO_STREAM*, MEMORY_HANDLE, struct CONVERT_OPTIONS* );
MEMORY_HANDLE cga_read( struct DIO_STREAM*, struct CONVERT_OPTIONS* );

#endif /* NO_CGA_FUNCTIONS */

#endif /* CGA_H */

