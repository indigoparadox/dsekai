
#include "dsekai.h"

#ifdef RESOURCE_FILE

int16_t tilemap_parse_spawn(
   struct TILEMAP* t, int16_t spawn_idx,
   struct jsmntok* tokens, uint16_t tokens_sz, char* json_buffer,
   char* iter_path, uint16_t iter_path_sz
) {
   struct TILEMAP_SPAWN* spawn = (struct TILEMAP_SPAWN*)&(t->spawns[spawn_idx]);
   char spawn_buffer[TILEMAP_SPAWN_T_MAX];
   int16_t spawn_buffer_sz = 0;

   dio_snprintf( iter_path, iter_path_sz, TILEMAP_JPATH_MOB_NAME, spawn_idx );
   spawn_buffer_sz = json_str_from_path(
      iter_path, JSON_PATH_SZ,
      spawn_buffer, TILEMAP_SPAWN_T_MAX, tokens, tokens_sz, json_buffer );

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
      iter_path, iter_path_sz, TILEMAP_JPATH_MOB_X, spawn_idx );
   spawn->coords.x = json_int_from_path(
      iter_path, iter_path_sz, &(tokens[0]), tokens_sz, json_buffer );
   spawn->coords.x /= TILE_W;

   /* Parse Y */
   dio_snprintf(
      iter_path, iter_path_sz, TILEMAP_JPATH_MOB_Y, spawn_idx );
   spawn->coords.y = json_int_from_path(
      iter_path, iter_path_sz, &(tokens[0]), tokens_sz, json_buffer );
   spawn->coords.y /= TILE_H;

   /* Parse Script */
   spawn->script_id = -1;
   dio_snprintf(
      iter_path, iter_path_sz, TILEMAP_JPATH_MOB_SCRIPT, spawn_idx );
   spawn->script_id = json_int_from_path(
      iter_path, iter_path_sz, &(tokens[0]), tokens_sz, json_buffer );

   if( spawn->script_id  >= TILEMAP_SCRIPTS_MAX ) {
      error_printf( "mobile uses script ID beyond maximum available!" );

   } else if( spawn->script_id >= t->scripts_count ) {
      /* Attempt to load the script, since it was referenced. */
      dio_snprintf(
         iter_path, iter_path_sz, TILEMAP_JPATH_SCRIPT, spawn->script_id );
      spawn_buffer[0] = '\0';
      spawn_buffer_sz = json_str_from_path(
         iter_path, iter_path_sz,
         spawn_buffer, TILEMAP_SPAWN_T_MAX,
         &(tokens[0]), tokens_sz, json_buffer );
      script_parse_str( spawn_buffer, spawn_buffer_sz,
         &(t->scripts[spawn->script_id]) );
      t->scripts_count++;
   }

   debug_printf( 2, "%d spawn at %d, %d (script %d)",
      spawn->type, spawn->coords.x, spawn->coords.y, spawn->script_id );

   return 1;
}

int16_t tilemap_parse_tileset(
   struct TILEMAP* t, char* ts_name, uint16_t ts_name_sz,
   struct jsmntok* tokens, uint16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   RESOURCE_ID map_path
) {
   int16_t i = 0,
      str_sz = 0,
      tile_filename_sz = 0;
   char tile_filename[RESOURCE_PATH_MAX],
      tile_json_path[JSON_PATH_SZ];

   do {
      /* Prepend asset path. */
      tile_filename_sz = tilemap_fix_asset_path(
         tile_filename, RESOURCE_PATH_MAX, map_path );

      /* Load each tile bitmap. */
      dio_snprintf( tile_json_path, JSON_PATH_SZ, TILEMAP_JPATH_TS_TILE, i );
      str_sz = json_str_from_path(
         tile_json_path, JSON_PATH_SZ,
         &(tile_filename[tile_filename_sz]),
         RESOURCE_PATH_MAX - tile_filename_sz,
         tokens, tokens_sz, json_buffer );
      tile_filename_sz += str_sz;

      if( 0 < str_sz ) {
         debug_printf(
            2, "assigning tile path: %s, %s\n", tile_json_path, tile_filename );
         resource_assign_id( t->tileset[i].image, tile_filename );
         t->tileset[i].flags = 0x00;
      }

      i++;
   } while( 0 < str_sz );

   return 1;
}

int8_t tilemap_json_tile(
   char* tile_path, int16_t tile_idx,
   struct jsmntok* tokens, uint16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz
) {
   int8_t tile_id_in = 0;
   char iter_path[JSON_PATH_SZ];

   /* Load tile data into the grid. */
   dio_snprintf( iter_path, JSON_PATH_SZ, tile_path, tile_idx );
   tile_id_in = 
      json_int_from_path(
         iter_path, JSON_PATH_SZ, tokens, tokens_sz, json_buffer );
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

   return tile_id_in;
}

int16_t tilemap_parse(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz
) {
   char iter_path[JSON_PATH_SZ];
   int16_t string_sz_tmp = 0;
   
   /* TODO: Split this up. */

   /* Load strings.*/
   debug_printf( 2, "loading strings" ); 
   for(
      t->strings_count = 0 ;
      TILEMAP_STRINGS_MAX > t->strings_count ;
      t->strings_count++
   ) {
      dio_snprintf(
         iter_path, JSON_PATH_SZ, TILEMAP_JPATH_STRING, t->strings_count );
      string_sz_tmp = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         t->strings[t->strings_count], TILEMAP_STRINGS_SZ,
         tokens, tokens_sz, json_buffer );
      if( 0 >= string_sz_tmp ) {
         /* Last string index was not parsed, so we're done. */
         break;
      } else {
         debug_printf( 1, "loaded string: %s (%d)",
            t->strings[t->strings_count], string_sz_tmp );
         t->string_szs[t->strings_count] = string_sz_tmp;
      }
   }

   debug_printf( 2, "loading spawns" ); 
   while( tilemap_parse_spawn(
      t, t->spawns_count, tokens, tokens_sz, json_buffer,
      iter_path, JSON_PATH_SZ
   ) ) {
      
      /* Iterate to the next spawn. */
      t->spawns_count++;
   }

   return 1;
}

int16_t tilemap_json_tilegrid(
   struct TILEMAP* t, char* tilegrid_path,
   char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz
) {
   int16_t tiles_count = 0,
      i = 0;
   
   /* Load map properties. */
   tiles_count = (TILEMAP_TW * TILEMAP_TH);
   for( i = 0 ; tiles_count > i ; i++ ) {
      t->tiles[i / 2] |= tilemap_json_tile(
         tilegrid_path, i,
         tokens, tokens_sz, json_buffer, json_buffer_sz );
   }

   return i;
}

/* Get the real path to the tileset (it's JSON so assume file paths). */
/* Resource IDs would be using pre-parsed maps. */

uint16_t tilemap_fix_asset_path(
   char* path_in, uint16_t path_in_sz, const char* map_path
) {
   uint16_t path_sz_out = 0,
      map_path_sz = 0;

   map_path_sz = strlen( map_path );
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

static int16_t tilemap_load_file(
   char* filename, char** json_buffer_p, RESOURCE_JSON_HANDLE* json_handle_p,
   uint32_t* json_buffer_sz_p, uint16_t* tok_parsed_p, struct jsmntok* tokens
) {
   if( NULL != *json_buffer_p ) {
      debug_printf( 1, "closing tilemap resource" );
      *json_buffer_p = resource_unlock_handle( *json_handle_p );
      resource_free_handle( *json_handle_p );
   }
   debug_printf( 1, "opening tilemap resource: %s", filename );
   *json_handle_p = resource_get_json_handle( filename );
   if( NULL == *json_handle_p ) {
      error_printf( "could not get handle for %s", filename );
      return 0;
   }
   debug_printf( 1, "setting up tokens and parsing..." );
   *json_buffer_sz_p = memory_sz( *json_handle_p );
   *json_buffer_p = resource_lock_handle( *json_handle_p );
   memory_zero_ptr( tokens, JSON_TOKENS_MAX * sizeof( struct jsmntok ) );
   *tok_parsed_p = json_load(
      *json_buffer_p, *json_buffer_sz_p, tokens, JSON_TOKENS_MAX );

   debug_printf( 1, "parsed %d tokens", *tok_parsed_p );

   return 1;
}

int16_t tilemap_load( RESOURCE_ID id, struct TILEMAP* t ) {
   char* json_buffer = NULL;
   int16_t retval = 1;
   RESOURCE_JSON_HANDLE json_handle = NULL;
   MEMORY_HANDLE tokens_handle = NULL;
   uint32_t json_buffer_sz = 0;
   uint16_t ts_name_sz = 0,
      tok_parsed = 0;
   char ts_name[JSON_PATH_SZ];
   struct jsmntok* tokens = NULL;

   memory_zero_ptr( t, sizeof( struct TILEMAP ) );

   /* Allocate buffers for parsing JSON. */
   json_handle = resource_get_json_handle( id );
   if( NULL == json_handle ) {
      error_printf( "could not get tilemap resource handle" );
      retval = 0;
      goto cleanup;
   }

   tokens_handle = memory_alloc( JSON_TOKENS_MAX, sizeof( struct jsmntok ) );
   if( NULL == tokens_handle ) {
      error_printf( "could not allocate space for JSON tokens" );
      retval = 0;
      goto cleanup;
   }

   tokens = memory_lock( tokens_handle );

   debug_printf( 1, "JSON token buffer allocated: %lu bytes",
      sizeof( struct jsmntok ) * JSON_TOKENS_MAX );

   /* Load the tilemap JSON and parse it. */

   retval = tilemap_load_file(
      id, &json_buffer, &json_handle, &json_buffer_sz, &tok_parsed, tokens );
   if( 0 == retval ) {
      error_printf( "unable to open tilemap" );
      goto cleanup;
   }

   /* Parse properties, first pass. */

   debug_printf( 1, "getting tilemap name and tileset path" );

   json_str_from_path(
      TILEMAP_JPATH_PROP_NAME, sizeof( TILEMAP_JPATH_PROP_NAME ),
      t->name, TILEMAP_NAME_MAX, tokens, tok_parsed, json_buffer );

   retval = tilemap_parse(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX );

   ts_name_sz = tilemap_fix_asset_path(
      ts_name, RESOURCE_PATH_MAX, id );
   ts_name_sz += json_str_from_path(
      TILEMAP_JPATH_TS_SRC, sizeof( TILEMAP_JPATH_TS_SRC ),
      &(ts_name[ts_name_sz]), JSON_PATH_SZ, tokens, tok_parsed, json_buffer );
   if( 0 == retval || 0 == ts_name_sz ) {
      error_printf( "could not find tileset name" );
      retval = 0;
      goto cleanup;
   }

   debug_printf( 1, "tileset path is: %s", ts_name );

   /* Unload the map, load the tileset. */

   retval = tilemap_load_file(
      ts_name, &json_buffer, &json_handle, &json_buffer_sz, &tok_parsed,
      tokens );
   if( 0 == retval ) {
      goto cleanup;
   }

   /* Parse the map tileset. */

   tilemap_parse_tileset( t, ts_name, ts_name_sz,
      tokens, tok_parsed, json_buffer, json_buffer_sz, id );

   /* Unload the tileset, load the map and finish parsing. */

   retval = tilemap_load_file(
      id, &json_buffer, &json_handle, &json_buffer_sz, &tok_parsed, tokens );
   if( 0 == retval ) {
      goto cleanup;
   }

   retval = tilemap_json_tilegrid(
      t, TILEMAP_JPATH_TILE, json_buffer, json_buffer_sz,
      tokens, tok_parsed );

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

#endif /* RESOURCE_FILE */

void tilemap_refresh_tiles( struct TILEMAP* t ) {
   int x = 0, y = 0;

   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         t->tiles_flags[(y * TILEMAP_TW) + x] |= TILEMAP_TILE_FLAG_DIRTY;
      }
   }
}

#if defined( SCREEN_W ) && defined( SCREEN_H )

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
            0, 0,
            (x * TILE_W) - state->screen_scroll_x,
            (y * TILE_H) - state->screen_scroll_y, TILE_W, TILE_H );
      }
   }
}

#endif /* SCREEN_W && SCREEN_H */

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

