
#ifndef MEMORY_H
#define MEMORY_H

#include "unilayer.h"

#ifdef PLATFORM_PALM
#include "memory/palmm.h"
#elif defined( PLATFORM_WIN )
#include "memory/winm.h"
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
   MEMORY_PTR ptr;
   uint32_t ptr_sz;
   uint32_t locks;
   struct FAKE_MEMORY_HANDLE* next;
};

MEMORY_HANDLE memory_alloc( uint32_t, uint32_t );
void memory_free( MEMORY_HANDLE );
uint32_t memory_sz( MEMORY_HANDLE );
uint32_t memory_resize( MEMORY_HANDLE, uint32_t );
void memory_copy_ptr( MEMORY_PTR, CONST_MEMORY_PTR, uint32_t );
void memory_zero_ptr( MEMORY_PTR, uint32_t );
WARN_UNUSED MEMORY_PTR memory_lock( MEMORY_HANDLE );
WARN_UNUSED MEMORY_PTR memory_unlock( MEMORY_HANDLE );
char* memory_strncpy_ptr( char*, const char*, uint16_t );
int16_t memory_strncmp_ptr( const char*, const char*, uint16_t );
int16_t memory_strnlen_ptr( const char*, uint16_t );

#endif /* !MEMORY_H */

