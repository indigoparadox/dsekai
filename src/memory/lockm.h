
#ifndef LOCKM_H
#define LOCKM_H

struct MEMORY_HANDLE {
   void* ptr_active;
   void* ptr_internal;
   uint8_t locks;
};

#include <stdlib.h>

#define memory_alloc( sz, ct ) \
   calloc( sizeof( struct MEMORY_HANDLE ), 1 ); \
   calloc( sz, ct )
#define memory_free( ptr_ptr ) \
   if( NULL != (ptr_ptr) && NULL != *(ptr_ptr) ) { \
      free( *(ptr_ptr) ); \
      *(ptr_ptr) = NULL; \
   }
#define memory_realloc( ptr, sz ) \
   realloc( ptr, (sz) );

#endif /* LOCKM_H */

