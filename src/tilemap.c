
#include "dsekai.h"

#ifdef USE_JSON_MAPS

#define SPAWN_TYPE_MAX 32

int16_t tilemap_parse_spawn(
   struct TILEMAP* t, int16_t spawn_idx,
   jsmntok_t* tokens, uint16_t tokens_sz, char* json_buffer,
   char* iter_path, uint16_t iter_path_sz
) {
   struct TILEMAP_SPAWN* spawn = (struct TILEMAP_SPAWN*)&(t->spawns[spawn_idx]);
   char spawn_buffer[SPAWN_TYPE_MAX];
   int16_t spawn_buffer_sz = 0;

   dio_snprintf(
      iter_path, iter_path_sz, "/layers/[name=mobiles]/objects/%d/name",
         spawn_idx );
   spawn_buffer_sz = json_str_from_path(
      iter_path, JSON_PATH_SZ,
      spawn_buffer, SPAWN_TYPE_MAX, tokens, tokens_sz, json_buffer );

   if( 0 > spawn_buffer_sz ) {
      error_printf( "unable to parse mobile name" );
      return 0;
   }

   /* Parse Type */
   if( 0 == memory_strncmp_ptr( "player", spawn_buffer, 6 ) ) {
      spawn->type = MOBILE_TYPE_PLAYER;
   } else if( 0 == memory_strncmp_ptr( "princess", spawn_buffer, 8 ) ) {
      spawn->type = MOBILE_TYPE_PRINCESS;
   }

   /* Parse X */
   dio_snprintf(
      iter_path, iter_path_sz,
      "/layers/[name=mobiles]/objects/%d/x", spawn_idx );
   spawn->coords.x = json_int_from_path(
      iter_path, iter_path_sz, &(tokens[0]), tokens_sz, json_buffer );
   spawn->coords.x /= TILE_W;

   /* Parse Y */
   dio_snprintf(
      iter_path, iter_path_sz,
      "/layers/[name=mobiles]/objects/%d/y", spawn_idx );
   spawn->coords.y = json_int_from_path(
      iter_path, iter_path_sz, &(tokens[0]), tokens_sz, json_buffer );
   spawn->coords.y /= TILE_H;

#if 0
   /* Parse Interaction */
   dio_snprintf(
      iter_path, iter_path_sz,
      "/layers/[name=mobiles]/objects/%d/properties/[name=interaction]/value",
      spawn_idx );
   spawn_buffer_sz = json_str_from_path(
      iter_path, iter_path_sz,
      spawn_buffer, SPAWN_TYPE_MAX, &(tokens[0]), tokens_sz, json_buffer );
   if( 0 == memory_strncmp_ptr( "talk", spawn_buffer, 4 ) ) {
      spawn->interaction = MOBILE_IACT_TALK;
      debug_printf( 1, "mobile interaction: talk" );
   }

   /* Parse Interaction Data */
   switch( spawns->interaction ) {
   case MOBILE_IACT_TALK:
      dio_snprintf(
         iter_path, iter_path_sz,
         "/layers/[name=mobiles]/objects/%d/properties/[name=data]/value",
         spawn_idx );
      spawn_buffer_sz = json_str_from_path(
         iter_path, iter_path_sz,
         spawn_buffer, SPAWN_TYPE_MAX, &(tokens[0]), tokens_sz, json_buffer );
      if( TILEMAP_STRINGS_MAX <= t->strings_count + 1 ) {
         error_printf( "tilemap string table full" );
         break;
      }
      memory_strncpy_ptr(
         &(t->strings[t->strings_count][0]),
         spawn_buffer,
         spawn_buffer_sz );
      debug_printf( 2, "added string \"%s\" at index %d",
         spawn_buffer, t->strings_count );
      t->strings_count++;

      break;
   }
#endif

   debug_printf( 2, "%d spawn at %d, %d",
      spawn->type, spawn->coords.x, spawn->coords.y );

   return 1;
}

int16_t tilemap_parse_tileset(
   struct TILEMAP* t,
   jsmntok_t* tokens, uint16_t tokens_sz, char* json_buffer,
   char* iter_path, uint16_t iter_path_sz
) {
   char tileset_name[DRC_FILENAME_SZ];
   int16_t tileset_source_len = 0;

   memory_zero_ptr( tileset_name, DRC_FILENAME_SZ );

   tileset_source_len = json_str_from_path(
      "/tilesets/0/source", 18, /* Path Sz */
      tileset_name, DRC_FILENAME_SZ, tokens, tokens_sz, json_buffer );
   if( 0 >= tileset_source_len ) {
      error_printf( "tileset source not found" );
      return 0;
   }
   debug_printf( 1, "tileset source is %s (%d)",
      tileset_name, tileset_source_len ); 

   return 1;
}

int8_t tilemap_parse_tile(
   struct TILEMAP* t, int16_t tile_idx,
   jsmntok_t* tokens, uint16_t tokens_sz, char* json_buffer,
   char* iter_path, uint16_t iter_path_sz
) {
   int8_t tile_id_in = 0;

   /* Load tile data into the grid. */
   dio_snprintf(
      iter_path, iter_path_sz, "/layers/[name=terrain]/data/%d", tile_idx );
   tile_id_in = 
      json_int_from_path(
         iter_path, iter_path_sz, tokens, tokens_sz, json_buffer );
   if( 0 > tile_id_in ) {
      error_printf( "invalid tile ID received" );
      return tile_id_in;
   }
   tile_id_in--;
   if( 0 == tile_idx % 2 ) {
      tile_id_in <<= 4;
      tile_id_in &= 0xf0;
   } else {
      tile_id_in &= 0x0f;
   }
   t->tiles[tile_idx / 2] |= tile_id_in;

   return tile_id_in;
}

int16_t tilemap_parse(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   jsmntok_t* tokens, uint16_t tokens_sz
) {
   int16_t tok_parsed = 0,
      tiles_count = 0,
      name_len = 0,
      i = 0,
      j = 0;
   int8_t tile_id_in = 0;
   jsmn_parser parser;
   char iter_path[JSON_PATH_SZ];

   if( '{' != json_buffer[0] ) {
      error_printf( "invalid tilemap json (must start with '{') found: %s",
         json_buffer );
      return 0;
   }

   jsmn_init( &parser );
   tok_parsed = jsmn_parse(
      &parser, json_buffer, json_buffer_sz, tokens, tokens_sz );

   debug_printf( 2, "%d tokens parsed", tok_parsed );
   if( 0 >= tok_parsed ) {
      return 0;
   }

   /* TODO: tilemap_parse_tileset */
  
   /* Load map properties. */
   tiles_count = (TILEMAP_TW * TILEMAP_TH);
   for( i = 0 ; tiles_count > i ; i++ ) {
      tilemap_parse_tile(
         t, i, tokens, tok_parsed, json_buffer,
         iter_path, JSON_PATH_SZ );
   }

   /* Parse Name */
   name_len = json_str_from_path(
      "/properties/[name=name]/value", 29, /* Path Sz */
      t->name, TILEMAP_NAME_MAX, tokens, tok_parsed, json_buffer );
   if( 0 >= name_len ) {
      error_printf( "tilemap name not found" );
      return 0;
   }
   debug_printf( 2, "tilemap name is %s (%d)", t->name, name_len ); 

   debug_printf( 1, "loading spawns" ); 
   while( tilemap_parse_spawn(
      t, t->spawns_count, tokens, tok_parsed, json_buffer,
      iter_path, JSON_PATH_SZ
   ) ) {
      
      /* Iterate to the next spawn. */
      t->spawns_count++;
   }

   return 1;
}

int16_t tilemap_load( RESOURCE_ID id, struct TILEMAP* t ) {
   char* json_buffer = NULL;
   int16_t retval = 1;
   RESOURCE_JSON_HANDLE json_handle = NULL;
   MEMORY_HANDLE tokens_handle = NULL;
   uint32_t json_buffer_sz = 0;
   jsmntok_t* tokens = NULL;

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


   retval = tilemap_parse(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX );

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

