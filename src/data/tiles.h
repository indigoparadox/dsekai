
#ifndef TILES_H
#define TILES_H

#include "../graphics.h"
#include "../tilemap.h"

/*
These are stored as 2bpp 16x16 pixel CGA images in little-endian order.
This makes a huge difference in performance on a real 8086.
*/

const GRAPHICS_TILE gc_tiles_field[TILEMAP_TILESETS_MAX][TILE_W] = {
   {{0x0, 0x11000000, 0x4000000, 0x4000000, 0x4004, 0x110001, 0x40001, 0x40000, 0x0, 0x0, 0x0, 0x11, 0x4, 0x440004, 0x100000, 0x100000}}, /* Grass */
   {{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}}, 
   {{0x0, 0x545500, 0xdddd01, 0x40777707, 0xc0dddd05, 0x40777707, 0xdddd01, 0x747700, 0xe02e00, 0x800b00, 0x800e00, 0x800b00, 0xa02e00, 0xac203, 0x800, 0x0}},  /* Tree */
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

