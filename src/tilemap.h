
#ifndef TILEMAP_H
#define TILEMAP_H

#define TILEMAP_TW 40
#define TILEMAP_TH 40
#define TILEMAP_NAME_SZ 32
#define TILEMAP_TILESETS_MAX 12
#define SCREEN_TW (SCREEN_MAP_W / TILE_W)
#define SCREEN_TH (SCREEN_MAP_H / TILE_H)

#define TILEMAP_TILESET_FLAG_BLOCK  0x01

#define TILEMAP_TILE_FLAG_DIRTY     0x01

#define tilemap_get_tile_id( t, x, y ) ((t->tiles[((y * TILEMAP_TW) + x) / 2] >> (0 == x % 2 ? 4 : 0)) & 0x0f)

#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
struct TILESET_TILE {
   uint32_t image;
   uint32_t flags;
};

#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
struct TILEMAP_SPAWN {
   struct TILEMAP_COORDS coords;
   int16_t type;
};

#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
struct TILEMAP {
   char name[TILEMAP_NAME_SZ];
   struct TILESET_TILE tileset[TILEMAP_TILESETS_MAX];
   uint8_t tiles[(TILEMAP_TH * TILEMAP_TW) / 2];
   uint8_t tiles_flags[TILEMAP_TH * TILEMAP_TW];
   struct TILEMAP_SPAWN spawns[20];
};

int16_t tilemap_load( uint32_t, struct TILEMAP* );
void tilemap_refresh_tiles( struct TILEMAP* );
void tilemap_draw( struct TILEMAP*, struct DSEKAI_STATE* );
uint8_t tilemap_collide( const struct TILEMAP*, uint8_t, uint8_t );
void tilemap_deinit( struct TILEMAP* );

#endif /* TILEMAP_H */

