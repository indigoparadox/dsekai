
#ifndef MEMORY_H
#define MEMORY_H

#include "dstypes.h"

void* memory_alloc( uint16_t, uint16_t );
void memory_free( void** );

#endif /* MEMORY_H */

