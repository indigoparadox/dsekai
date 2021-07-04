
#include "icns.h"

#include "../../src/dio.h"
#include "../../src/memory.h"

#include <stdio.h>
#include <stdlib.h>

struct CONVERT_GRID* icns_read_file(
   const char* path, struct CONVERT_OPTIONS* o
) {
   uint8_t* icns_buffer = NULL;
   uint32_t icns_buffer_sz = 0;
   MEMORY_HANDLE icns_buffer_handle = NULL;
   struct CONVERT_GRID* grid_out = NULL;

   icns_buffer_sz = dio_read_file( path, &icns_buffer_handle );

   icns_buffer = memory_lock( icns_buffer_handle );

   grid_out = icns_read( icns_buffer, icns_buffer_sz, o );

   icns_buffer = memory_unlock( icns_buffer_handle );

   memory_free( icns_buffer_handle );

   return grid_out;
}

struct CONVERT_GRID* icns_read(
   const uint8_t* buf, uint32_t buf_sz, struct CONVERT_OPTIONS* o
) {
   int32_t bit_idx = 0,
      byte_idx = 0,
      i = 0;
   struct CONVERT_GRID* grid_out = NULL;
   /* struct ICNS_FILE_HEADER* file_header = NULL; */
   struct ICNS_DATA_HEADER* data_header = NULL;

   /* file_header = (struct ICNS_FILE_HEADER*)&(buf[0]); */
   data_header = (struct ICNS_DATA_HEADER*)&(buf[ICNS_FILE_HEADER_SZ]);

   /* printf( "icns %c%c%c%c data %d long\n",
      data_header->icon_type[0],
      data_header->icon_type[1],
      data_header->icon_type[2],
      data_header->icon_type[3],
      dio_reverse_endian_32( data_header->data_sz ) ); */

   grid_out = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid_out );

   grid_out->data = calloc( 256, 1 );
   grid_out->data_sz = 256;
   grid_out->bpp = 1;
   grid_out->sz_x = 16;
   grid_out->sz_y = 16;

   for( i = 0 ; 256 > i ; i++ ) {
      assert( byte_idx < data_header->data_sz );
      grid_out->data[i] |= buf[ICNS_DATA_HEADER_SZ + 
         ICNS_FILE_HEADER_SZ + byte_idx] & (0x1 << (7 - bit_idx));
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

int icns_write_file(
   const char* path, const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   uint32_t icns_buffer_sz = 0,
      icns_canvas_sz = 0;
   uint8_t* icns_buffer = NULL;
   FILE* file_out = NULL;
   int retval = 0;

   icns_canvas_sz = ((grid->sz_x * grid->sz_y * o->bpp) / 8);
   if( 1 == o->bpp ) {
      /* Add a mask. */
      icns_canvas_sz *= 2;
   }

   icns_buffer_sz = 
      ICNS_FILE_HEADER_SZ +
      ICNS_DATA_HEADER_SZ +
      icns_canvas_sz;

   /* TODO: Use memory architecture. */
   icns_buffer = calloc( 1, icns_buffer_sz );
   assert( NULL != icns_buffer );

   retval = icns_write( icns_buffer, icns_buffer_sz, grid, o );
   assert( !retval );
   if( retval ) {
      free( icns_buffer );
      return retval;
   }

   file_out = fopen( path, "wb" );
   assert( NULL != file_out );

   debug_printf( 2, "icns: writing to %s...\n", path );

   fwrite( icns_buffer, 1, icns_buffer_sz, file_out );

   fclose( file_out );
   free( icns_buffer );

   return retval;
}

int icns_write(
   uint8_t* buf_ptr, uint32_t buf_sz,
   const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   int retval = 0;
   int32_t grid_x = 0,
      grid_y = 0,
      file_byte_idx = 0,
      data_byte_idx = 0,
      bit_idx = 0,
      grid_idx = 0;
   uint8_t byte_buffer = 0;
   struct ICNS_FILE_HEADER* file_header =
      (struct ICNS_FILE_HEADER*)&(buf_ptr[0]);
   struct ICNS_DATA_HEADER* data_header =
      (struct ICNS_DATA_HEADER*)&(buf_ptr[ICNS_FILE_HEADER_SZ]);

   file_header->id[0] = 'i';
   file_header->id[1] = 'c';
   file_header->id[2] = 'n';
   file_header->id[3] = 's';

   if( 1 == o->bpp && 16 == grid->sz_x && 16 == grid->sz_y ) {
      data_header->icon_type[0] = 'i';
      data_header->icon_type[1] = 'c';
      data_header->icon_type[2] = 's';
      data_header->icon_type[3] = '#';

   } else if( 4 == o->bpp && 16 == grid->sz_x && 16 == grid->sz_y ) {
      data_header->icon_type[0] = 'i';
      data_header->icon_type[1] = 'c';
      data_header->icon_type[2] = 's';
      data_header->icon_type[3] = '4';

   } else if( 8 == o->bpp && 16 == grid->sz_x && 16 == grid->sz_y ) {
      data_header->icon_type[0] = 'i';
      data_header->icon_type[1] = 'c';
      data_header->icon_type[2] = 's';
      data_header->icon_type[3] = '8';

   } else if( 1 == o->bpp && 32 == grid->sz_x && 32 == grid->sz_y ) {
      data_header->icon_type[0] = 'I';
      data_header->icon_type[1] = 'C';
      data_header->icon_type[2] = 'N';
      data_header->icon_type[3] = '#';

   } else if( 8 == grid->bpp && 32 == grid->sz_x && 32 == grid->sz_y ) {
      data_header->icon_type[0] = 'i';
      data_header->icon_type[1] = 'c';
      data_header->icon_type[2] = 'l';
      data_header->icon_type[3] = '8';
   }

   debug_printf( 2, "icns: writing type %c%c%c%c (%dx%d %d bpp)\n",
      data_header->icon_type[0],
      data_header->icon_type[1],
      data_header->icon_type[2],
      data_header->icon_type[3],
      grid->sz_x,
      grid->sz_y,
      o->bpp );

   file_byte_idx += sizeof( struct ICNS_FILE_HEADER ) \
      + sizeof( struct ICNS_DATA_HEADER );
   data_byte_idx = 0;
   for( grid_y = 0 ; grid->sz_y > grid_y ; grid_y++ ) {
      for( grid_x = 0 ; grid->sz_x > grid_x ; grid_x++ ) {
         grid_idx = ((grid_y * grid->sz_x) + grid_x);

         assert( grid_idx < grid->data_sz );
         assert( file_byte_idx < buf_sz );

         byte_buffer <<= o->bpp;
         /* Only allow reversal if 1bpp. */
         if( o->reverse && 1 == o->bpp ) {
            byte_buffer |= 0x01;
            byte_buffer &= ~((grid->data[grid_idx] & 0x01));
         } else {
            assert( 1 == o->bpp );
            if( grid->data[grid_idx] ) {
               byte_buffer |= 0x01;
            } else {
               byte_buffer |= 0x00;
            }
         }
         bit_idx += o->bpp;

         if( 8 <= bit_idx ) {
            buf_ptr[file_byte_idx] = byte_buffer;
            file_byte_idx++;
            data_byte_idx++;
            bit_idx = 0;
            byte_buffer = 0;
         }
      }
   }

   /* Write the mask. */
   /* TODO: Make the insides opaque. */
   for( grid_y = 0 ; grid->sz_y > grid_y ; grid_y++ ) {
      for( grid_x = 0 ; grid->sz_x > grid_x ; grid_x++ ) {
         grid_idx = ((grid_y * grid->sz_x) + grid_x);

         assert( grid_idx < grid->data_sz );
         assert( file_byte_idx < buf_sz );

         byte_buffer <<= o->bpp;
         /* Only allow reversal if 1bpp. */
         if( o->reverse && 1 == o->bpp ) {
            byte_buffer |= 0x01;
            byte_buffer &= ~((grid->data[grid_idx] & 0x01));
         } else {
            assert( 1 == o->bpp );
            if( grid->data[grid_idx] ) {
               byte_buffer |= 0x01;
            } else {
               byte_buffer |= 0x00;
            }
         }
         bit_idx += o->bpp;

         if( 8 <= bit_idx ) {
            buf_ptr[file_byte_idx] = byte_buffer;
            file_byte_idx++;
            data_byte_idx++;
            bit_idx = 0;
            byte_buffer = 0;
         }
      }
   }

   file_header->file_sz = dio_reverse_endian_32( file_byte_idx );
   data_header->data_sz = dio_reverse_endian_32( data_byte_idx +
      ICNS_DATA_HEADER_SZ );

   return retval;
}

