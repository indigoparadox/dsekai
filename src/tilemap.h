
#ifndef TILEMAP_H
#define TILEMAP_H

#define TILEMAP_TILESET_FLAG_BLOCK  0x01

#define TILEMAP_TILE_FLAG_DIRTY     0x01

#define tilemap_get_tile_id( t, x, y ) ((t->tiles[((y * TILEMAP_TW) + x) / 2] >> (0 == x % 2 ? 4 : 0)) & 0x0f)

struct TILEMAP {
   char* name;
   struct GRAPHICS_BITMAP tileset[TILEMAP_TILESETS_MAX];
   uint8_t tileset_flags[TILEMAP_TILESETS_MAX];
   uint8_t tiles[(TILEMAP_TH * TILEMAP_TW) / 2];
   uint8_t tiles_flags[TILEMAP_TH * TILEMAP_TW];
};

int16_t tilemap_load( uint32_t, struct TILEMAP* );
void tilemap_refresh_tiles( struct TILEMAP* );
void tilemap_draw( struct TILEMAP*, struct DSEKAI_STATE* );
uint8_t tilemap_collide( const struct TILEMAP*, uint8_t, uint8_t );
void tilemap_deinit( struct TILEMAP* );

#endif /* TILEMAP_H */

