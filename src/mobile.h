
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

#endif /* MOBILE_H */

