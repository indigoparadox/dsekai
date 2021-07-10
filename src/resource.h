
#ifndef RESOURCE_H
#define RESOURCE_H

RESOURCE_BITMAP_HANDLE resource_get_bitmap_handle( RESOURCE_ID );
RESOURCE_JSON_HANDLE resource_get_json_handle( RESOURCE_ID );
void resource_free_handle( MEMORY_HANDLE );
MEMORY_PTR resource_lock_handle( MEMORY_HANDLE );
MEMORY_PTR resource_unlock_handle( MEMORY_HANDLE );

#endif /* RESOURCE_H */

