
#include "../memory.h"

#include <Multiverse.h>

void* memory_alloc( uint16_t sz, uint16_t count ) {
   /* TODO: Detect overflow. */
   return NewHandle( (sz * count) );
}

void memory_free( void** ptr_ptr ) {
   if( NULL == ptr_ptr ) {
      return;
   }

   if( NULL == *ptr_ptr ) {
      return;
   }

   DisposeHandle( *ptr_ptr );
   *ptr_ptr = NULL;
}

void* memory_realloc( void* ptr, uint16_t sz ) {
   /* TODO */
   return NULL;
}

