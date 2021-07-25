
#include "../unilayer.h"

static MEMORY_HANDLE resource_get_handle( RESOURCE_ID id, char type[4] ) {
   debug_printf( 2, "loading resource %u of type %u", id, type );

   return DmGetResource( type, id );
}

RESOURCE_BITMAP_HANDLE resource_get_bitmap_handle( RESOURCE_ID id ) {
   return resource_get_handle( id, 'Tbmp' );
}

RESOURCE_BITMAP_HANDLE resource_get_json_handle( RESOURCE_ID id ) {
   return resource_get_handle( id, 'json' );
}

MEMORY_PTR resource_lock_handle( MEMORY_HANDLE handle ) {
   return memory_lock( handle );
}

MEMORY_PTR resource_unlock_handle( MEMORY_HANDLE handle ) {
   return memory_unlock( handle );
}

void resource_free_handle( MEMORY_HANDLE handle ) {
   DmReleaseResource( handle );
}

