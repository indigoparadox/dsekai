
#include "../unilayer.h"

#include <string.h>

int32_t memory_init() {
   return 1;
}

void memory_shutdown() {
}

MEMORY_HANDLE memory_alloc( uint32_t sz, uint32_t count ) {
   return LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT, sz * count );
}

void memory_free( MEMORY_HANDLE handle ) {
   LocalFree( handle );
}

uint32_t memory_sz( MEMORY_HANDLE handle ) {
   return LocalSize( handle );
}

/**
 * \return New size of handle; either given size or old size if unsuccessful.
 */
uint32_t memory_resize( MEMORY_HANDLE handle, uint32_t sz ) {
   return realloc( handle, sz );
}

void memory_copy_ptr( MEMORY_PTR dest, CONST_MEMORY_PTR src, uint32_t sz ) {
   memcpy( dest, src, sz );
}

void memory_zero_ptr( MEMORY_PTR ptr, uint32_t sz ) {
   memset( ptr, '\0', sz );
}

MEMORY_PTR memory_lock( MEMORY_HANDLE handle ) {
   return LocalLock( handle );
}

MEMORY_PTR memory_unlock( MEMORY_HANDLE handle ) {
   if( LocalUnlock( handle ) ) {
      return 0;
   } else {
      return 1;
   }
}

char* memory_strncpy_ptr( char* dest, const char* src, uint16_t sz ) {
   return strncpy( dest, src, sz );
}

int16_t memory_strncmp_ptr( const char* s1, const char* s2, uint16_t sz ) {
   return strncmp( s1, s2, sz );
}

int16_t memory_strnlen_ptr( const char* s1, uint16_t sz ) {
   return strlen( s1 );
}

