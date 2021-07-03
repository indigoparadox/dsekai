
#include "tilemap.h"

#include "data/json.h"
#include "data/dio.h"
#include "data/drc.h"
#include "memory.h"

#include <string.h>

#ifdef TILEMAP_JSON

#define JSON_TOKENS_MAX 1024
#define JSON_PATH_SZ 255
#define JSON_BUFFER_SZ 4096

int16_t tilemap_load( uint32_t id, struct TILEMAP* t ) {
   int16_t tok_parsed = 0,
      tiles_count = 0,
      i = 0,
      retval = 0;
   uint8_t tile_id_in = 0;
   jsmn_parser parser;
   jsmntok_t* tokens = NULL;
   char iter_path[JSON_PATH_SZ];
   struct DIO_RESOURCE rsrc;

   memset( &rsrc, '\0', sizeof( struct DIO_RESOURCE ) );
   retval = dio_get_resource_handle( id, 'json', &rsrc );
   if( !retval ) {
      goto cleanup;
   }

   tokens = memory_alloc( JSON_TOKENS_MAX, sizeof( jsmntok_t ) );

   jsmn_init( &parser );
   tok_parsed = jsmn_parse(
      &parser, rsrc.ptr, rsrc.ptr_sz, tokens, JSON_TOKENS_MAX );

   debug_printf( 2, "%d tokens parsed", tok_parsed );
   if( 0 == tok_parsed ) {
      retval = 0;
      goto cleanup;
   }

   /* Load map properties. */
   tiles_count = (TILEMAP_TW * TILEMAP_TH);
   for( i = 0 ; tiles_count > i ; i++ ) {
      /* Load tile data into the grid. */
      dio_snprintf( iter_path, 255, "/layers/0/data/%d", i );
      tile_id_in = 
         json_int_from_path( iter_path, &(tokens[0]), tok_parsed, rsrc.ptr );
      tile_id_in--;
      if( 0 == i % 2 ) {
         tile_id_in <<= 4;
         tile_id_in &= 0xf0;
      } else {
         tile_id_in &= 0x0f;
      }
      t->tiles[i / 2] |= tile_id_in;
   }

cleanup:

   dio_free_resource_handle( &rsrc );

   memory_free( &tokens );

   return retval;
}

#endif /* TILEMAP_JSON */

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

