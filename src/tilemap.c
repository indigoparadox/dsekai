
#include "tilemap.h"

#include "../tools/data/json.h"
#include "data/dio.h"
#include "data/drc.h"

#include <string.h>

#define JSON_TOKENS_MAX 4096
#define JSON_PATH_SZ 255
#define JSON_BUFFER_SZ 40960

int16_t tilemap_load( uint32_t id, struct TILEMAP* t ) {
   int16_t tok_parsed = 0,
      tiles_count = 0,
      buffer_used = 0,
      i = 0,
      retval = 0;
   uint8_t tile_id_in = 0;
   jsmn_parser parser;
   jsmntok_t tokens[JSON_TOKENS_MAX];
   char iter_path[JSON_PATH_SZ];
   char json_buffer[JSON_BUFFER_SZ];
   struct DIO_STREAM drc_file;
   union DRC_TYPE map_type = DRC_MAP_TYPE;

   dio_open_stream_file( DRC_ARCHIVE, "r", &drc_file );
   if( 0 == dio_type_stream( &drc_file ) ) {
      error_printf( "unable to open archive for tilemap" );
      retval = -1;
      goto cleanup;
   }

   memset( json_buffer, '\0', JSON_BUFFER_SZ );

   buffer_used = drc_get_resource(
      &drc_file, map_type, id, &(json_buffer[0]),
      JSON_BUFFER_SZ );

   printf( "%s (%d)\n", json_buffer, buffer_used );

   jsmn_init( &parser );
   tok_parsed = jsmn_parse(
      &parser, &(json_buffer[0]), buffer_used, tokens, JSON_TOKENS_MAX );

   debug_printf( 2, "%d tokens parsed", tok_parsed );
   assert( 0 < tok_parsed );

   /* Load map properties. */
   tiles_count = (TILEMAP_TW * TILEMAP_TH);
   for( i = 0 ; tiles_count > i ; i++ ) {
      /* Load tile data into the grid. */
      dio_snprintf( iter_path, 255, "/layers/0/data/%d", i );
      tile_id_in = 
         json_int_from_path( iter_path, &(tokens[0]), tok_parsed, json_buffer );
      t->tiles[i / 2] =
         ((0 == i % 2) ? (tile_id_in << 4) : tile_id_in) & 0x0f;
   }

cleanup:

   if( 0 < dio_type_stream( &drc_file ) ) {
      dio_close_stream( &drc_file );
   }

   return retval;
}

void tilemap_draw(
   const struct TILEMAP* t, uint8_t* tiles_flags,
   uint16_t tiles_flags_w, uint16_t tiles_flags_h,
   uint16_t screen_x, uint16_t screen_y, uint8_t force
) {
   int x = 0,
      y = 0;
   uint8_t tile_id = 0;
   uint16_t viewport_tx1 = 0,
      viewport_ty1 = 0,
      viewport_tx2 = 0,
      viewport_ty2 = 0;

   viewport_tx1 = screen_x / TILE_W;
   viewport_ty1 = screen_y / TILE_H;
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
            !force &&
            !(tiles_flags[(y * tiles_flags_w) + x] & TILEMAP_TILE_FLAG_DIRTY)
         ) {
            continue;
         }
#endif /* !IGNORE_DIRTY */

         assert( y < TILEMAP_TH );
         assert( x < TILEMAP_TW );
         assert( y >= 0 );
         assert( x >= 0 );

         tiles_flags[(y * tiles_flags_w) + x] &= ~TILEMAP_TILE_FLAG_DIRTY;

         /* Grab the left byte if even or the right if odd. */
         tile_id = tilemap_get_tile_id( t, x, y );

         /* Blit the tile. */
         graphics_blit_at(
            &(t->tileset[tile_id]),
            (x * TILE_W) - screen_x, (y * TILE_H) - screen_y, TILE_W, TILE_H );
      }
   }
}

uint8_t tilemap_collide( const struct TILEMAP* t, uint8_t x, uint8_t y ) {
   uint8_t tile_id = 0;

   tile_id = tilemap_get_tile_id( t, x, y );
   if( t->tileset_flags[tile_id] & (uint8_t)TILEMAP_TILESET_FLAG_BLOCK ) {
      return 1;
   }
   return 0;
}

void tilemap_deinit( struct TILEMAP* t ) {
   int i = 0;

   if( NULL == t ) {
      return;
   }

   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      graphics_unload_bitmap( &(t->tileset[i]) );
   }
}

