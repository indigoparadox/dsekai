
#include "../dstypes.h"

extern HINSTANCE g_instance;

MEMORY_HANDLE resource_get_handle( uint32_t id, RESOURCE_ID type ) {
   MEMORY_HANDLE handle_out = NULL;
   char* buffer = NULL;
   int read = 0;
   HRSRC rsrc = NULL;

   debug_printf( 2, "loading resource %u of type %u", id, type );
   switch( type ) {
   case DRC_BITMAP_TYPE:
      debug_printf( 2, "as bitmap resource" );
      return LoadBitmap( g_instance, MAKEINTRESOURCE( id ) );

   case DRC_MAP_TYPE:
      debug_printf( 2, "as string resource" );
      handle_out = memory_alloc( JSON_BUFFER_SZ + 1, 1 );
      buffer = memory_lock( handle_out );
      read = LoadString(
         g_instance, MAKEINTRESOURCE( id ), buffer, JSON_BUFFER_SZ );
      debug_printf( 2, "string is %d bytes long (out of %d)",
         read, JSON_BUFFER_SZ );
      buffer = memory_unlock( handle_out );
      return handle_out;
   }
   return NULL;
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

