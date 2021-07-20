
#ifndef RESOURCE_H
#define RESOURCE_H

#if defined RESOURCE_DRC
#include "resource/drcr.h"
#elif defined RESOURCE_HEADER
#include "resource/header.h"
#elif defined( RESOURCE_FILE )
#include "resource/file.h"
#elif defined( PLATFORM_PALM )
#include "resource/palmr.h"
#elif defined( PLATFORM_WIN )
#include "resource/winr.h"
#else
#include "resource/nullr.h"
#endif /* PLATFORM_* */

RESOURCE_BITMAP_HANDLE resource_get_bitmap_handle( RESOURCE_ID );
#ifdef USE_JSON_MAPS
RESOURCE_JSON_HANDLE resource_get_json_handle( RESOURCE_ID );
#endif /* USE_JSON_MAPS */
void resource_free_handle( MEMORY_HANDLE );
MEMORY_PTR resource_lock_handle( MEMORY_HANDLE );
MEMORY_PTR resource_unlock_handle( MEMORY_HANDLE );

#endif /* RESOURCE_H */

