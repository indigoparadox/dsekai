
#ifndef DRC_H
#define DRC_H

#include "../dstypes.h"

struct DRC_RES {
   uint32_t id;
   uint8_t* data;
   uint32_t data_sz;
};

void drc_add_resource( const char*, uint32_t, uint8_t*, uint32_t );
void drc_remove_resource( const char*, uint32_t );
struct DRC_RES drc_get_resource( const char*, uint32_t );

#endif /* DRC_H */

