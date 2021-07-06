
#include "icns.h"

#include "../../src/dio.h"
#include "../../src/memory.h"

#include <stdio.h>
#include <stdlib.h>

DECLARE_FMT_WRITE( icns ) {
   uint8_t
      * grid_buffer = NULL,
      * grid_data = NULL;
   struct CONVERT_GRID_HEADER* grid = NULL;
   struct ICNS_FILE_HEADER file_header;
   struct ICNS_DATA_HEADER data_header;
   int retval = 0;
   int32_t grid_x = 0,
      grid_y = 0,
      file_byte_idx = 0,
      data_byte_idx = 0,
      bit_idx = 0,
      grid_idx = 0;
   uint8_t byte_buffer = 0;

   LOCK_CONVERT_GRID( grid_data, grid, grid_handle );

   file_header.id[0] = 'i';
   file_header.id[1] = 'c';
   file_header.id[2] = 'n';
   file_header.id[3] = 's';

   if( 1 == o->bpp && 16 == grid->sz_x && 16 == grid->sz_y ) {
      data_header.icon_type[0] = 'i';
      data_header.icon_type[1] = 'c';
      data_header.icon_type[2] = 's';
      data_header.icon_type[3] = '#';

   } else if( 4 == o->bpp && 16 == grid->sz_x && 16 == grid->sz_y ) {
      data_header.icon_type[0] = 'i';
      data_header.icon_type[1] = 'c';
      data_header.icon_type[2] = 's';
      data_header.icon_type[3] = '4';

   } else if( 8 == o->bpp && 16 == grid->sz_x && 16 == grid->sz_y ) {
      data_header.icon_type[0] = 'i';
      data_header.icon_type[1] = 'c';
      data_header.icon_type[2] = 's';
      data_header.icon_type[3] = '8';

   } else if( 1 == o->bpp && 32 == grid->sz_x && 32 == grid->sz_y ) {
      data_header.icon_type[0] = 'I';
      data_header.icon_type[1] = 'C';
      data_header.icon_type[2] = 'N';
      data_header.icon_type[3] = '#';

   } else if( 8 == grid->bpp && 32 == grid->sz_x && 32 == grid->sz_y ) {
      data_header.icon_type[0] = 'i';
      data_header.icon_type[1] = 'c';
      data_header.icon_type[2] = 'l';
      data_header.icon_type[3] = '8';
   }

   debug_printf( 2, "icns: writing type %c%c%c%c (%dx%d %d bpp)\n",
      data_header.icon_type[0],
      data_header.icon_type[1],
      data_header.icon_type[2],
      data_header.icon_type[3],
      grid->sz_x,
      grid->sz_y,
      o->bpp );

   dio_seek_stream( stream,
      sizeof( struct ICNS_FILE_HEADER ) + sizeof( struct ICNS_DATA_HEADER ),
      SEEK_SET );

   file_byte_idx += sizeof( struct ICNS_FILE_HEADER ) \
      + sizeof( struct ICNS_DATA_HEADER );
   data_byte_idx = 0;
   for( grid_y = 0 ; grid->sz_y > grid_y ; grid_y++ ) {
      for( grid_x = 0 ; grid->sz_x > grid_x ; grid_x++ ) {
         grid_idx = ((grid_y * grid->sz_x) + grid_x);

         assert( grid_idx < grid->data_sz );

         byte_buffer <<= o->bpp;
         /* Only allow reversal if 1bpp. */
         if( o->reverse && 1 == o->bpp ) {
            byte_buffer |= 0x01;
            byte_buffer &= ~((grid_data[grid_idx] & 0x01));
         } else {
            assert( 1 == o->bpp );
            if( grid_data[grid_idx] ) {
               byte_buffer |= 0x01;
            } else {
               byte_buffer |= 0x00;
            }
         }
         bit_idx += o->bpp;

         if( 8 <= bit_idx ) {
            dio_write_stream( &byte_buffer, 1, stream );
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

         byte_buffer <<= o->bpp;
         /* Only allow reversal if 1bpp. */
         if( o->reverse && 1 == o->bpp ) {
            byte_buffer |= 0x01;
            byte_buffer &= ~((grid_data[grid_idx] & 0x01));
         } else {
            assert( 1 == o->bpp );
            if( grid_data[grid_idx] ) {
               byte_buffer |= 0x01;
            } else {
               byte_buffer |= 0x00;
            }
         }
         bit_idx += o->bpp;

         if( 8 <= bit_idx ) {
            dio_write_stream( &byte_buffer, 1, stream );
            file_byte_idx++;
            data_byte_idx++;
            bit_idx = 0;
            byte_buffer = 0;
         }
      }
   }

   file_header.file_sz = dio_reverse_endian_32( file_byte_idx );
   data_header.data_sz = dio_reverse_endian_32( data_byte_idx +
      sizeof( struct ICNS_DATA_HEADER ) );

   dio_write_stream( &file_header, sizeof( struct ICNS_FILE_HEADER ), stream );
   dio_write_stream( &data_header, sizeof( struct ICNS_FILE_HEADER ), stream );

cleanup:

   if( NULL != grid ) {
      grid = memory_unlock( grid_handle );
   }

   return file_byte_idx;
}

DECLARE_FMT_READ( icns ) {
   uint8_t
      * grid_buffer = NULL,
      * grid_data = NULL;
   struct CONVERT_GRID_HEADER* grid = NULL;
   struct ICNS_DATA_HEADER data_header;
   int32_t bit_idx = 0,
      byte_idx = 0,
      i = 0;
   uint8_t byte_buffer = 0;

   dio_seek_stream( stream, 0, SEEK_SET );
   dio_read_stream( &data_header, sizeof( struct ICNS_FILE_HEADER ), stream );

   /* TODO: Handle 32x32. */
   NEW_CONVERT_GRID( 16, 16, 1, grid_data, grid, *grid_handle );

   for( i = 0 ; 256 > i ; i++ ) {
      assert( byte_idx < data_header.data_sz );
      dio_read_stream( &byte_buffer, 1, stream );
      grid_data[i] |= (byte_buffer & (0x1 << (7 - bit_idx)));
      grid_data[i] >>= (7 - bit_idx);
      bit_idx++;
      if( 8 <= bit_idx ) {
         byte_idx++;
         bit_idx = 0;
      }
   }

cleanup:

   if( NULL != grid ) {
      grid = memory_unlock( *grid_handle );
   }

   return byte_idx;
}

#if 0
int icns_write_file(
   struct DIO_STREAM stream, const struct CONVERT_GRID* grid,
   struct CONVERT_OPTIONS* o
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
#endif

