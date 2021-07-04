
#include "../memory.h"

#include <PalmOS.h>

MEMORY_HANDLE memory_alloc( uint32_t sz, uint32_t count ) {
   /* TODO: Rollover protection. */
   return MemHandleNew( sz * count );
}

void memory_free( MEMORY_HANDLE handle ) {
   MemHandleFree( handle );
}

uint32_t memory_sz( MEMORY_HANDLE handle ) {
   return MemHandleSize( handle );
}

uint32_t memory_resize( MEMORY_HANDLE handle, uint32_t sz ) {
   if( !MemHandleResize( handle, sz ) ) {
      return sz;
   } else {
      return 0;
   }
}

void* memory_lock( MEMORY_HANDLE handle ) {
   return MemHandleLock( handle );
}

void* memory_unlock( MEMORY_HANDLE handle ) {
   MemHandleUnlock( handle );
   return NULL;
}

