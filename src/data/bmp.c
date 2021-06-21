
#include "bmp.h"

#include "../dstypes.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define BMP_BUFFER_SZ 1024

#define BMP_FMT_FILE_HEADER_SZ   14
#define BMP_FMT_BMP_HEADER_SZ    40

#ifndef BMP_SILENT
#define bmp_printf( ... ) printf( __VA_ARGS__ )
#else
#define bmp_printf( ... )
#endif

#define bmp_int( type, buf, offset ) *(type*)&(buf[offset])

static uint32_t bmp_reverse_endian( uint32_t int_in, int sz_in ) {
   int i = 0;
   uint32_t int_out = 0;

   for( i = sz_in - 1 ; 0 <= i ; i-- ) {
      int_out <<= 8;
      int_out |= (int_in & 0xff);
      int_in >>= 8;
   }

   return int_out;
}

unsigned char* bmp_read( char* path ) {
   size_t offset = 0,
      read = 0,
      x = 0,
      y = 0,
      i = 0,
      byte_idx = 0,
      bit_idx = 0;
   uint32_t bmp_file_size = 0,
      bmp_data_offset = 0,
      bmp_data_size = 0;
   int32_t sz_x = 0,
      sz_y = 0;
   uint16_t bpp = 0;
   FILE* bmp_file = NULL;
   uint8_t* grid = NULL,
      * raw_bmp_data = NULL;
   char buffer[BMP_BUFFER_SZ + 1];
   char byte_buffer = 0;

   memset( buffer, '\0', BMP_BUFFER_SZ );

   bmp_file = fopen( path, "rb" );
   assert( NULL != bmp_file );

   /* Read the bitmap file header. */
   read = fread( buffer, 1, BMP_FMT_FILE_HEADER_SZ, bmp_file );
   assert( BMP_FMT_FILE_HEADER_SZ == read );
   assert( 'B' == buffer[0] );
   assert( 'M' == buffer[1] );
   bmp_file_size = bmp_int( uint32_t, buffer, 2 );
   bmp_printf( "bitmap file is %u bytes\n", bmp_file_size );
   bmp_data_offset = bmp_int( uint32_t, buffer, 10 );
   bmp_printf( "bitmap data starts at %u bytes\n", bmp_data_offset );

   memset( buffer, '\0', BMP_BUFFER_SZ );
   read = fread( buffer, 1, BMP_FMT_BMP_HEADER_SZ, bmp_file );
   assert( BMP_FMT_BMP_HEADER_SZ == read );

   /* Read the bitmap image header. */
   bmp_printf( "bitmap header is %u bytes\n", bmp_int( uint32_t, buffer, 0 ) );
   assert( 40 == bmp_int( uint32_t, buffer, 0 ) ); /* Windows BMP. */
   sz_x = bmp_int( int32_t, buffer, 4 );
   sz_y = bmp_int( int32_t, buffer, 8 );
   bpp = bmp_int( uint16_t, buffer, 14 );
   bmp_printf( "bitmap is %d x %d, %u colors (palette has %u)\n",
      sz_x, sz_y, bpp, bmp_int( uint32_t, buffer, 28 ) );
   assert( bmp_int( uint16_t, buffer, 16 ) == 0 ); /* No compression. */

   /* Read the bitmap data. */
   bmp_data_size = bmp_file_size - bmp_data_offset;
   printf( "bitmap data is %lu bytes\n", bmp_data_size );
   raw_bmp_data = calloc( 1, bmp_data_size );
   assert( NULL != raw_bmp_data );
   fseek( bmp_file, bmp_data_offset, SEEK_SET );
   read = fread( raw_bmp_data, 1, bmp_data_size, bmp_file );
   assert( read == bmp_data_size );
   grid = calloc( 1, sz_x * sz_y );
   assert( NULL != grid );

   y = sz_y - 1;
   while( bmp_data_size > byte_idx ) {
      printf( "byte_idx %lu, bit_idx %lu, row %lu, col %lu (%lu)\n",
         byte_idx, bit_idx, y, x, (y * sz_x) + x );
      if( 0 == bit_idx % 8 ) {
         byte_buffer = raw_bmp_data[byte_idx];
         byte_idx++;
         bit_idx = 0;
      }

      assert( (y * sz_x) + x < (bmp_data_size * (8 / bpp)) );

      /* Read this pixel into the grid. */
      for( i = 0 ; bpp > i ; i++ ) {
         grid[(y * sz_x) + x] |= byte_buffer & (0x01 << bit_idx);
         bit_idx++;
      }
      grid[(y * sz_x) + x] >>= (bit_idx - bpp);

      /* Move to the next pixel. */
      x++;
      if( x >= sz_x ) {
         /* Move to the next row. */
         y--;
         x = 0;
         while( byte_idx % 4 != 0 ) {
            byte_idx++;
         }
         /* Get past the padding. */
      }
   }

   for( y = 0 ; sz_y > y ; y++ ) {
      printf( "\n" );
      for( x = 0 ; sz_x > x ; x++ ) {
         if( 0 == grid[(y * sz_x) + x] ) {
            printf( " ," );
         } else {
            printf( "%x,", grid[(y * sz_x) + x] );
         }
      }
   }
   printf( "\n" );

   /*
   for( y = sz_y - 1 ; 0 <= y ; y-- ) {
      for( x = 0 ; sz_x > x ; x++ ) {
         byte_buffer = 
         grid[(y * sz_x) + x] = byte_buffer;
      }
   }
   */

   return grid;
}

