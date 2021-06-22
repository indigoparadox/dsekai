
#ifndef IFF_H
#define IFF_H

#include "../dstypes.h"

struct IFF {
   char type[4];
   uint32_t data_sz;
   uint8_t* data;
};

struct IFF* iff_get_next_segment( const struct IFF*, const char* );
struct IFF* iff_add_segment( struct IFF*, const char*, uint32_t );

#endif /* IFF_H */

