
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

#define TILEMAP_FLAG_EDITABLE 0x10

/*! \} */

/**
 * \brief Return value indicating movement is blocked by terrain.
 * \warning This should not overlap with ::MOBILE_ERROR_BLOCKED!
 */
#define TILEMAP_ERROR_BLOCKED -40

/**
 * \relates ::TILESET_TILE
 * \brief TILESET_TILE::flags bit flag indicating tile will block movement.
 */
#define TILESET_FLAG_BLOCK  0x01

/**
 * \relates ::TILESET_TILE
 * \brief TILESET_TILE::flags bit flag indicating player is allowed to grow a
 *        ::CROP_PLOT on this tile.
 */
#define TILESET_FLAG_FARMABLE 0x02

/**
 * \relates ::TILESET_TILE
 * \brief TILESET_TILE::flags bit flag indicating tile is not empty.
 */
#define TILESET_FLAG_ACTIVE 0x80

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

#ifndef IGNORE_DIRTY

#define tilemap_is_dirty( x, y, map ) (((map)->tiles_flags[((y) * TILEMAP_TW) + (x)] & TILEMAP_TILE_FLAG_DIRTY))

#define tilemap_set_dirty( x, y, map ) (map)->tiles_flags[(y * TILEMAP_TW) + x] |= TILEMAP_TILE_FLAG_DIRTY;

#define tilemap_unset_dirty( x, y, map ) (map)->tiles_flags[(y * TILEMAP_TW) + x] &= ~TILEMAP_TILE_FLAG_DIRTY;

#endif /* !IGNORE_DIRTY */

#ifdef RESOURCE_FILE

uint16_t tilemap_fix_asset_path(
   char* path_in, uint16_t path_in_sz, const char* map_path );

#endif /* RESOURCE_FILE */

/**
 * \brief Load tilemap with map_name into the given ::TILEMAP and prepare it
 *        for running in the engine.
 */
int16_t tilemap_load( const char* map_name, struct TILEMAP* t );

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
void tilemap_set_weather( struct TILEMAP* t, uint8_t weather ) SECTION_TILEMAP;

/**
 * \brief Detect potential collision between a MOBILE and TILEMAP tile with
 *        blocking flags.
 * \param dir MOBILE::dir in which the MOBILE is moving.
 * \param t ::MEMORY_PTR to the TILEMAP on which the MOBILE is moving.
 * \return 1 if a collision will occur and 0 otherwise.
 */
int8_t tilemap_collide( 
   uint8_t x, uint8_t y, uint8_t dir, struct TILEMAP* t );

/**
 * \brief Increment the tile_id on the specified tile coordinates. Useful for
 *        editing ::TILEMAP.
 */
void tilemap_advance_tile_id( struct TILEMAP* t, uint16_t x, uint16_t y )
SECTION_TILEMAP;

/**
 * \relates TILEMAP
 * \brief Prepare a TILEMAP for deallocation.
 * \param t ::MEMORY_PTR to a TILEMAP to deinitialize.
 */
void tilemap_deinit( struct TILEMAP* ) SECTION_TILEMAP;

/*! \} */

#  ifdef TILEMAP_C

#     ifdef RESOURCE_FILE

#        include "tmjson.h"
#        include "tmasn.h"

/* Get the real path to the tileset (it's JSON so assume file paths). */
/* Resource IDs would be using pre-parsed maps. */

uint16_t tilemap_fix_asset_path(
   char* path_in, uint16_t path_in_sz, const char* map_path
) {
   uint16_t path_sz_out = 0,
      map_path_sz = 0;

   map_path_sz = memory_strnlen_ptr( map_path, RESOURCE_PATH_MAX );
   path_sz_out = dio_char_idx_r( map_path, map_path_sz, PLATFORM_DIR_SEP );
   if(
      /* Found a map path. */
      0 < path_sz_out &&
      /* Map path fits in buffer with filename, separator, and NULL. */
      path_in_sz > path_sz_out + 2
   ) {
      /* Prepend map directory to tileset name. */
      memory_strncpy_ptr( path_in, map_path, path_sz_out );

      /* Append path separator. */
      path_in[path_sz_out++] = PLATFORM_DIR_SEP;

      /* Add (temporary) NULL terminator. */
      path_in[path_sz_out] = '\0';

      debug_printf( 2, "map directory: %s", path_in );

   } else {
      error_printf( "unable to fit map path into buffer!" );
      path_sz_out = 0;
   }

   return path_sz_out;
}

#     endif /* RESOURCE_FILE */

#  endif /* TILEMAP_C */

#endif /* TILEMAP_H */

