
#ifndef TILES_H
#define TILES_H

#include "../graphics.h"
#include "../tilemap.h"

const GRAPHICS_TILE gc_tiles_field[TILEMAP_TILESETS_MAX][TILE_W] = {
   {{0x8, 0x40, 0x0, 0x22, 0x88, 0x0, 0x2, 0x20}},      /* Grass */
   {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}, 
   {{0x3c, 0x6a, 0xd5, 0xeb, 0x7e, 0x18, 0x18, 0x2c}},  /* Tree */
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}, 
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}, 
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}, 
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}, 
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}, 
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}, 
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}, 
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}, 
   {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
};

const uint8_t gc_tiles_field_flags[TILEMAP_TILESETS_MAX] = {
   0x00,
   0x01,
   0x01,
   0x01,
   0x01,
   0x01,
   0x01,
   0x01,
   0x01,
   0x01,
   0x01,
   0x01
};

const GRAPHICS_COLOR gc_tiles_field_colors[TILEMAP_TILESETS_MAX] = {
   GRAPHICS_COLOR_CYAN,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE,
   GRAPHICS_COLOR_WHITE
};

#endif /* TILES_H */

