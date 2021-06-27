
#include "../memory.h"

#include <PalmOS.h>

void* memory_alloc( uint16_t sz, uint16_t count ) {
   return MemPtrNew( sz * count );
}

void memory_free( void** ptr_ptr ) {
   if( NULL == ptr_ptr ) {
      return;
   }

   if( NULL == *ptr_ptr ) {
      return;
   }

   MemPtrFree( *ptr_ptr );
   *ptr_ptr = NULL;
}

