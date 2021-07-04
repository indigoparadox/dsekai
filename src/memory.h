
#ifndef MEMORY_H
#define MEMORY_H

#include "dstypes.h"

#ifdef PLATFORM_PALM
#include "memory/palmm.h"
#elif defined( PLATFORM_MAC7 )
#include "memory/mac7m.h"
#else
#include "memory/fakem.h"
#endif /* PLATFORM_* */

struct FAKE_MEMORY_HANDLE {
#if 0
   uint32_t sz;
   uint32_t offset;
#endif
   void* ptr;
   uint32_t ptr_sz;
   uint32_t locks;
   struct FAKE_MEMORY_HANDLE* next;
};

MEMORY_HANDLE memory_alloc( uint32_t, uint32_t );
void memory_free( MEMORY_HANDLE );
uint32_t memory_sz( MEMORY_HANDLE );
uint32_t memory_resize( MEMORY_HANDLE, uint32_t );
void* memory_lock( MEMORY_HANDLE );
void* memory_unlock( MEMORY_HANDLE );

#endif /* !MEMORY_H */

