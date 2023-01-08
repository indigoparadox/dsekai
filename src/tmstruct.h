
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
/*! \brief Maximum TILEMAP width in tiles. */
#define TILEMAP_TW 40
/*! \brief Maximum TILEMAP height in tiles. */
#define TILEMAP_TH 40
/*! \brief TILEMAP::tileset maximum length. */
#define TILEMAP_TILESETS_MAX 12
#define TILEMAP_SPAWN_NAME_SZ 12
/*! \brief Screen width in tiles. */
#define SCREEN_TW (SCREEN_MAP_W / TILE_W)
/*! \brief Screen height in tiles. */
#define SCREEN_TH (SCREEN_MAP_H / TILE_H)
#define TILEMAP_TS ((TILEMAP_TH * TILEMAP_TW) / 2)

/*! \brief X/Y coordinates of a tile on the current map. */
struct TILEMAP_COORDS {
   /*! \brief Horizontal coordinate in tiles. */
   uint8_t x;
   /*! \brief Vertical coordinate in tiles. */
   uint8_t y;
};

/*! \brief Tile prototype stored in TILEMAP::tileset. */
struct TILESET_TILE {
   /*! \brief Indentifier for graphical asset representing this tile. */
   RESOURCE_NAME image_name;
   unsigned char ascii;
   /*! \brief Flags indicating behavior for this tile. */
   uint8_t flags;
   /**
    * \brief Index of the tile image loaded in the
    *        \ref unilayer_graphics_cache.
    */
   uint16_t image_cache_id;
};

/* TODO: Move spawner to mostruct.h for consistency. */
/*! \brief Defines a spawner to create mobiles in the world.
 *
 *  Modifications to this struct should also be reflected in tools/map2h.c.
 */
struct TILEMAP_SPAWN {
   /*! \brief Internal name of a spawned MOBILE. */
   char name[TILEMAP_SPAWN_NAME_SZ];
   /* TODO: Allow a range of coordinates in which to spawn. */
   /* TODO: Allow multiple spawns from single spawner. */
   /*! \brief Tile-based coordinates at which to spawn. */
   struct TILEMAP_COORDS coords;
   /**
    * \brief RESOURCE_NAME for the MOBILE::sprite on a spawned MOBILE.
    */
   RESOURCE_NAME sprite_name;
   unsigned char ascii;
   /*! \brief \ref dsekai_mobile_flags to pass to spawned MOBILE::flags. */
   uint16_t flags;
   /**
    * \brief Unique identifying number for this spawner loaded from tilemap.
    *
    * Passed to MOBILE::spawner_gid on spawned ::MOBILE objects.
    */
   uint16_t gid;
   /*! \brief Index of TILEMAP::scripts attached to mobiles spawned. */
   int16_t script_id;
};

/*! \brief Information pertaining to in-game world currently loaded.
 *
 *  Modifications to this struct should also be reflected in tools/map2h.c.
 */
struct TILEMAP {
   /*! \brief Tilemap name. */
   char name[TILEMAP_NAME_MAX];
   /*! \brief Current \ref dsekai_tilemaps_flags. */
   uint8_t flags;
   /**
    * \brief Globally unique ID for this tilemap.
    *
    * \attention This should not be set to ::MOBILE_MAP_GID_ALL, or weird
    *            things will happen!
    */
   uint16_t gid;
   /*! \brief Array of prototype tiles all map tiles are based on. */
   struct TILESET_TILE tileset[TILEMAP_TILESETS_MAX];
   /*! \brief Array of 4-bit numbers representing tiles composing the map. */
   uint8_t tiles[(TILEMAP_TH * TILEMAP_TW) / 2];
   /*! \brief Special flag bitfields indicating each tile's behavior. */
   uint8_t tiles_flags[TILEMAP_TH * TILEMAP_TW];
   /*! \brief Mobile spawns on this map. */
   struct TILEMAP_SPAWN spawns[TILEMAP_SPAWNS_MAX];
   /*! \brief Strings used in dialog/signs on this map. */
   char strpool[TILEMAP_STRPOOL_SZ];
   /* \brief Scripts available to attach to any MOBILE. */
   struct SCRIPT scripts[TILEMAP_SCRIPTS_MAX];
   /*! \brief Items able to be collected on this map. */
   struct ITEM item_defs[TILEMAP_ITEMS_MAX];
   /*! \brief Crops able to be grown on this map. */
   struct CROP_DEF crop_defs[TILEMAP_CROP_DEFS_MAX];
};

/*! \} */

#endif /* !TMSTRUCTS_H */

