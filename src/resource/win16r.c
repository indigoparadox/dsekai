
#include "../dstypes.h"

extern HINSTANCE g_instance;

MEMORY_HANDLE resource_get_handle( uint32_t id, uint32_t type ) {
   char* buffer = NULL;
   int read = 0;
   HRSRC rsrc = NULL;
   MEMORY_HANDLE handle_out = NULL;

   /* XXX: Resource types aren't being passed in properly? */
   type = DRC_BITMAP_TYPE;

   debug_printf( 2, "loading resource %u of type %u", id, type );
   switch( type ) {
   case DRC_BITMAP_TYPE:
      debug_printf( 2, "as bitmap resource" );
      rsrc = FindResource( g_instance, MAKEINTRESOURCE( id ), RT_BITMAP );
      handle_out = \
         (MEMORY_HANDLE)calloc( 1, sizeof( struct FAKE_MEMORY_HANDLE ) );
      handle_out->ptr_sz = SizeofResource( g_instance, rsrc );
      assert( 0 != handle_out->ptr_sz );
      handle_out->ptr = 
         (MEMORY_PTR)LoadBitmap( g_instance, MAKEINTRESOURCE( id ) );
      break;

   case DRC_MAP_TYPE:
      debug_printf( 2, "as string resource" );
      handle_out = memory_alloc( JSON_BUFFER_SZ + 1, 1 );
      buffer = memory_lock( handle_out );
      read = LoadString(
         g_instance, MAKEINTRESOURCE( id ), &buffer, 0 );
      printf( "XXX %s\n", buffer );
      debug_printf( 2, "string is %d bytes long (out of %d)",
         read, JSON_BUFFER_SZ );
      buffer = memory_unlock( handle_out );
      break;
   }

   return handle_out;
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

