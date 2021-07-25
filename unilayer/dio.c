
#include "unilayer.h"

#include <stdarg.h>
#include <stdlib.h> /* for getenv() */
#include <string.h> /* for strtok() */

#define PSITOA_BUF_LEN 6 /* 65535 = 5 digits + NULL. */

int32_t g_next_stream_id = 0;

struct CONVERT_GRID;

#ifndef DISABLE_FILESYSTEM

int32_t dio_open_stream_file(
   const char* path, const char* mode, struct DIO_STREAM* stream
) {
   debug_printf( 2, "opening file %s mode %s as stream %d...",
      path, mode, g_next_stream_id );

   assert( 0 == stream->type );
   
   stream->buffer.file = fopen( path, mode );
   if( NULL == stream->buffer.file ) {
      return DIO_ERROR_COULD_NOT_OPEN_FILE;
   }

   /* Grab the file size for later */
   fseek( stream->buffer.file, 0, SEEK_END );
   stream->buffer_sz = ftell( stream->buffer.file );
   fseek( stream->buffer.file, 0, SEEK_SET );

   debug_printf( 2, "file is %d bytes", stream->buffer_sz );

   stream->id = g_next_stream_id;
   stream->type = DIO_STREAM_FILE;
   stream->position = 0;

   g_next_stream_id++;

   return stream->buffer_sz;
}

#endif /* DIO_STREAM_FILE */

int32_t dio_open_stream_buffer(
   uint8_t* buf, uint32_t buf_sz, struct DIO_STREAM* stream
) {
   debug_printf( 2, "opening buffer as stream %d...", g_next_stream_id );

   assert( 0 == stream->type );

   stream->buffer.bytes = buf;
   stream->buffer_sz = buf_sz;
   stream->type = DIO_STREAM_BUFFER;
   stream->id = g_next_stream_id;
   stream->position = 0;

   g_next_stream_id++;

   return buf_sz;
}

void dio_close_stream( struct DIO_STREAM* stream ) {
   if( NULL == stream || 0 == stream->type ) {
      return;
   }

   debug_printf( 2, "closing stream %d...", stream->id );

   switch( stream->type ) {
#ifndef DISABLE_FILESYSTEM
   case DIO_STREAM_FILE:
      fclose( stream->buffer.file );
      break;
#endif /* !DISABLE_FILESYSTEM */

   default:
      break;
   }

   /* TODO: Free buffer? */

   stream->type = 0;
   stream->id = -1;
   stream->buffer_sz = 0;
}

int32_t dio_seek_stream( struct DIO_STREAM* stream, int32_t seek, uint8_t m ) {

   dio_assert_stream( stream );

   switch( m ) {
   case SEEK_SET:
      debug_printf( 0, "seeking stream %d to %d...", stream->id, seek );
      assert( 0 <= seek );
      assert( stream->buffer_sz >= seek );
      if( seek >= stream->buffer_sz ) {
         error_printf( "attempted to seek beyond stream %d end",
            stream->id);
         return -1;
      }
      stream->position = seek;
      break;

   case SEEK_CUR:
      debug_printf( 0, "seeking stream %d by +%d...", stream->id, seek );
      assert( stream->buffer_sz > stream->position + seek );
      if( seek >= stream->buffer_sz || stream->position + seek < 0 ) {
         error_printf( "attempted to seek beyond stream %d end",
            stream->id);
         return -1;
      }
      stream->position += seek;
      break;

   case SEEK_END:
      debug_printf( 0, "seeking stream %d to %d from end...",
         stream->id, seek );
      assert( 0 <= stream->buffer_sz - 1 - seek );
      if( 
         stream->buffer_sz - 1 - seek < 0 ||
         stream->buffer_sz - 1 - seek >= stream->buffer_sz
      ) {
         error_printf( "attempted to seek beyond stream %d end",
            stream->id);
         return -1;
      }
      stream->position = stream->buffer_sz - 1 - seek;
      break;
   }

   switch( stream->type ) {
#ifndef DISABLE_FILESYSTEM
   case DIO_STREAM_FILE:
      fseek( stream->buffer.file, seek, m );
      return ftell( stream->buffer.file );
#endif /* !DIO_STREAM_FILE */

   case DIO_STREAM_BUFFER:
      /* This was handled by setting the position above. */
      return stream->position;
   }

   return -1;
}

int32_t dio_tell_stream( struct DIO_STREAM* stream ) {
   dio_assert_stream( stream );
   return stream->position;
}

int32_t dio_sz_stream( struct DIO_STREAM* stream ) {
   dio_assert_stream( stream );
   return stream->buffer_sz;
}

int32_t dio_position_stream( struct DIO_STREAM* stream ) {
   dio_assert_stream( stream );
   return stream->position;
}

uint8_t dio_type_stream( struct DIO_STREAM* stream ) {
   return stream->type;
}

int32_t dio_read_stream( MEMORY_PTR dest, uint32_t sz, struct DIO_STREAM* src ) {
   int32_t sz_out = 0;

   dio_assert_stream( src );

   assert( NULL != dest );

   debug_printf( 0, "reading %d bytes at %d (out of %d)...",
      sz, src->position, src->buffer_sz );

   /* assert( src->position + sz < src->buffer_sz );
   assert( src->position + sz >= 0 ); */

   switch( src->type ) {
#ifndef DISABLE_FILESYSTEM
   case DIO_STREAM_FILE:
      sz_out = fread( dest, 1, sz, src->buffer.file );
      src->position += sz_out;
      /* assert( ftell( src->buffer.file ) == src->position ); */
      debug_printf( 0, "read complete" );
      return sz_out;
#endif /* !DISABLE_FILESYSTEM */

   case DIO_STREAM_BUFFER:
      memory_copy_ptr( (MEMORY_PTR)dest,
         (MEMORY_PTR)&(src->buffer.bytes[src->position]), sz );
      src->position += sz;
      return sz;

   default:
      return DIO_ERROR_INVALID_STREAM;
   }
}

int32_t dio_write_stream(
   const MEMORY_PTR src, uint32_t sz, struct DIO_STREAM* dest
) {
   int32_t sz_out = 0;

   dio_assert_stream( dest );

   debug_printf( 0, "writing %d bytes at %d (out of %d)...",
      sz, dest->position, dest->buffer_sz );

   assert( dest->position + sz >= dest->position );
   
   switch( dest->type ) {
#ifndef DISABLE_FILESYSTEM
   case DIO_STREAM_FILE:
      sz_out = fwrite( src, 1, sz, dest->buffer.file );
      if( dest->position + sz_out >= dest->buffer_sz ) {
         dest->buffer_sz = dest->position + sz_out;
      }
      dest->position += sz_out;
      assert( ftell( dest->buffer.file ) == dest->position );
      return sz_out;
#endif /* !DISABLE_FILESYSTEM */

   case DIO_STREAM_BUFFER:
      assert( dest->position + sz < dest->buffer_sz );
      memory_copy_ptr( &(dest->buffer.bytes[dest->position]), src, sz );
      dest->position += sz;
      return sz;
   }
   return -1;
}

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

#ifdef USE_DIO_PRINT_GRID

void dio_print_grid( struct CONVERT_GRID* grid ) {
   size_t x = 0,
      y = 0;
   /* Display the bitmap on the console. */
   dio_printf( "\npreview:\n" );
   for( y = 0 ; grid->sz_y > y ; y++ ) {
      dio_printf( "\n" );
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         if( 0 == grid->data[(y * grid->sz_x) + x] ) {
            dio_printf( " ," );
         } else {
            dio_printf( "%x,", grid->data[(y * grid->sz_x) + x] );
         }
      }
   }
   dio_printf( "\n" );
}

#endif /* USE_DIO_PRINT_GRID */

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

int16_t dio_mktemp_path( char* buf, uint16_t buf_sz, const char* tmpfilename ) {
   const char* temp_dir;

   temp_dir = getenv( "TEMP" );
   if( NULL == temp_dir ) {
      temp_dir = DIO_PATH_TEMP;
   }

   if(
      memory_strnlen_ptr( temp_dir, buf_sz ) + 1 +
      memory_strnlen_ptr( tmpfilename, buf_sz ) >= buf_sz
   ) {
      error_printf( "temporary path too long" );
      return -1;
   }

   memory_zero_ptr( buf, buf_sz );
   memory_strncpy_ptr( buf, temp_dir, buf_sz );

   buf[memory_strnlen_ptr( temp_dir, 0 )] = DIO_PATH_SEP;
   memory_strncpy_ptr( &(buf[memory_strnlen_ptr( temp_dir, 0 ) + 1]),
      tmpfilename, buf_sz - memory_strnlen_ptr( temp_dir + 1, 0 ) );

   return memory_strnlen_ptr( buf, buf_sz );
}

/**
 * @return Index of filename in path string, or -1 if a problem occurred.
 */
int32_t dio_basename( const char* path, uint32_t path_sz ) {
   int32_t retval = -1;
   char* path_tmp = NULL,
      * basename_ptr = NULL;
   MEMORY_HANDLE handle = NULL;

   handle = memory_alloc( path_sz + 1, 1 );
   path_tmp = (char*)memory_lock( handle );
   /* XXX */
   memory_copy_ptr( (MEMORY_PTR)path_tmp, (MEMORY_PTR)path, path_sz );

   basename_ptr = strtok( path_tmp, "\\/" );
   while( NULL != basename_ptr ) {
      retval = memory_strnlen_ptr( path, 0 ) -
      memory_strnlen_ptr( basename_ptr, 0 );
      assert( retval < path_sz );
      basename_ptr = strtok( NULL, "\\/" );
   }

   path_tmp = (char*)memory_unlock( handle );
   memory_free( handle );

   return retval;
}

/* TODO: Remove dio_read_file() and integrate into tools formats. */

uint32_t dio_read_file( const char* path, MEMORY_HANDLE* buffer_handle ) {
   FILE* file_in = NULL;
   uint32_t read = 0,
      read_total = 0,
      file_in_sz = 0;
   uint8_t buffer_tmp[DIO_READ_FILE_BLOCK_SZ + 1];
   uint8_t* buffer_ptr = NULL;

   memory_zero_ptr( buffer_tmp, DIO_READ_FILE_BLOCK_SZ + 1 );

   assert( NULL != buffer_handle );

   file_in = fopen( path, "rb" );
   assert( NULL != file_in );

   /* Grab and allocate the file size. */
   fseek( file_in, 0, SEEK_END );
   file_in_sz = ftell( file_in );
   fseek( file_in, 0, SEEK_SET );
   *buffer_handle = memory_alloc( file_in_sz, 1 );
   assert( NULL != *buffer_handle );
   buffer_ptr = memory_lock( *buffer_handle );

   while(
      (read = (fread( buffer_tmp, 1, DIO_READ_FILE_BLOCK_SZ, file_in )))
   ) {
      memory_copy_ptr( (MEMORY_PTR)(buffer_ptr + read_total),
         (MEMORY_PTR)buffer_tmp, read );
      read_total += read;
   }

   debug_printf( 2, "read %u bytes (vs %u)", read_total, file_in_sz );
   assert( read_total == file_in_sz );

   buffer_ptr = memory_unlock( *buffer_handle );

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

   memory_zero_ptr( buffer_tmp, DIO_READ_FILE_BLOCK_SZ + 1 );

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
      
      debug_printf( 1, "copy: read %d bytes, wrote %d bytes", read_total,
         wrote_total );
      if( read_total != wrote_total ) {
         retval = DIO_ERROR_COPY_MISMATCH;
         goto cleanup;
      }
   }

   debug_printf( 2, "copied %d bytes", wrote_total );

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

int32_t dio_remove_file( const char* path ) {
   remove( path );
   return 0;
}

#endif /* !DISABLE_FILESYSTEM */

int16_t dio_strnchr( const char* buffer, uint16_t buffer_len, char c ) {
   int16_t i = 0;

   for( i = 0 ; buffer_len > i ; i++ ) {
      if( buffer[i] == c ) {
         return i;
      }
   }

   return -1;
}

int16_t dio_itoa(
   char* buffer, uint16_t buffer_len, int16_t d, uint8_t d_base
) {
   int16_t idx = 0,
      tmp_idx = 0,
      d_rem = 0,
      d_negative = 0;
   char buffer_tmp[PSITOA_BUF_LEN + 1];

   memory_zero_ptr( buffer_tmp, PSITOA_BUF_LEN + 1 );

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

int16_t dio_atoi( const char* a, int base ) {
   uint16_t idx = 0;
   int16_t out = 0;

   /* TODO: Handle negatives. */
   while( a[idx] >= 48 && a[idx] <= 57 ) {
      out *= base;
      out += (a[idx] - 48);
      idx++;
   }

   return out;
}

int16_t dio_strncmp( const char* a, const char* b, uint16_t len, char sep ) {
   int i = 0;

   while( len > i && sep != a[i] && sep != b[i] ) {
      if( a[i] != b[i] ) {
         return a[i] - b[i];
      }

      i++;
   }

   return 0;
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

