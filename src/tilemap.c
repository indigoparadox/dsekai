
#include "dstypes.h"

#ifdef USE_JSON_MAPS

#define JSON_TOKENS_MAX 2048
#define JSON_PATH_SZ 255
#define SPAWN_TYPE_MAX 32

int16_t tilemap_load( RESOURCE_ID id, struct TILEMAP* t ) {
   int16_t tok_parsed = 0,
      tiles_count = 0,
      i = 0,
      retval = 1,
      spawn_type_sz = 0,
      tileset_source_sz = 0;
   int8_t tile_id_in = 0;
   char* json_buffer = NULL;
   jsmn_parser parser;
   jsmntok_t* tokens = NULL;
   char iter_path[JSON_PATH_SZ];
   RESOURCE_JSON_HANDLE json_handle = NULL;
   MEMORY_HANDLE tokens_handle = NULL;
   uint32_t json_buffer_sz = 0;
   char tileset_name[DRC_FILENAME_SZ];
   char spawn_type[SPAWN_TYPE_MAX];

   memory_zero_ptr( tileset_name, DRC_FILENAME_SZ );

   json_handle = resource_get_json_handle( id );
   if( NULL == json_handle ) {
      error_printf( "could not get tilemap resource handle" );
      retval = 0;
      goto cleanup;
   }

   tokens_handle = memory_alloc( JSON_TOKENS_MAX, sizeof( jsmntok_t ) );
   if( NULL == tokens_handle ) {
      error_printf( "could not allocate space for JSON tokens" );
      retval = 0;
      goto cleanup;
   }

   debug_printf( 2, "JSON token buffer allocated: %lu bytes",
      sizeof( jsmntok_t ) * JSON_TOKENS_MAX );

   json_buffer_sz = memory_sz( json_handle );
   json_buffer = resource_lock_handle( json_handle );
   tokens = memory_lock( tokens_handle );

   if( '{' != json_buffer[0] ) {
      error_printf( "invalid tilemap json res %d (must start with '{')", id );
      error_printf( "found: %s\n", json_buffer );
      goto cleanup;
   }

   jsmn_init( &parser );
   tok_parsed = jsmn_parse(
      &parser, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX );

   debug_printf( 2, "%d tokens parsed", tok_parsed );
   if( 0 >= tok_parsed ) {
      retval = 0;
      goto cleanup;
   }

   tileset_source_sz = json_str_from_path(
      "/tilesets/0/source", 18, /* Path Sz */
      tileset_name, DRC_FILENAME_SZ,
      &(tokens[0]), tok_parsed, json_buffer );
   if( 0 >= tileset_source_sz ) {
      error_printf( "tileset source not found" );
      goto cleanup;
   }
   debug_printf( 1, "tileset source is %s (%d)",
      tileset_name, tileset_source_sz );
   
   /* Load map properties. */
   tiles_count = (TILEMAP_TW * TILEMAP_TH);
   for( i = 0 ; tiles_count > i ; i++ ) {
      /* Load tile data into the grid. */
      dio_snprintf(
         iter_path, JSON_PATH_SZ, "/layers/[name=terrain]/data/%d", i );
      tile_id_in = 
         json_int_from_path(
            iter_path, JSON_PATH_SZ, &(tokens[0]), tok_parsed, json_buffer );
      if( 0 > tile_id_in ) {
         error_printf( "invalid tile ID received" );
         continue;
      }
      tile_id_in--;
      if( 0 == i % 2 ) {
         tile_id_in <<= 4;
         tile_id_in &= 0xf0;
      } else {
         tile_id_in &= 0x0f;
      }
      t->tiles[i / 2] |= tile_id_in;
   }

   i = 0;
   dio_snprintf(
      iter_path, JSON_PATH_SZ, "/layers/[name=mobiles]/objects/%d/name", i );
   spawn_type_sz = json_str_from_path(
      iter_path, JSON_PATH_SZ,
      spawn_type, SPAWN_TYPE_MAX, &(tokens[0]), tok_parsed, json_buffer );
   while( 0 <= spawn_type_sz ) {
      if( 0 == memory_strncmp_ptr( "player", spawn_type, 6 ) ) {
         t->spawns[i].type = MOBILE_TYPE_PLAYER;
         t->spawns[i].coords.x = (dio_snprintf(
            iter_path, JSON_PATH_SZ,
            "/layers/[name=mobiles]/objects/%d/x", i ) / TILE_W);
         t->spawns[i].coords.y = (dio_snprintf(
            iter_path, JSON_PATH_SZ,
            "/layers/[name=mobiles]/objects/%d/y", i ) / TILE_H);
         debug_printf( 2, "player spawn at %d, %d",
            t->spawns[i].coords.x, t->spawns[i].coords.y );
      }

      i++;
      dio_snprintf(
         iter_path, JSON_PATH_SZ, "/layers/[name=mobiles]/objects/%d/name", i );
      spawn_type_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         spawn_type, SPAWN_TYPE_MAX, &(tokens[0]), tok_parsed, json_buffer );
   }
cleanup:

   if( NULL != tokens ) {
      tokens = memory_unlock( tokens_handle );
   }

   if( NULL != tokens_handle ) {
      memory_free( tokens_handle );
   }

   if( NULL != json_buffer ) {
      json_buffer = resource_unlock_handle( json_handle );
   }

   if( NULL != json_handle ) {
      resource_free_handle( json_handle );
   }

   return retval;
}

#endif /* USE_JSON_MAPS */

void tilemap_refresh_tiles( struct TILEMAP* t ) {
   int x = 0, y = 0;

   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         t->tiles_flags[(y * TILEMAP_TW) + x] |= TILEMAP_TILE_FLAG_DIRTY;
      }
   }
}

void tilemap_draw( struct TILEMAP* t, struct DSEKAI_STATE* state ) {
   int x = 0,
      y = 0;
   uint8_t tile_id = 0;
   uint16_t viewport_tx1 = 0,
      viewport_ty1 = 0,
      viewport_tx2 = 0,
      viewport_ty2 = 0;

   viewport_tx1 = state->screen_scroll_x / TILE_W;
   viewport_ty1 = state->screen_scroll_y / TILE_H;
   viewport_tx2 = TILEMAP_TW > viewport_tx1 + SCREEN_TW ?
      viewport_tx1 + SCREEN_TW : viewport_tx1 + (TILEMAP_TW - viewport_tx1);
   viewport_ty2 = TILEMAP_TH > viewport_ty1 + SCREEN_TH ?
      viewport_ty1 + SCREEN_TH : viewport_tx1 + (TILEMAP_TH - viewport_ty1);

   assert( viewport_tx2 <= TILEMAP_TW );
   assert( viewport_ty2 <= TILEMAP_TH );

   for( y = viewport_ty1 ; viewport_ty2 > y ; y++ ) {
      for( x = viewport_tx1 ; viewport_tx2 > x ; x++ ) {
#ifndef IGNORE_DIRTY
         if(
            !(t->tiles_flags[(y * TILEMAP_TW) + x] & TILEMAP_TILE_FLAG_DIRTY)
         ) {
            continue;
         }
#endif /* !IGNORE_DIRTY */

         assert( y < TILEMAP_TH );
         assert( x < TILEMAP_TW );
         assert( y >= 0 );
         assert( x >= 0 );

         t->tiles_flags[(y * TILEMAP_TW) + x] &= ~TILEMAP_TILE_FLAG_DIRTY;

         /* Grab the left byte if even or the right if odd. */
         tile_id = tilemap_get_tile_id( t, x, y );

         /* Blit the tile. */
         graphics_blit_at(
            t->tileset[tile_id].image,
            (x * TILE_W) - state->screen_scroll_x,
            (y * TILE_H) - state->screen_scroll_y, TILE_W, TILE_H );
      }
   }
}

uint8_t tilemap_collide( const struct TILEMAP* t, uint8_t x, uint8_t y ) {
   uint8_t tile_id = 0;

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

