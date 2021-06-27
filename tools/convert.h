
#ifndef CONVERT_H
#define CONVERT_H

#include "../src/dstypes.h"

#define CONVERT_BUFFER_SZ 1024

struct CONVERT_GRID {
   int32_t sz_x;
   int32_t sz_y;
   uint16_t bpp;
   uint32_t data_sz;
   uint8_t *data;
};

struct CONVERT_OPTIONS {
   uint8_t reverse;
   uint16_t bpp;
   int32_t w;
   int32_t h;
   uint32_t line_padding;
   uint32_t plane_padding;
   uint32_t bmp_data_sz;
   uint8_t cga_use_header;
   uint8_t little_endian;
};

#endif /* CONVERT_H */

