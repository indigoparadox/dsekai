
#ifndef RESOURCE_H
#define RESOURCE_H

#include "dstypes.h"
#include "memory.h"
#include "dio.h"
#include "drc.h"

#ifdef PLATFORM_PALM
#include "resource/palmr.h"
#else
#include "resource/drcr.h"
#endif /* PLATFORM_* */

MEMORY_HANDLE resource_get_handle( uint32_t, union DRC_TYPE );
void resource_free_handle( MEMORY_HANDLE );

#endif /* RESOURCE_H */

