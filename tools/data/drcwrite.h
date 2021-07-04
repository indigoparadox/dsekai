
#ifndef DRCWRITE_H
#define DRCWRITE_H

#include "../../src/drc.h"

int32_t drc_create( struct DIO_STREAM* );
int32_t drc_add_resource( struct DIO_STREAM*, struct DIO_STREAM*,
   union DRC_TYPE, uint32_t, const char*, uint16_t, const uint8_t*, uint32_t );
int32_t drc_remove_resource( struct DIO_STREAM*, struct DIO_STREAM*,
   union DRC_TYPE, uint32_t );

#endif /* DRCWRITE_H */

