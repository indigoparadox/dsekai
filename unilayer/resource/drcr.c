
#include "../unilayer.h"

/**
 * \brief Allocate a handle to a resource in DRC archive or OS resource fork.
 */
MEMORY_HANDLE resource_get_handle( RESOURCE_ID id, RESOURCE_TYPE drc_type ) {
   struct DIO_STREAM drc_file;
   MEMORY_FAR_PTR ptr = NULL;
   int32_t ptr_sz = 0;
   MEMORY_HANDLE handle_out = NULL;

   memory_zero_ptr( (MEMORY_PTR)&drc_file, sizeof( struct DIO_STREAM ) );

   /* Open the DRC archive. */
   dio_open_stream_file( DRC_ARCHIVE, "r", &drc_file );
   if( 0 == dio_type_stream( &drc_file ) ) {
      error_printf( "unable to open archive for tilemap" );
      goto cleanup;
   }

   /* Allocate a handle, lock it, copy in the file, release it. */
   ptr_sz = drc_get_resource_sz( &drc_file, drc_type, id );
   if( 0 >= ptr_sz ) {
      error_printf( "could not find resource %u with type %c%c%c%c in archive",
         id, drc_type.str[0], drc_type.str[1],
         drc_type.str[2], drc_type.str[3] );
      goto cleanup;
   }
   debug_printf( 2, "allocating %d bytes for resource", ptr_sz );
   handle_out = memory_alloc( ptr_sz, 1 );
   if( NULL == handle_out ) {
      error_printf( "could not allocate space for resource" );
      goto cleanup;
   }
   ptr = memory_lock( handle_out );
   if( NULL == ptr ) {
      error_printf( "could not lock allocated handle" );
      goto cleanup;
   }
   drc_get_resource( &drc_file, drc_type, id, ptr, ptr_sz );
   ptr = memory_unlock( handle_out );

cleanup:

   if( 0 < dio_type_stream( &drc_file ) ) {
      dio_close_stream( &drc_file );
   }

   return handle_out;
}

RESOURCE_BITMAP_HANDLE resource_get_bitmap_handle( RESOURCE_ID id ) {
   union DRC_TYPE type = DRC_BITMAP_TYPE;
   return resource_get_handle( id, type );
}

RESOURCE_JSON_HANDLE resource_get_json_handle( RESOURCE_ID id ) {
   union DRC_TYPE type = DRC_JSON_TYPE;
   return resource_get_handle( id, type );
}

MEMORY_PTR resource_lock_handle( MEMORY_HANDLE handle ) {
   return memory_lock( handle );
}

MEMORY_PTR resource_unlock_handle( MEMORY_HANDLE handle ) {
   return memory_unlock( handle );
}

void resource_free_handle( MEMORY_HANDLE handle ) {
   memory_free( handle );
}

