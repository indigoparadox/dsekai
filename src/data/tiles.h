
#ifndef TILES_H
#define TILES_H

/*
To add a tile:

1. Create a tile image.
2. Add its basename to a tile here.
3. drcpack should pick it up and create the header during make.

This needs to be reorganized/automated.
*/

const struct TILESET_TILE gc_tiles_field[TILEMAP_TILESETS_MAX] = {
   { tile_field_grass,      0x00 },
   { tile_field_brick_wall, 0x01 },
   { tile_field_tree,       0x01 },
   { tile_pillar, 0x01 },
   { tile_pool, 0x01 },
   { 0, 0x01 },
   { 0, 0x01 },
   { 0, 0x01 },
   { 0, 0x01 },
   { 0, 0x01 },
   { 0, 0x01 },
   { 0, 0x01 }
};

#endif /* TILES_H */

