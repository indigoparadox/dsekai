
#include "../unilayer.h"

#include <string.h>

MEMORY_HANDLE memory_alloc( uint32_t sz, uint32_t count ) {
   MEMORY_HANDLE handle = NULL;
   uint32_t handle_sz = 0,
      new_sz = 0;
   MEMORY_PTR ptr = NULL;

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

void memory_copy_ptr( MEMORY_PTR dest, CONST_MEMORY_PTR src, uint32_t sz ) {
   memcpy( dest, src, sz );
}

void memory_zero_ptr( MEMORY_PTR ptr, uint32_t sz ) {
   MemSet( ptr, sz, 0 );
}

MEMORY_PTR memory_lock( MEMORY_HANDLE handle ) {
   return MemHandleLock( handle );
}

MEMORY_PTR memory_unlock( MEMORY_HANDLE handle ) {
   MemHandleUnlock( handle );
   return NULL;
}

char* memory_strncpy_ptr( char* dest, const char* src, uint16_t sz ) {
   StrNCopy( dest, src, sz );
   return dest;
}

int16_t memory_strncmp_ptr( const char* s1, const char* s2, uint16_t sz ) {
   return StrNCompare( s1, s2, sz );
}

int16_t memory_strnlen_ptr( const char* s1, uint16_t sz ) {
   int16_t sz_out = 0;
   sz_out = StrLen( s1 );
   if( 0 < sz && sz_out >= sz ) {
      sz_out = sz;
   }
   return sz_out;
}

