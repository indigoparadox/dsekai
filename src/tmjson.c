
#include "tmjson.h"

static int16_t tilemap_json_parse_spawn(
   struct TILEMAP* t, int16_t spawn_idx,
   struct jsmntok* tokens, uint16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   RESOURCE_ID map_path
) {
   struct TILEMAP_SPAWN* spawn = (struct TILEMAP_SPAWN*)&(t->spawns[spawn_idx]);
   char spawn_buffer[RESOURCE_PATH_MAX];
   char iter_path[JSON_PATH_SZ];
   int16_t spawn_buffer_sz = 0,
      str_sz = 0,
      x_px_in = 0,
      y_px_in = 0;

   /* Prepend asset path. */

   spawn_buffer_sz = tilemap_fix_asset_path(
      spawn_buffer, RESOURCE_PATH_MAX, map_path );

   /* Parse Sprite */

   dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_TYPE, spawn_idx );
   str_sz = json_str_from_path(
      iter_path, JSON_PATH_SZ,
      &(spawn_buffer[spawn_buffer_sz]),
      RESOURCE_PATH_MAX - spawn_buffer_sz,
      tokens, tokens_sz, json_buffer );
   spawn_buffer_sz += str_sz;

   if( 0 >= str_sz ) {
      error_printf( "could not parse mobile: %d", spawn_idx );
      return 0;
   }

   resource_assign_id( spawn->type, spawn_buffer );

   /* Parse Name */
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_NAME, spawn_idx );
   json_str_from_path(
      iter_path, JSON_PATH_SZ,
      spawn->name, TILEMAP_SPAWN_NAME_SZ, tokens, tokens_sz, json_buffer );

   /* Parse X */
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_X, spawn_idx );
   x_px_in = json_int_from_path(
      iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );
   spawn->coords.x = x_px_in / TILE_W;

   /* Parse Y */
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_Y, spawn_idx );
   y_px_in = json_int_from_path(
      iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );
   spawn->coords.y = y_px_in / TILE_H;

   /* Parse Script */
   spawn->script_id = -1;
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_SCRIPT, spawn_idx );
   spawn->script_id = json_int_from_path(
      iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

   if( spawn->script_id  >= TILEMAP_SCRIPTS_MAX ) {
      error_printf( "mobile uses script ID beyond maximum available!" );

   } else if( spawn->script_id >= t->scripts_count ) {
      /* Attempt to load the script, since it was referenced. */
      dio_snprintf(
         iter_path, JSON_PATH_SZ, TILEMAP_JPATH_SCRIPT, spawn->script_id );
      spawn_buffer[0] = '\0';
      spawn_buffer_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         spawn_buffer, RESOURCE_PATH_MAX,
         &(tokens[0]), tokens_sz, json_buffer );
      script_parse_str( spawn_buffer, spawn_buffer_sz,
         &(t->scripts[spawn->script_id]) );
      t->scripts_count++;
   }

   debug_printf( 2, "%s spawn at %d, %d (script %d)",
      spawn->type, spawn->coords.x, spawn->coords.y, spawn->script_id );

   return 1;
}

static int16_t tilemap_json_parse_tileset(
   struct TILEMAP* t,
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

         /* Load tile flags. */
         dio_snprintf(
            tile_json_path, JSON_PATH_SZ, TILEMAP_JPATH_TS_FLAGS, i );
         t->tileset[i].flags = json_int_from_path(
            tile_json_path, JSON_PATH_SZ, tokens, tokens_sz, json_buffer );
         debug_printf(
            2, "tile flags: %s, %d\n", tile_json_path, t->tileset[i].flags );
            resource_assign_id( t->tileset[i].image, tile_filename );
      }

      i++;
   } while( 0 < str_sz );

   return 1;
}

static int8_t tilemap_json_tile(
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

static int16_t tilemap_json_parse_strings(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz
) {
   char iter_path[JSON_PATH_SZ];
   int16_t string_sz_tmp = 0;
   
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

   return 1;
}

static int16_t tilemap_json_tilegrid(
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

static int16_t tilemap_json_load_file(
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
   if( (RESOURCE_JSON_HANDLE)0 == *json_handle_p ) {
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

int16_t tilemap_json_load( RESOURCE_ID id, struct TILEMAP* t ) {
   char* json_buffer = NULL;
   int16_t retval = 1;
   RESOURCE_JSON_HANDLE json_handle = (RESOURCE_JSON_HANDLE)0;
   MEMORY_HANDLE tokens_handle = (MEMORY_HANDLE)0;
   uint32_t json_buffer_sz = 0;
   uint16_t ts_name_sz = 0,
      tok_parsed = 0;
   char ts_name[JSON_PATH_SZ];
   struct jsmntok* tokens = NULL;

   memory_zero_ptr( t, sizeof( struct TILEMAP ) );

   /* Allocate buffers for parsing JSON. */
   json_handle = resource_get_json_handle( id );
   if( (RESOURCE_JSON_HANDLE)0 == json_handle ) {
      error_printf( "could not get tilemap resource handle" );
      retval = 0;
      goto cleanup;
   }

   tokens_handle = memory_alloc( JSON_TOKENS_MAX, sizeof( struct jsmntok ) );
   if( (MEMORY_HANDLE)0 == tokens_handle ) {
      error_printf( "could not allocate space for JSON tokens" );
      retval = 0;
      goto cleanup;
   }

   tokens = memory_lock( tokens_handle );

   debug_printf( 1, "JSON token buffer allocated: %lu bytes",
      sizeof( struct jsmntok ) * JSON_TOKENS_MAX );

   /* Load the tilemap JSON and parse it. */

   retval = tilemap_json_load_file(
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

   retval = tilemap_json_load_file(
      ts_name, &json_buffer, &json_handle, &json_buffer_sz, &tok_parsed,
      tokens );
   if( 0 == retval ) {
      goto cleanup;
   }

   /* Parse the map tileset. */

   tilemap_json_parse_tileset( t,
      tokens, tok_parsed, json_buffer, json_buffer_sz, id );

   /* Unload the tileset, load the map and finish parsing. */

   retval = tilemap_json_load_file(
      id, &json_buffer, &json_handle, &json_buffer_sz, &tok_parsed, tokens );
   if( 0 == retval ) {
      goto cleanup;
   }

   debug_printf( 2, "loading tilegrid" ); 
   retval = tilemap_json_tilegrid(
      t, TILEMAP_JPATH_TILE, json_buffer, json_buffer_sz,
      tokens, tok_parsed );

   debug_printf( 2, "loading spawns" ); 
   while(
      TILEMAP_SPAWNS_MAX > t->spawns_count &&
      tilemap_json_parse_spawn(
         t, t->spawns_count, tokens, tok_parsed, json_buffer, json_buffer_sz, id
      )
   ) {
      
      /* Iterate to the next spawn. */
      t->spawns_count++;
   }

   retval = tilemap_json_parse_strings(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX );

cleanup:

   if( NULL != tokens ) {
      tokens = memory_unlock( tokens_handle );
   }

   if( (MEMORY_HANDLE)0 != tokens_handle ) {
      memory_free( tokens_handle );
   }

   if( NULL != json_buffer ) {
      json_buffer = resource_unlock_handle( json_handle );
   }

   if( (RESOURCE_JSON_HANDLE)0 != json_handle ) {
      resource_free_handle( json_handle );
   }

   return retval;
}

