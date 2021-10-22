
#ifndef TMSTRUCTS_H
#define TMSTRUCTS_H

#include "scstruct.h"

/*! \brief Maximum number of SCRIPT structs attached to a tilemap. */
#define TILEMAP_SCRIPTS_MAX 8
/*! \brief TILESET_TILE width in pixels. */
#define TILE_W 16
/*! \brief TILESET_TILE height in pixels. */
#define TILE_H 16
/*! \brief Maximum TILEMAP width in tiles. */
#define TILEMAP_TW 40
/*! \brief Maximum TILEMAP height in tiles. */
#define TILEMAP_TH 40
/*! \brief TILEMAP::name maxium length. */
#define TILEMAP_NAME_MAX 32
/*! \brief TILEMAP::tileset maximum length. */
#define TILEMAP_TILESETS_MAX 12
/*! \brief TILEMAP::strings maximum number of strings. */
#define TILEMAP_STRINGS_MAX 20
/*! \brief Maximum length of each string in TILEMAP::strings. */
#define TILEMAP_STRINGS_SZ 128
/*! \brief Maximum number of TILEMAP::spawns. */
#define TILEMAP_SPAWN_T_MAX 32
/*! \brief Screen width in tiles. */
#define SCREEN_TW (SCREEN_MAP_W / TILE_W)
/*! \brief Screen height in tiles. */
#define SCREEN_TH (SCREEN_MAP_H / TILE_H)

/*! \brief X/Y coordinates of a tile on the current map. */
struct PACKED TILEMAP_COORDS {
   int32_t x;
   int32_t y;
};

/*! \brief Tile prototype stored in TILEMAP::tileset. */
struct PACKED TILESET_TILE {
   /*! \brief Indentifier for graphical asset representing this tile. */
   RESOURCE_ID image;
   /*! \brief Flags indicating behavior for this tile. */
   uint32_t flags;
};

/*! \brief Defines a spawner to create mobiles in the world.
 *
 *  Modifications to this struct should also be reflected in tools/map2h.c.
 */
struct PACKED TILEMAP_SPAWN {
   /* TODO: Allow a range of coordinates in which to spawn. */
   /* TODO: Allow multiple spawns from single spawner. */
   /*! \brief Tile-based coordinates at which to spawn. */
   struct TILEMAP_COORDS coords;
   /*! \brief Mobile type to spawn. */
   int16_t type;
   /*! \brief Index of TILEMAP::scripts attached to mobiles spawned. */
   int16_t script_id;
};

/*! \brief Information pertaining to in-game world currently loaded.
 *
 *  Modifications to this struct should also be reflected in tools/map2h.c.
 */
struct PACKED TILEMAP {
   /*! \brief Tilemap name. */
   char name[TILEMAP_NAME_MAX];
   /*! \brief Array of prototype tiles all map tiles are based on. */
   struct TILESET_TILE tileset[TILEMAP_TILESETS_MAX];
   /*! \brief Array of tiles composing the map. */
   uint8_t tiles[(TILEMAP_TH * TILEMAP_TW) / 2];
   /*! \brief Special flag bitfields indicating each tile's behavior. */
   uint8_t tiles_flags[TILEMAP_TH * TILEMAP_TW];
   /*! \brief Mobile spawns on this map. */
   struct TILEMAP_SPAWN spawns[20];
   /*! \brief Strings used in dialog/signs on this map. */
   char strings[TILEMAP_STRINGS_MAX][TILEMAP_STRINGS_SZ];
   /*! \brief Size of each loaded string. */
   uint8_t string_szs[TILEMAP_STRINGS_MAX];
   /*! \brief Number of TILEMAP::strings active on this map. */
   uint8_t strings_count;
   /*! \brief Number of TILEMAP::spawns active on this map. */
   uint16_t spawns_count;
   /* \brief Scripts available to attach to any MOBILE. */
   struct SCRIPT scripts[TILEMAP_SCRIPTS_MAX];
   /* \brief Number of TILEMAP::scripts currently available. */
   uint16_t scripts_count;
};

#endif /* !TMSTRUCTS_H */
