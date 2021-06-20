
#ifndef TILES_H
#define TILES_H

/*
To add a tile:

1. Add the tile image data to tilebmps.h.
2. Add the reference to the image here.
3. Add a resource ID in pamrsc.h.
4. Add a static assignment in data.h.
5. Add an extern ref in data.h.

This needs to be reorganized/automated.
*/

const GRAPHICS_TILE* gc_tiles_field[TILEMAP_TILESETS_MAX] = {
   &gc_tile_field_grass,
   &gc_tile_field_brick_wall,
   &gc_tile_field_tree,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL
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

