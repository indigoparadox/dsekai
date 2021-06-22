
#ifndef CONVERT_H
#define CONVERT_H

#include "dstypes.h"

#define CONVERT_BUFFER_SZ 1024

#ifndef CONVERT_SILENT
#define convert_printf( ... ) printf( __VA_ARGS__ )
#else
#define convert_printf( ... )
#endif

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
};

uint32_t convert_reverse_endian_32( uint32_t );
uint16_t convert_reverse_endian_16( uint16_t );
void convert_print_binary( uint8_t byte_in );
uint32_t convert_read_file( const char*, uint8_t** );

#endif /* CONVERT_H */

