
#include "../memory.h"

#include <PalmOS.h>

MEMORY_HANDLE memory_alloc( uint32_t sz, uint32_t count ) {
   MEMORY_HANDLE handle = NULL;
   uint32_t handle_sz = 0,
      new_sz = 0;
   void* ptr = NULL;

   /* Rollover protection. Naive about negative numbers. */
   if( sz * count < sz ) {
      error_printf( "attempted to request impossible amount of memory" );
      return NULL;
   }

   new_sz = sz * count;
   handle = MemHandleNew( new_sz );
   handle_sz = MemHandleSize( handle );

   if( NULL == handle || new_sz != handle_sz ) {
      error_printf( "failed to allocate %u bytes dynamic heap", sz * count );
      return NULL;
   }
   debug_printf( 2, "allocated %u bytes dynamic heap (%u requested)",
      handle_sz, new_sz );

   ptr = MemHandleLock( handle );
   MemSet( ptr, new_sz, 0 );
   MemHandleUnlock( handle );

   return handle;
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

