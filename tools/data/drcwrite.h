
#ifndef DRCWRITE_H
#define DRCWRITE_H

#include "../../src/data/drc.h"

int32_t drc_create( const char* );
int32_t drc_add_resource( const char*, uint32_t, uint32_t, const char*,
   uint16_t, const uint8_t*, uint32_t );
int32_t drc_remove_resource( const char*, uint32_t, uint32_t );

#endif /* DRCWRITE_H */

