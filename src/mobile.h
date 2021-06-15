
#ifndef MOBILE_H
#define MOBILE_H

#include "dstypes.h"

struct MOBILE {
   const uint8_t (*sprites)[8];
   uint8_t tx;
   uint8_t tx_prev;
   uint8_t ty;
   uint8_t ty_prev;
   int8_t steps;
};

uint8_t mobile_walk_start( struct MOBILE*, int8_t x_mod, int8_t y_mod );
void mobile_animate( struct MOBILE* m );

#endif /* MOBILE_H */

