
#define TILEMAP_C
#include "dsekai.h"

#ifndef RESOURCE_FILE

extern RES_CONST char gc_map_names[][TILEMAP_NAME_MAX];
extern RES_CONST struct TILEMAP* gc_map_structs[];
extern RES_CONST uint8_t gc_map_count;

#endif /* !RESOURCE_FILE */

int16_t tilemap_load( const char* map_name, struct TILEMAP* t ) {
   int16_t map_retval = 0,
      i = 0;
#ifdef RESOURCE_FILE
   char map_load_path[RESOURCE_PATH_MAX];
#else
   int8_t map_name_sz = 0;
#endif /* RESOURCE_FILE */

#ifdef RESOURCE_FILE
#  ifdef TILEMAP_FMT_JSON
   memory_zero_ptr( (MEMORY_PTR)map_load_path, TILEMAP_NAME_MAX );
   dio_snprintf(
      map_load_path,
      RESOURCE_PATH_MAX,
      ASSETS_PATH "m_%s.json", map_name );
   map_retval = tilemap_json_load( map_load_path, t );
#  elif defined TILEMAP_FMT_ASN
   dio_snprintf(
      map_load_path,
      RESOURCE_PATH_MAX,
      ASSETS_PATH "m_%s.asn", map_name );
   map_retval = tilemap_asn_load( map_load_path, t );
#  else
   /* Can't load a map for the engine; we're probably in map2asn. */
   map_retval = 0;
#  endif

#else
   map_name_sz = memory_strnlen_ptr( map_name, TILEMAP_NAME_MAX );
   debug_printf( 2, "attempting to open tilemap: %s (%d chars)",
      map_name, map_name_sz );
   debug_printf( 1, "looping through %d tilemaps...", gc_map_count );
   for( i = 0 ; gc_map_count > i ; i++ ) {
      if( 0 == memory_strncmp_ptr( gc_map_names[i], map_name, map_name_sz ) ) {
         debug_printf( 1, "gc_map_%s vs %s", gc_map_names[i], map_name );
         memory_copy_ptr( (MEMORY_PTR)t, (MEMORY_PTR)gc_map_structs[i],
            sizeof( struct TILEMAP ) );
         map_retval = 1;
         break;
      }
   }

#endif /* RESOURCE_FILE */

   if( 0 >= map_retval ) {
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
      t->tileset[i].image_id = graphics_cache_load_bitmap(
         t->tileset[i].image );
   }

cleanup:

   return map_retval;
}

void tilemap_refresh_tiles( struct TILEMAP* t ) {
   int8_t x = 0, y = 0;

   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         t->tiles_flags[(y * TILEMAP_TW) + x] |= TILEMAP_TILE_FLAG_DIRTY;
      }
   }
}

void tilemap_set_weather( struct TILEMAP* t, uint8_t weather ) {

   if(
      TILEMAP_FLAG_WEATHER_SNOW ==
      (TILEMAP_FLAG_WEATHER_MASK & weather)
   ) {
      debug_printf( 2, "current weather is snowy" );
#ifndef DISABLE_WEATHER_EFFECTS
      animate_create(
         ANIMATE_TYPE_SNOW, ANIMATE_FLAG_WEATHER | ANIMATE_FLAG_FG,
         SCREEN_MAP_X, SCREEN_MAP_Y, SCREEN_MAP_W, SCREEN_MAP_H );
#endif /* !DISABLE_WEATHER_EFFECTS */
   } else {
      debug_printf( 2, "current weather is clear" );
   }

   t->flags |= (TILEMAP_FLAG_WEATHER_MASK & weather);
}

uint8_t tilemap_collide(
   struct MOBILE* m, uint8_t dir, struct TILEMAP* t
) {
   uint8_t tile_id = 0;
   int16_t x = 0, y = 0;
   uint8_t collide_out = 0;

   assert( dir < 4 );

   x = m->coords.x + gc_mobile_x_offsets[dir];
   y = m->coords.y + gc_mobile_y_offsets[dir];

   tile_id = tilemap_get_tile_id( t, x, y );
   if( t->tileset[tile_id].flags & (uint8_t)TILESET_FLAG_BLOCK ) {
      collide_out = 1;
   }
   return collide_out;
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

