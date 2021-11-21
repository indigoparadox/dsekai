
#ifndef TMSTRUCTS_H
#define TMSTRUCTS_H

/**
 * \addtogroup dsekai_tilemaps
 *
 * \{
 */

/*! \file tmstruct.h
 *  \brief Structs representing tilemaps and related objects.
 */

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
#define TILEMAP_SPAWNS_MAX 20
#define TILEMAP_SPAWN_NAME_SZ 12
/*! \brief Screen width in tiles. */
#define SCREEN_TW (SCREEN_MAP_W / TILE_W)
/*! \brief Screen height in tiles. */
#define SCREEN_TH (SCREEN_MAP_H / TILE_H)
#define TILEMAP_TS ((TILEMAP_TH * TILEMAP_TW) / 2)

/*! \brief X/Y coordinates of a tile on the current map. */
struct PACKED TILEMAP_COORDS {
   /*! \brief Horizontal coordinate in tiles. */
   uint8_t x;
   /*! \brief Vertical coordinate in tiles. */
   uint8_t y;
};

/*! \brief Tile prototype stored in TILEMAP::tileset. */
struct PACKED TILESET_TILE {
   /*! \brief Indentifier for graphical asset representing this tile. */
   RESOURCE_ID image;
   /*! \brief Flags indicating behavior for this tile. */
   uint8_t flags;
};

/*! \brief Defines a spawner to create mobiles in the world.
 *
 *  Modifications to this struct should also be reflected in tools/map2h.c.
 */
struct PACKED TILEMAP_SPAWN {
   /*! \brief Internal name of a spawned MOBILE. */
   char name[TILEMAP_SPAWN_NAME_SZ];
   /* TODO: Allow a range of coordinates in which to spawn. */
   /* TODO: Allow multiple spawns from single spawner. */
   /*! \brief Tile-based coordinates at which to spawn. */
   struct TILEMAP_COORDS coords;
   /*! \brief RESOURCE_ID for the MOBILE::sprite on a spawned MOBILE. */
   RESOURCE_ID type;
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
   /*! \brief Engine type tilemap is supposed to be used with. */
   uint8_t engine_type;
   /*! \brief Current \ref dsekai_tilemaps_weather. */
   uint8_t weather;
   /*! \brief Array of prototype tiles all map tiles are based on. */
   struct TILESET_TILE tileset[TILEMAP_TILESETS_MAX];
   /*! \brief Array of 4-bit numbers representing tiles composing the map. */
   uint8_t tiles[(TILEMAP_TH * TILEMAP_TW) / 2];
   /*! \brief Special flag bitfields indicating each tile's behavior. */
   uint8_t tiles_flags[TILEMAP_TH * TILEMAP_TW];
   /*! \brief Mobile spawns on this map. */
   struct TILEMAP_SPAWN spawns[TILEMAP_SPAWNS_MAX];
   /*! \brief Strings used in dialog/signs on this map. */
   char strings[TILEMAP_STRINGS_MAX][TILEMAP_STRINGS_SZ];
   /* \brief Scripts available to attach to any MOBILE. */
   struct SCRIPT scripts[TILEMAP_SCRIPTS_MAX];
};

/*! \} */

#endif /* !TMSTRUCTS_H */

