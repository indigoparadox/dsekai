
#include "icns.h"

#include "../../src/data/dio.h"
#include "../../src/memory.h"

#include <stdio.h>

struct CONVERT_GRID* icns_read_file(
   const char* path, struct CONVERT_OPTIONS* o
) {
   uint8_t* icns_buffer = NULL;
   uint32_t icns_buffer_sz = 0;
   struct CONVERT_GRID* grid_out = NULL;

   icns_buffer_sz = dio_read_file( path, &icns_buffer );

   grid_out = icns_read( icns_buffer, icns_buffer_sz, o );

   memory_free( &icns_buffer );

   return grid_out;
}

struct CONVERT_GRID* icns_read(
   const uint8_t* buf, uint32_t buf_sz, struct CONVERT_OPTIONS* o
) {
   uint32_t x = 0;
   int32_t bit_idx = 0,
      byte_idx = 0,
      i = 0;
   struct CONVERT_GRID* grid_out = NULL;
   struct ICNS_FILE_HEADER* file_header = NULL;
   struct ICNS_DATA_HEADER* data_header = NULL;

   file_header = (struct ICNS_DATA_HEADER*)buf;
   data_header = (struct ICNS_DATA_HEADER*)&(buf[ICNS_FILE_HEADER_SZ]);

   printf( "icns %c%c%c%c data %d long\n",
      data_header->icon_type[0],
      data_header->icon_type[1],
      data_header->icon_type[2],
      data_header->icon_type[3],
      dio_reverse_endian_32( data_header->data_sz ) );

   grid_out = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid_out );

   grid_out->data = calloc( 256, 1 );
   grid_out->data_sz = 256;
   grid_out->bpp = 1;
   grid_out->sz_x = 16;
   grid_out->sz_y = 16;

   for( i = 0 ; 256 > i ; i++ ) {
      assert( byte_idx < data_header->data_sz );
      grid_out->data[i] |= buf[ICNS_DATA_HEADER_SZ + ICNS_FILE_HEADER_SZ + byte_idx] & (0x1 << (7 - bit_idx));
      grid_out->data[i] >>= (7 - bit_idx);
      bit_idx++;
      if( 8 <= bit_idx ) {
         byte_idx++;
         bit_idx = 0;
      }
   }

   /* printf( "%d vs %d\n", byte_idx, data_header->data_sz );
   assert( byte_idx == data_header->data_sz ); */

   return grid_out;
}

