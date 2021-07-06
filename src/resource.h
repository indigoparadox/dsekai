
#ifndef RESOURCE_H
#define RESOURCE_H

#ifdef PLATFORM_PALM
#include "resource/palmr.h"
#elif defined( PLATFORM_WIN16 )
#include "resource/win16r.h"
#else
#include "resource/drcr.h"
#endif /* PLATFORM_* */

MEMORY_HANDLE resource_get_handle( uint32_t, RESOURCE_ID );
void resource_free_handle( MEMORY_HANDLE );
void* resource_lock_handle( MEMORY_HANDLE );
void* resource_unlock_handle( MEMORY_HANDLE );

#endif /* RESOURCE_H */

