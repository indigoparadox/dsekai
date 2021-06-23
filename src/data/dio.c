
#include "dio.h"

#include "../convert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t dio_reverse_endian_32( uint32_t int_in ) {
   int i = 0;
   uint32_t int_out = 0;

   for( i = 3 ; 0 <= i ; i-- ) {
      int_out <<= 8;
      int_out |= (int_in & 0xff);
      int_in >>= 8;
   }

   return int_out;
}

void dio_print_grid( struct CONVERT_GRID* grid ) {
   size_t x = 0,
      y = 0;
   /* Display the bitmap on the console. */
   dio_printf( "\npreview:\n" );
   for( y = 0 ; grid->sz_y > y ; y++ ) {
      printf( "\n" );
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         if( 0 == grid->data[(y * grid->sz_x) + x] ) {
            printf( " ," );
         } else {
            printf( "%x,", grid->data[(y * grid->sz_x) + x] );
         }
      }
   }
   printf( "\n" );
}

uint32_t dio_read_file( const char* path, uint8_t** buffer_ptr ) {
   FILE* file_in = NULL;
   uint32_t read = 0,
      read_total = 0,
      file_in_sz = 0;
   uint8_t buffer_tmp[DIO_READ_FILE_BLOCK_SZ + 1];

   memset( buffer_tmp, '\0', DIO_READ_FILE_BLOCK_SZ + 1 );

   assert( NULL != buffer_ptr );

   file_in = fopen( path, "rb" );
   assert( NULL != file_in );

   /* Grab and allocate the file size. */
   fseek( file_in, 0, SEEK_END );
   file_in_sz = ftell( file_in );
   fseek( file_in, 0, SEEK_SET );
   *buffer_ptr = calloc( file_in_sz, 1 );
   assert( NULL != *buffer_ptr );

   while(
      read = (fread( buffer_tmp, 1, DIO_READ_FILE_BLOCK_SZ, file_in ))
   ) {
      memcpy( *buffer_ptr + read_total, buffer_tmp, read );
      read_total += read;
   }

   dio_printf( "read %u bytes (vs %u)\n", read_total, file_in_sz );
   assert( read_total == file_in_sz );

   return read_total;
}

void dio_print_binary( uint8_t byte_in ) {
   printf( "bin: %d%d%d%d%d%d%d%d\n",
      byte_in & 0x80 ? 1 : 0,
      byte_in & 0x40 ? 1 : 0,
      byte_in & 0x20 ? 1 : 0,
      byte_in & 0x10 ? 1 : 0,
      byte_in & 0x08 ? 1 : 0,
      byte_in & 0x04 ? 1 : 0,
      byte_in & 0x02 ? 1 : 0,
      byte_in & 0x01 ? 1 : 0 );
}

