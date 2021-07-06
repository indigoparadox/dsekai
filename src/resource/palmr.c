
#include "palmr.h"

MEMORY_HANDLE resource_get_handle( uint32_t id, RESOURCE_ID type ) {
   debug_printf( 2, "loading resource %u of type %u", id, type );

   return DmGetResource( type, id );
}

void* resource_lock_handle( MEMORY_HANDLE handle ) {
   return memory_lock( handle );
}

void* resource_unlock_handle( MEMORY_HANDLE handle ) {
   return memory_unlock( handle );
}

void resource_free_handle( MEMORY_HANDLE handle ) {
   DmReleaseResource( handle );
}

