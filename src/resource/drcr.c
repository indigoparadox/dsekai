
#include "../dstypes.h"

/**
 * \brief Allocate a handle to a resource in DRC archive or OS resource fork.
 */
MEMORY_HANDLE resource_get_handle( uint32_t id, RESOURCE_ID drc_type ) {
   struct DIO_STREAM drc_file;
   uint8_t* ptr = NULL;
   int32_t ptr_sz = 0;
   MEMORY_HANDLE handle = NULL;

   if( 0 >= id ) {
      error_printf( "attempted to fetch invalid resource id: %u", id );
      return 0;
   }

   memory_zero_ptr( &drc_file, sizeof( struct DIO_STREAM ) );

   /* Open the DRC archive. */
   dio_open_stream_file( DRC_ARCHIVE, "r", &drc_file );
   if( 0 == dio_type_stream( &drc_file ) ) {
      error_printf( "unable to open archive for tilemap" );
      goto cleanup;
   }

   /* Allocate a handle, lock it, copy in the file, release it. */
   ptr_sz = drc_get_resource_sz( &drc_file, drc_type, id );
   debug_printf( 2, "allocating %d bytes for resource", ptr_sz );
   if( 0 > ptr_sz ) {
      error_printf( "could not find resource %u with type %c%c%c%c in archive",
         id, drc_type.str[0], drc_type.str[1],
         drc_type.str[2], drc_type.str[3] );
      goto cleanup;
   }
   handle = memory_alloc( ptr_sz, 1 );
   if( NULL == handle ) {
      error_printf( "could not allocate space for resource" );
      goto cleanup;
   }
   ptr = memory_lock( handle );
   assert( NULL != ptr );
   drc_get_resource( &drc_file, drc_type, id, ptr, ptr_sz );
   memory_unlock( handle );

cleanup:

   if( 0 < dio_type_stream( &drc_file ) ) {
      dio_close_stream( &drc_file );
   }

   return handle;
}

void* resource_lock_handle( MEMORY_HANDLE handle ) {
   return memory_lock( handle );
}

void* resource_unlock_handle( MEMORY_HANDLE handle ) {
   return memory_unlock( handle );
}

void resource_free_handle( MEMORY_HANDLE handle ) {
   memory_free( handle );
}

