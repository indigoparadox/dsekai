
#include "../dstypes.h"

extern HINSTANCE g_instance;

typedef HBITMAP RESOURCE_BITMAP_HANDLE;

RESOURCE_BITMAP_HANDLE resource_get_bitmap_handle( uint32_t id ) {
   debug_printf( 2, "loading bitmap resource %u", id );
   return LoadBitmap( g_instance, MAKEINTRESOURCE( id ) );
}

#ifdef USE_JSON_MAPS
RESOURCE_JSON_HANDLE resource_get_json_handle( uint32_t id ) {
   char* buffer = NULL;
   int read = 0;
   HRSRC rsrc = NULL;
   MEMORY_HANDLE handle_out = NULL;

   debug_printf( 2, "loading json resource %u", id );
   handle_out = memory_alloc( JSON_BUFFER_SZ + 1, 1 );
   buffer = memory_lock( handle_out );
   read = LoadString(
      g_instance, MAKEINTRESOURCE( id ), &buffer, 0 );
   debug_printf( 2, "string is %d bytes long (out of %d)",
      read, JSON_BUFFER_SZ );
   buffer = memory_unlock( handle_out );
}
#endif /* USE_JSON_MAPS */

MEMORY_PTR resource_lock_handle( MEMORY_HANDLE handle ) {
   return memory_lock( handle );
}

MEMORY_PTR resource_unlock_handle( MEMORY_HANDLE handle ) {
   return memory_unlock( handle );
}

void resource_free_handle( MEMORY_HANDLE handle ) {
   memory_free( handle );
}

