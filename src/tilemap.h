
#ifndef TILEMAP_H
#define TILEMAP_H

/**
 * \addtogroup dsekai_tilemaps Tilemaps
 *
 * \{
 */

/*! \file tilemap.h
 *  \brief Functions and macros pertaining to tilemaps.
 */

/**
 * \addtogroup dsekai_tilemaps_flags Tilemap Flags
 * 
 * \{
 */

/**
 * \brief Bitmask indicating flags that determine ::TILEMAP weather.
 */
#define TILEMAP_FLAG_WEATHER_MASK 0x0f

#define TILEMAP_FLAG_WEATHER_SNOW 0x01
#define TILEMAP_FLAG_WEATHER_RAIN 0x02

/*! \} */

/**
 * \relates ::TILESET_TILE
 * \brief TILESET_TILE::flags bit flag indicating tile will block movement.
 */
#define TILEMAP_TILESET_FLAG_BLOCK  0x01

/**
 * \relates ::TILESET_TILE
 * \brief TILESET_TILE::flags bit flag indicating tile is not empty.
 */
#define TILEMAP_TILESET_FLAG_LOADED 0x80

/**
 * \relates ::TILEMAP
 * \brief TILEMAP::tiles_flags bit flag indicating tile must be redrawn.
 */
#define TILEMAP_TILE_FLAG_DIRTY     0x01

/**
 * \brief Get the TILEMAP::tileset index for the tile at the given coords.
 * \param t ::MEMORY_PTR to TILEMAP from which to get the tile.
 * \param x Tile X coordinate in tiles.
 * \param y Tile Y coordinate in tiles.
 */
#define tilemap_get_tile_id( t, x, y ) (((t)->tiles[((y * TILEMAP_TW) + x) / 2] >> (0 == x % 2 ? 4 : 0)) & 0x0f)

uint16_t tilemap_fix_asset_path(
   char* path_in, uint16_t path_in_sz, const char* map_path );

/**
 * \relates TILEMAP
 * \brief Force a redraw of all tiles on a TILEMAP by marking them dirty.
 * \param t ::MEMORY_PTR to a TILEMAP to redraw.
 */
void tilemap_refresh_tiles( struct TILEMAP* t );

/**
 * \relates TILEMAP
 * \brief Set the \ref dsekai_tilemaps_weather of the given ::TILEMAP,
 *        including starting any related animation effects.
 * \param t ::MEMORY_PTR to a TILEMAP to set the weather of.
 * \param weather Weather-related \ref dsekai_tilemaps_flags.
 *
 * This will start the relevant animation if necessary.
 */
void tilemap_set_weather( struct TILEMAP* t, uint8_t weather );

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

void tilemap_advance_tile_id( struct TILEMAP* t, uint16_t x, uint16_t y );

/**
 * \relates TILEMAP
 * \brief Prepare a TILEMAP for deallocation.
 * \param t ::MEMORY_PTR to a TILEMAP to deinitialize.
 */
void tilemap_deinit( struct TILEMAP* );

/*! \} */

#endif /* TILEMAP_H */

