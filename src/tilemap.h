
#ifndef TILEMAP_H
#define TILEMAP_H

#include "dstypes.h"
#include "graphics.h"

#define TILEMAP_TW 40
#define TILEMAP_TH 30

#define TILEMAP_TILESETS_MAX  12
#define TILEMAP_TILE_W        8

struct TILEMAP {
   const char* name;
   const uint8_t tileset[TILEMAP_TILESETS_MAX][TILEMAP_TILE_W];
   const GRAPHICS_COLOR tileset_colors[TILEMAP_TILESETS_MAX];
   const uint8_t tiles[6][10];
};

void tilemap_draw( const struct TILEMAP* t );

#endif /* TILEMAP_H */

