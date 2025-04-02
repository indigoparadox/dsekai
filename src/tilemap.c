
#define TILEMAP_C
#include "dsekai.h"

#ifndef RESOURCE_FILE

extern MAUG_CONST char gc_map_names[][TILEMAP_NAME_MAX];
extern MAUG_CONST struct TILEMAP* gc_map_structs[];
extern MAUG_CONST uint8_t gc_map_count;

#endif /* !RESOURCE_FILE */

int16_t tilemap_load( const char* map_name, struct TILEMAP* t ) {
   int16_t retval = 0,
      i = 0;
   retroflat_asset_path map_load_path;

   /* TODO: Work this into resource API in a modular fashion. */
#  ifdef TILEMAP_FMT_JSON
   maug_mzero( map_load_path, RETROFLAT_PATH_MAX );
   maug_snprintf(
      map_load_path, RETROFLAT_PATH_MAX,
      "m_%s.json", map_name );
   retval = tilemap_json_load( map_load_path, t );
#  elif defined TILEMAP_FMT_ASN
   dio_snprintf(
      map_load_path, RETROFLAT_PATH_MAX,
      "m_%s.asn", map_name );
   retval = tilemap_asn_load_res( map_load_path, t );
#  else
   /* Can't load a map for the engine; we're probably in map2asn. */
   retval = 0;
#  endif

   if( 0 >= retval ) {
      goto cleanup;
   }

   /* Preload tilemap tileset tiles into graphics cache. */
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      /* Skip inactive tiles. */
      if(
         TILESET_FLAG_ACTIVE != (t->tileset[i].flags & TILESET_FLAG_ACTIVE)
      ) {
         continue;
      }

      debug_printf( 2, "preloading tile %d image...", i );
      t->tileset[i].image_cache_id = retrogxc_load_bitmap(
         t->tileset[i].image_name, 0 );
      maug_cleanup_if_lt( t->tileset[i].image_cache_id, 0, "%d", -1 );
   }

cleanup:

   return retval;
}

void tilemap_refresh_tiles( struct TILEMAP* t ) {
   int8_t x = 0, y = 0;

   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         t->tiles_flags[(y * TILEMAP_TW) + x] |= TILEMAP_TILE_FLAG_DIRTY;
      }
   }
}

void tilemap_set_weather( uint8_t weather, struct TILEMAP* t ) {

   if(
      TILEMAP_FLAG_WEATHER_SNOW ==
      (TILEMAP_FLAG_WEATHER_MASK & weather)
   ) {
      debug_printf( 2, "current weather is snowy" );
#ifndef DISABLE_WEATHER_EFFECTS
/* TODO
      retroani_create(
         RETROANI_TYPE_SNOW, ANIMATE_FLAG_WEATHER | ANIMATE_FLAG_FG,
         SCREEN_MAP_X(), SCREEN_MAP_Y(), SCREEN_MAP_W(), SCREEN_MAP_H() );
*/
#endif /* !DISABLE_WEATHER_EFFECTS */
   } else {
      debug_printf( 2, "current weather is clear" );
   }

   t->flags |= (TILEMAP_FLAG_WEATHER_MASK & weather);
}

int8_t tilemap_collide(
   uint8_t x, uint8_t y, uint8_t dir, struct TILEMAP* t
) {
   uint8_t tile_id = 0;
   int16_t x_test = 0, y_test = 0;
   int8_t retval = 0;

   assert( dir < 4 );

   x_test = x + gc_mobile_x_offsets[dir];
   y_test = y + gc_mobile_y_offsets[dir];

   tile_id = tilemap_get_tile_id( t, x_test, y_test );
   if( t->tileset[tile_id].flags & (uint8_t)TILESET_FLAG_BLOCK ) {
      retval = TILEMAP_ERROR_BLOCKED;
   }
   return retval;
}

void tilemap_advance_tile_id( struct TILEMAP* t, uint16_t x, uint16_t y ) {
   int8_t tile_id = 0;

   if( x >= TILEMAP_TW || y >= TILEMAP_TH ) {
      error_printf( "attempted to advance tile out of range!" );
      return;
   }

   tile_id = tilemap_get_tile_id( t, x, y );

   /* Find the next highest loaded tile, wrapping around to 0 if needed. */
   do {
      tile_id++;

      if( TILEMAP_TILESETS_MAX <= tile_id ) {
         tile_id = 0;
      }
   } while(
      TILESET_FLAG_ACTIVE !=
      (TILESET_FLAG_ACTIVE & t->tileset[tile_id].flags)
   );

   debug_printf( 0, "tile_id: %d", tile_id );

   /* Merge the tile ID back into the tilemap. */
   if( 0 == x % 2 ) {
      t->tiles[((y * TILEMAP_TW) + x) / 2] &= ~0xf0;
      tile_id <<= 4;
   } else {
      t->tiles[((y * TILEMAP_TW) + x) / 2] &= ~0x0f;
   }
   t->tiles[((y * TILEMAP_TW) + x) / 2] |= tile_id;
}

void tilemap_deinit( struct TILEMAP* t ) {
   if( NULL == t ) {
      return;
   }
}

