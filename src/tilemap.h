
#ifndef TILEMAP_H
#define TILEMAP_H

#include "config.h"
#include "dstypes.h"
#include "graphics.h"

#define TILEMAP_TILESET_FLAG_BLOCK  0x01

#define TILEMAP_TILE_FLAG_DIRTY     0x01

#define tilemap_get_tile_id( t, x, y ) \
   ((t->tiles[y][x / 2] >> (0 == x % 2 ? 4 : 0)) & 0x0f)

struct TILEMAP {
   const char* name;
   const GRAPHICS_TILE (*tileset)[TILEMAP_TILESETS_MAX][TILE_W];
   const GRAPHICS_COLOR (*tileset_colors)[TILEMAP_TILESETS_MAX];
   const uint8_t (*tileset_flags)[TILEMAP_TILESETS_MAX];
   const uint8_t tiles[TILEMAP_TH][TILEMAP_TW / 2];
};

void tilemap_draw(
   const struct TILEMAP* t, uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW]
);
uint8_t tilemap_collide( const struct TILEMAP*, uint8_t, uint8_t );

#endif /* TILEMAP_H */

