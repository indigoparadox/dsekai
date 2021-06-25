
#ifndef MEMORY_H
#define MEMORY_H

#include "dstypes.h"

#ifdef MEMORY_CALLOC

#include <stdlib.h>

#define memory_alloc( sz, ct ) calloc( sz, ct )
#define memory_free( ptr_ptr ) \
   if( NULL != (ptr_ptr) && NULL != *(ptr_ptr) ) { \
      free( *(ptr_ptr) ); \
      *(ptr_ptr) = NULL; \
   }

#else

void* memory_alloc( uint16_t, uint16_t );
void memory_free( void** );

#endif /* MEMORY_CALLOC */

#endif /* MEMORY_H */

