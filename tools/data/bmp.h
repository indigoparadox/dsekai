
#ifndef BMP_H
#define BMP_H

#include "../../src/dstypes.h"
#include "../convert.h"
#include "../../src/memory.h"
#include "../../src/dio.h"

struct CONVERT_GRID;
struct CONVERT_OPTIONS;

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
BITMAP_FILE_HEADER {
   char id[2];
   uint32_t file_sz;
   uint16_t reserved1;
   uint16_t reserved2;
   uint32_t bmp_offset;
};

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
BITMAP_DATA_HEADER {
   uint32_t header_sz;
   int32_t bitmap_w;
   int32_t bitmap_h;
   uint16_t planes;
   uint16_t bpp;
   uint32_t compression;
   uint32_t image_sz;
   int32_t hres;
   int32_t vres;
   uint32_t colors;
   uint32_t colors_important;
};

#define BMP_COMPRESSION_NONE (0)

uint8_t bmp_colors_count( uint8_t );
DECLARE_FMT_READ( bmp );
DECLARE_FMT_WRITE( bmp );

#endif /* BMP_H */

