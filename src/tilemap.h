
#ifndef TILEMAP_H
#define TILEMAP_H

/*! \file tilemap.h
 *  \brief Structs, functions, and macros pertaining to tilemaps.
 */

#ifndef PACKED
#error "no packed"
#endif

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

/*! \brief TILESET_TILE::flags bit flag indicating tile will block movement. */
#define TILEMAP_TILESET_FLAG_BLOCK  0x01

/*! \brief TILEMAP::tiles_flags bit flag indicating tile must be redrawn. */
#define TILEMAP_TILE_FLAG_DIRTY     0x01

/*! \brief JSON path to script definition. */
#define TILEMAP_JPATH_SCRIPT "/properties/[name=script_%d]/value"
/*! \brief JSON path to string definition. */
#define TILEMAP_JPATH_STRING "/properties/[name=string_%d]/value"
/*! \brief JSON path to tileset source in map data. */
#define TILEMAP_JPATH_TS_SRC     "/tilesets/0/source"
/*! \brief JSON path to terrain tile TILEMAP::tileset index in map data. */
#define TILEMAP_JPATH_TILE       "/layers/[name=terrain]/data/%d"
/*! \brief JSON path to map name. */
#define TILEMAP_JPATH_PROP_NAME  "/properties/[name=name]/value"
/*! \brief JSON path to mobile tile X. */
#define TILEMAP_JPATH_MOB_X      "/layers/[name=mobiles]/objects/%d/x"
/*! \brief JSON path to mobile tile Y. */
#define TILEMAP_JPATH_MOB_Y      "/layers/[name=mobiles]/objects/%d/y"
/*! \brief JSON path to mobile name. */
#define TILEMAP_JPATH_MOB_NAME   "/layers/[name=mobiles]/objects/%d/name"
/*! \brief JSON path to mobile script index. */
#define TILEMAP_JPATH_MOB_SCRIPT "/layers/[name=mobiles]/objects/%d/properties/[name=script]/value"
#define TILEMAP_JPATH_TS_TILE "/tiles/%d/image"

/**
 * \brief Get the TILEMAP::tileset index for the tile at the given coords.
 * \param t TILEMAP from which to get the tile.
 * \param x Tile X coordinate in tiles.
 * \param y Tile Y coordinate in tiles.
 */
#define tilemap_get_tile_id( t, x, y ) ((t->tiles[((y * TILEMAP_TW) + x) / 2] >> (0 == x % 2 ? 4 : 0)) & 0x0f)

struct jsmntok;

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

int16_t tilemap_json_load(
   char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz );

/**
 * \brief Parse tokenized JSON into a tileset array.
 * \param t ::MEMORY_PTR to a TILEMAP load the tileset into.
 * \param tokens JSON tokens to parse.
 * \param tokens_sz Number of tokens to parse.
 * \param json_buffer String buffer containing JSON referred to by tokens.
 * \param iter_path Empty string buffer used to contain JSON path to iterate.
 * \param iter_path_sz Memory size of iter_path.
 * \return 1 if successful, or 0 otherwise.
 */
int16_t tilemap_parse_tileset(
   struct TILEMAP* t, char* ts_name, uint16_t ts_name_sz,
   struct jsmntok* tokens, uint16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz );

/**
 * \brief Parse individual TILEMAP::tileset index from JSON map data.
 * \param tilegrid_path JSON path to tile data to load with %d for tile index.
 * \param tile_idx TILEMAP::tiles index of desired tile in JSON map data.
 * \param tokens JSON tokens to parse.
 * \param tokens_sz Number of tokens to parse.
 * \param json_buffer String buffer containing JSON referred to by tokens.
 * \param json_buffer_sz
 * \return Index that can be looked up in TILEMAP::tileset for image and flags.
 */
int8_t tilemap_json_tile(
   char* tilegrid_path, int16_t tile_idx,
   struct jsmntok* tokens, uint16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz );

/**
 * \brief Load the tiles into TILEMAP::tiles in one go.
 * \return Number of tiles loaded.
 */
int16_t tilemap_json_tilegrid(
   struct TILEMAP* t, char* tilegrid_path,
   char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz );

uint16_t tilemap_json_string(
   char* str_buffer, uint16_t str_buffer_sz,
   char* json_path, uint16_t json_path_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz );

int16_t tilemap_parse(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz );

/**
 * \brief Load tilemap specified by id into TILEMAP struct t.
 * \param id A RESOURCE_ID indicating the tilemap to load from disk.
 * \param t ::MEMORY_PTR to an empty TILEMAP struct to be loaded into.
 * \return
 */
int16_t tilemap_load( RESOURCE_ID id, struct TILEMAP* t );

/**
 * \brief Force a redraw of all tiles on a TILEMAP by marking them dirty.
 * \param t ::MEMORY_PTR to a TILEMAP to redraw.
 */
void tilemap_refresh_tiles( struct TILEMAP* t );

/**
 * \brief Draw currently on-screen portion of a TILEMAP.
 * \param t ::MEMORY_PTR to a TILEMAP to draw.
 * \param state ::MEMORY_PTR to the current engine state, used to determine
 *              what is currently on-screen.
 */
void tilemap_draw( struct TILEMAP* t, struct DSEKAI_STATE* state );
uint8_t tilemap_collide( const struct TILEMAP*, uint8_t, uint8_t );
void tilemap_deinit( struct TILEMAP* );

#endif /* TILEMAP_H */

