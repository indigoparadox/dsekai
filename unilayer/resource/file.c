
#define RESOURCE_C
#include "../unilayer.h"

static MEMORY_HANDLE resource_get_handle( RESOURCE_ID id ) {
   FILE* res_file = NULL;
   uint8_t* buffer = NULL;
   int32_t res_sz = 0,
      read = 0;
   MEMORY_HANDLE res_handle = NULL;

   res_file = fopen( id, "rb" );
   if( NULL == res_file ) {
      error_printf( "unable to load resource %s", id );
      return NULL;
   }

   fseek( res_file, 0, SEEK_END );
   res_sz = ftell( res_file );
   fseek( res_file, 0, SEEK_SET );
   if( 0 >= res_sz ) {
      error_printf( "invalid resource size" );
      goto cleanup;
   }

   res_handle = memory_alloc( res_sz, 1 );
   if( NULL == res_handle ) {
      error_printf( "could not allocate resource buffer" );
      goto cleanup;
   }

   buffer = memory_lock( res_handle );
   if( NULL == buffer ) {
      error_printf( "could not lock resource buffer" );
      goto cleanup;
   }

   read = fread( buffer, 1, res_sz, res_file );
   if( read != res_sz ) {
      error_printf( "read invalid resource data (%u bytes vs %u bytes)",
         read, res_sz );
      goto cleanup;
   }

cleanup:

   if( NULL != buffer ) {
      buffer = memory_unlock( res_handle );
   }

   return res_handle;
}

RESOURCE_BITMAP_HANDLE resource_get_bitmap_handle( RESOURCE_ID id ) {
   return resource_get_handle( id );
}

RESOURCE_JSON_HANDLE resource_get_json_handle( RESOURCE_ID id ) {
   return resource_get_handle( id );
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

