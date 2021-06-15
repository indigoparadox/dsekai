
#ifndef TILEMAP_H
#define TILEMAP_H

#include "dstypes.h"

#define TILEMAP_TW 40
#define TILEMAP_TH 30

struct TILEMAP {
   const char* name;
   const uint8_t tileset[12][8];
   const uint8_t tiles[6][10];
};

#endif /* TILEMAP_H */

