
#ifndef TILEMAP_H
#define TILEMAP_H

/*! \file tilemap.h
 *  \brief Functions and macros pertaining to tilemaps.
 */

#ifndef PACKED
#error "no packed"
#endif

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
/*! \brief JSON path to TILEMAP_SPAWN::type. */
#define TILEMAP_JPATH_MOB_TYPE   "/layers/[name=mobiles]/objects/%d/type"
/*! \brief JSON path to mobile script index. */
#define TILEMAP_JPATH_MOB_SCRIPT "/layers/[name=mobiles]/objects/%d/properties/[name=script]/value"
/*! \brief JSON path to TILESET_TILE::image. */
#define TILEMAP_JPATH_TS_TILE "/tiles/%d/image"
/*! \brief JSON path to TILESET_TILE::flags. */
#define TILEMAP_JPATH_TS_FLAGS "/tiles/%d/properties/[name=flags]/value"

/**
 * \brief Get the TILEMAP::tileset index for the tile at the given coords.
 * \param t TILEMAP from which to get the tile.
 * \param x Tile X coordinate in tiles.
 * \param y Tile Y coordinate in tiles.
 */
#define tilemap_get_tile_id( t, x, y ) ((t->tiles[((y * TILEMAP_TW) + x) / 2] >> (0 == x % 2 ? 4 : 0)) & 0x0f)

struct jsmntok;

#include "tmstruct.h"

/**
 * \brief Prepare a buffer with the relative path of the TILEMAP so that asset
 *        paths can be appended to it.
 * \param path_in Empty buffer to prepare.
 * \param path_in_sz Maximum size of path_in.
 * \param map_path RESOURCE_ID from which TILEMAP was loaded.
 */
uint16_t tilemap_fix_asset_path(
   char* path_in, uint16_t path_in_sz, const char* map_path );

/**
 * \brief Parse tokenized JSON into a tileset array.
 * \param t ::MEMORY_PTR to a TILEMAP load the tileset into.
 * \param tokens JSON tokens to parse.
 * \param tokens_sz Number of tokens to parse.
 * \param json_buffer String buffer containing JSON referred to by tokens.
 * \param json_buffer_sz Size of json_buffer.
 * \param map_path TILEMAP RESOURCE_ID used as search base for assets.
 * \return 1 if successful, or 0 otherwise.
 */
int16_t tilemap_parse_tileset(
   struct TILEMAP* t,
   struct jsmntok* tokens, uint16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   RESOURCE_ID map_path );

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

/**
 * \brief Detect potential collision between a MOBILE and TILEMAP tile with
 *        blocking flags.
 * \param m ::MEMORY_PTR to the MOBILE that is moving.
 * \param dir MOBILE::dir in which the MOBILE is moving.
 * \param t ::MEMORY_PTR to the TILEMAP on which the MOBILE is moving.
 * \return 1 if a collision will occur and 0 otherwise.
 */
uint8_t tilemap_collide( 
   struct MOBILE* m, uint8_t dir, struct TILEMAP* t );

/**
 * \brief Prepare a TILEMAP for deallocation.
 * \param t ::MEMORY_PTR to a TILEMAP to deinitialize.
 */
void tilemap_deinit( struct TILEMAP* );

#endif /* TILEMAP_H */

