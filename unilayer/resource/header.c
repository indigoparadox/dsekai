
#define RESOURCE_C
#include "../unilayer.h"

#include <resext.h>

RESOURCE_BITMAP_HANDLE resource_get_bitmap_handle( RESOURCE_ID id ) {
   return gsc_resources[id];
}

RESOURCE_JSON_HANDLE resource_get_json_handle( RESOURCE_ID id ) {
   return gsc_resources[id];
}

MEMORY_PTR resource_lock_handle( MEMORY_HANDLE handle ) {
   return handle;
}

MEMORY_PTR resource_unlock_handle( MEMORY_HANDLE handle ) {
   return handle;
}

void resource_free_handle( MEMORY_HANDLE handle ) {
}

