
#include "dsekai.h"

#ifdef RESOURCE_FILE

#include "tmjson.h"
#include "tmasn.h"

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

#endif /* RESOURCE_FILE */

void tilemap_refresh_tiles( struct TILEMAP* t ) {
   int x = 0, y = 0;

   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         t->tiles_flags[(y * TILEMAP_TW) + x] |= TILEMAP_TILE_FLAG_DIRTY;
      }
   }
}

void tilemap_set_weather( struct TILEMAP* t, uint8_t weather ) {

   switch( weather ) {
   case TILEMAP_WEATHER_SNOW:
      debug_printf( 2, "current weather is snowy" );
#ifndef DISABLE_WEATHER_EFFECTS
      animate_create(
         ANIMATE_TYPE_SNOW,
         SCREEN_MAP_X, SCREEN_MAP_Y, SCREEN_MAP_W, SCREEN_MAP_H );
#endif /* !DISABLE_WEATHER_EFFECTS */
      break;

   default:
      debug_printf( 2, "current weather is clear" );
      break;
   }

   t->weather = weather;
}

uint8_t tilemap_collide(
   struct MOBILE* m, uint8_t dir, struct TILEMAP* t
) {
   uint8_t tile_id = 0;
   int16_t x = 0, y = 0;

   x = m->coords.x + gc_mobile_x_offsets[dir];
   y = m->coords.y + gc_mobile_y_offsets[dir];

   tile_id = tilemap_get_tile_id( t, x, y );
   if( t->tileset[tile_id].flags & (uint8_t)TILEMAP_TILESET_FLAG_BLOCK ) {
      return 1;
   }
   return 0;
}

void tilemap_deinit( struct TILEMAP* t ) {
   if( NULL == t ) {
      return;
   }
}

