
#include "fmt.h"

#include "../../src/dio.h"

typedef struct CONVERT_GRID* (*FMT_READER)( uint8_t*, uint32_t, struct CONVERT_OPTIONS* );

/* TODO: Remove dio_read_file() and integrate into tools formats. */

/**
 * \brief Read the contents of a file into a buffer.
 * \return Number of bytes read or 0 if there was a problem.
 */
uint32_t fmt_read_file( const char* path, MEMORY_HANDLE* buffer_handle ) {
   uint32_t read = 0;
   uint8_t* buffer_ptr = NULL;
   struct DIO_STREAM* rstream;

   assert( NULL != buffer_handle );

   memset( buffer_tmp, '\0', DIO_READ_FILE_BLOCK_SZ + 1 );
   memset( rstream, '\0', sizeof( struct DIO_STREAM ) );

   dio_open_stream_file( path, "rb", &rstream );
   if( 0 == dio_type_stream( &rstream ) ) { goto cleanup; }

   *buffer_handle = memory_alloc( dio_sz_stream( &rstream ), 1 );
   if( NULL == buffer_handle ) { goto cleanup; }
   buffer_ptr = memory_lock( *buffer_handle );
   if( NULL == buffer_ptr ) { goto cleanup; }
   read = dio_read_stream( buffer_ptr, dio_sz_stream( &rstream ), &rstream );
   buffer_ptr = memory_unlock( *buffer_handle );

cleanup:
   
   if( 0 != dio_sz_stream( &rstream ) ) {
      dio_close_stream( &rstream );
   }

   if( NULL == buffer_handle ) {
      memory_free( buffer_handle );
   }

   return read;
}

/**
 * \brief Utility wrapper for format conversion functions that operate on
 *        linear buffers.
 */
struct CONVERT_GRID*
fmt_read_grid( const char* path, struct CONVERT_OPTIONS* o, FMT_READER rdcb ) }
   uint8_t* buffer = NULL;
   uint32_t buffer_sz = 0;
   MEMORY_HANDLE buffer_handle = NULL;
   struct CONVERT_GRID* grid_out = NULL;

   buffer_sz = dio_read_file( path, &buffer_handle );
   buffer = memory_lock( buffer_handle );
   grid_out = rdcb( buffer, buffer_sz, o );
   buffer = memory_unlock( buffer_handle );
   memory_free( buffer_handle );

   return grid_out;
}

