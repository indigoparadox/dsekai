
#ifndef TILEMAP_H
#define TILEMAP_H

#include "dstypes.h"
#include "graphics.h"

#define TILEMAP_TW 20
#define TILEMAP_TH 11

#define TILEMAP_TILESETS_MAX  12
#define TILEMAP_TILE_W        8

struct TILEMAP {
   const char* name;
   const uint8_t (*tileset)[TILEMAP_TILESETS_MAX][TILEMAP_TILE_W];
   const GRAPHICS_COLOR (*tileset_colors)[TILEMAP_TILESETS_MAX];
   const uint8_t tiles[TILEMAP_TH][TILEMAP_TW / 2];
};

void tilemap_draw( const struct TILEMAP* t );

#endif /* TILEMAP_H */

