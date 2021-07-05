
#include "../memory.h"

#include <Multiverse.h>

MEMORY_HANDLE memory_alloc( uint32_t sz, uint32_t count ) {
   /* TODO: Detect overflow. */
   return NewHandleClear( (sz * count) );
}

void memory_free( MEMORY_HANDLE handle ) {
   DisposeHandle( handle );
}

uint32_t memory_sz( MEMORY_HANDLE handle ) {
   return GetHandleSize( handle );
}

uint32_t memory_resize( MEMORY_HANDLE handle, uint32_t sz ) {
   SetHandleSize( handle, sz );
   return sz;
}

void* memory_lock( MEMORY_HANDLE handle ) {
   HLock( handle );
   return *handle;
}

void* memory_unlock( MEMORY_HANDLE handle ) {
   HUnlock( handle );
   return NULL;
}


