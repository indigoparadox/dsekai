
#include "../memory.h"

#include <stdlib.h>

void* memory_alloc( uint16_t sz, uint16_t count ) {
   return calloc( sz, count );
}

void memory_free( void** ptr_ptr ) {
   if( NULL == ptr_ptr ) {
      return;
   }

   if( NULL == *ptr_ptr ) {
      return;
   }

   free( *ptr_ptr );
   *ptr_ptr = NULL;
}

