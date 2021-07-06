
#include "../memory.h"

#include <Multiverse.h>

MEMORY_HANDLE memory_alloc( uint16_t sz, uint16_t count ) {
   /* TODO: Detect overflow. */
   return NewHandleClear( (sz * count) );
}

void memory_free( MEMORY_HANDLE handle ) {
   DisposeHandle( handle );
}

uint16_t memory_sz( MEMORY_HANDLE handle ) {
   return GetHandleSize( handle );
}

uint16_t memory_resize( MEMORY_HANDLE handle, uint16_t sz ) {
   SetHandleSize( handle, sz );
   return sz;
}

void memory_copy_ptr( void* dest, const void* src, uint16_t sz ) {
   BlockMove( src, dest, sz );
}

void memory_zero_ptr( void* ptr, uint16_t sz ) {
   memset( ptr, 0, sz );
}

void* memory_lock( MEMORY_HANDLE handle ) {
   HLock( handle );
   return *handle;
}

void* memory_unlock( MEMORY_HANDLE handle ) {
   HUnlock( handle );
   return NULL;
}

char* memory_strncpy_ptr( char* dest, const char* src, uint16_t sz ) {
   return strncpy( dest, src, sz );
}

int16_t memory_strncmp_ptr( const char* s1, const char* s2, uint16_t sz ) {
   return strncmp( s1, s2, sz );
}

int16_t memory_strnlen_ptr( const char* s1, uint16_t sz ) {
   if( 0 < sz ) {
      return strnlen( s1, sz );
   } else {
      return strlen( s1 );
   }
}

