
#include "dio.h"

#include "../convert.h"
#include "../memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define PSITOA_BUF_LEN 6 /* 65535 = 5 digits + NULL. */

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

int32_t dio_char_idx( const char* str, int32_t str_sz, char c ) {
   int32_t i = 0;

   for( i = 0 ; str_sz > i ; i++ ) {
      if( str[i] == c ) {
         return i;
      }
   }

   return -1;
}

int32_t dio_char_idx_r( const char* str, int32_t str_sz, char c ) {
   int32_t i = 0;

   for( i = str_sz - 1 ; 0 <= i ; i-- ) {
      if( str[i] == c ) {
         return i;
      }
   }

   return -1;
}

/**
 * @return Index of filename in path string, or -1 if a problem occurred.
 */
int32_t dio_basename( const char* path, uint32_t path_sz ) {
   int32_t retval = -1;
   char* path_tmp = NULL,
      * basename_ptr = NULL;

   path_tmp = memory_alloc( path_sz + 1, 1 );
   memcpy( path_tmp, path, path_sz );

   basename_ptr = strtok( path_tmp, "\\/" );
   while( NULL != basename_ptr ) {
      retval = strlen( path ) - strlen( basename_ptr );
      assert( retval < path_sz );
      basename_ptr = strtok( NULL, "\\/" );
   }

   memory_free( &path_tmp );

   return retval;
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

#ifndef DISABLE_FILESYSTEM

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
   *buffer_ptr = memory_alloc( file_in_sz, 1 );
   assert( NULL != *buffer_ptr );

   while(
      (read = (fread( buffer_tmp, 1, DIO_READ_FILE_BLOCK_SZ, file_in )))
   ) {
      memcpy( *buffer_ptr + read_total, buffer_tmp, read );
      read_total += read;
   }

   dio_printf( "read %u bytes (vs %u)\n", read_total, file_in_sz );
   assert( read_total == file_in_sz );

   return read_total;
}

int32_t dio_copy_file( const char* src, const char* dest ) {
   FILE* file_in = NULL,
      * file_out = NULL;
   uint32_t read = 0,
      wrote = 0,
      read_total = 0,
      wrote_total = 0,
      retval = 0;
   uint8_t buffer_tmp[DIO_READ_FILE_BLOCK_SZ + 1];

   memset( buffer_tmp, '\0', DIO_READ_FILE_BLOCK_SZ + 1 );

   file_in = fopen( src, "rb" );
   if( NULL == file_in ) {
      retval = DIO_ERROR_COULD_NOT_OPEN_FILE;
      goto cleanup;
   }

   file_out = fopen( dest, "wb" );
   if( NULL == file_out ) {
      retval = DIO_ERROR_COULD_NOT_OPEN_FILE;
      goto cleanup;
   }

   while(
      (read = (fread( buffer_tmp, 1, DIO_READ_FILE_BLOCK_SZ, file_in )))
   ) {
      read_total += read;
      wrote = fwrite( buffer_tmp, 1, read, file_out );
      wrote_total += wrote;
      
      dio_printf( "copy: read %d bytes, wrote %d bytes\n", read_total,
         wrote_total );
      if( read_total != wrote_total ) {
         retval = DIO_ERROR_COPY_MISMATCH;
         goto cleanup;
      }
   }

   dio_printf( "copied %d bytes\n", wrote_total );

cleanup:
   if( file_in ) {
      fclose( file_in );
   }
   if( file_out ) {
      fclose( file_out );
   }

   return retval;
}

int32_t dio_move_file( const char* src, const char* dest ) {

   if( rename( src, dest ) ) {
      remove( dest );
      return dio_copy_file( src, dest );
   }

   return 0;
}

#endif /* !DISABLE_FILESYSTEM */

int16_t dio_itoa(
   char* buffer, uint16_t buffer_len, int16_t d, uint8_t d_base
) {
   int16_t idx = 0,
      tmp_idx = 0,
      d_rem = 0,
      d_negative = 0;
   char buffer_tmp[PSITOA_BUF_LEN + 1];

   memset( buffer_tmp, '\0', PSITOA_BUF_LEN + 1 );

   if( 0 == d && tmp_idx + 1 ) {
      buffer[idx++] = '0';
      return idx;
   }

   if( 10 == d_base && 0 > d ) {
      buffer[idx++] = '-';
      d = -d;
      d_negative = 1;
   }

   while( 0 != d && idx + 1 < buffer_len ) {
      d_rem = d % d_base;
      buffer_tmp[tmp_idx++] = 9 < d_rem ? 'a' + (d_rem - 10) : '0' + d_rem;
      d /= d_base;
   }

   if( d_negative && tmp_idx < PSITOA_BUF_LEN ) {
      buffer_tmp[tmp_idx++] = '-';
   }

   if( 0 < tmp_idx ) {
      tmp_idx--;
   }
   
   while(
      0 <= tmp_idx &&
      idx + 1 < buffer_len &&
      '\0' != buffer_tmp[tmp_idx]
   ) {
      buffer[idx++] = buffer_tmp[tmp_idx--];
   }

   return idx;
}

int16_t dio_snprintf(
   char* buffer, int buffer_len, const char* fmt, ...
) {
   va_list args;
   unsigned char c = '\0',
      last = '\0';
   int16_t d = 0,
      idx_in = 0,
      idx_out = 0;

   /* Quick and dirty debug string formatting function. */

   va_start( args, fmt );

   for( idx_in = 0 ; '\0' != fmt[idx_in] && idx_out < buffer_len ; idx_in++ ) {
      c = fmt[idx_in];

      if( '%' == last ) {
         switch( c ) {
         case 'd':
            d = va_arg( args, int );
            idx_out +=
               dio_itoa( &(buffer[idx_out]), buffer_len - idx_out, d, 10 );
            break;

         case 'x':
            d = va_arg( args, int );
            idx_out +=
               dio_itoa( &(buffer[idx_out]), buffer_len - idx_out, d, 16 );
            break;
         }
      } else if( '%' != c ) {
         buffer[idx_out++] = c;
      }

      last = c;
   }

   buffer[idx_out++] = '\0';

   va_end( args );

   return idx_out;
}


