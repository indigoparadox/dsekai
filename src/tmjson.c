
#include "tmjson.h"

#ifndef RESOURCE_FILE
#error Loading maps from JSON requires file resources!
#endif /* !RESOURCE_FILE */

static int16_t tilemap_json_parse_spawn(
   struct TILEMAP* t, int16_t spawn_idx,
   struct jsmntok* tokens, int16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   RESOURCE_ID map_path
) {
   struct TILEMAP_SPAWN* spawn = (struct TILEMAP_SPAWN*)&(t->spawns[spawn_idx]);
   char spawn_buffer[RESOURCE_PATH_MAX];
   char iter_path[JSON_PATH_SZ];
   int16_t spawn_buffer_sz = 0,
      x_px_in = 0,
      y_px_in = 0;

   /* Prepend asset path. */

#ifndef NO_FIX_ASSET_PATH
   spawn_buffer_sz = tilemap_fix_asset_path(
      spawn_buffer, RESOURCE_PATH_MAX, map_path );
#endif /* NO_FIX_ASSET_PATH */

   /* Parse Sprite */

   dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_TYPE, spawn_idx );
   spawn_buffer_sz = json_str_from_path(
      iter_path, JSON_PATH_SZ,
      &(spawn_buffer[spawn_buffer_sz]),
      RESOURCE_PATH_MAX - spawn_buffer_sz,
      tokens, tokens_sz, json_buffer );

   if( 0 >= spawn_buffer_sz ) {
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

   if( 0 > spawn->script_id || spawn->script_id  >= TILEMAP_SCRIPTS_MAX ) {
      error_printf( "spawn \"%s\" uses invalid script ID: %d",
         spawn->name, spawn->script_id );
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
      tile_filename_sz = 0;
   char tile_filename[RESOURCE_PATH_MAX],
      tile_json_path[JSON_PATH_SZ];

   do {
#ifndef NO_FIX_ASSET_PATH
      /* Prepend asset path. */
      tile_filename_sz = tilemap_fix_asset_path(
         tile_filename, RESOURCE_PATH_MAX, map_path );
#endif /* NO_FIX_ASSET_PATH */

      /* Load each tile bitmap. */
      dio_snprintf( tile_json_path, JSON_PATH_SZ, TILEMAP_JPATH_TS_TILE, i );
      tile_filename_sz = json_str_from_path(
         tile_json_path, JSON_PATH_SZ,
         &(tile_filename[tile_filename_sz]),
         RESOURCE_PATH_MAX - tile_filename_sz,
         tokens, tokens_sz, json_buffer );

      if( 0 < tile_filename_sz ) {
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
   } while( 0 < tile_filename_sz );

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

   /* Negative values become 0. */
   if( 0 > tile_id_in ) {
      tile_id_in = 0;
   }

   if( 0 == tile_idx % 2 ) {
      /* Tile is the left of the byte. */
      tile_id_in <<= 4;
      tile_id_in &= 0xf0;
   } else {
      /* Tile is the right of the byte. */
      tile_id_in &= 0x0f;
   }

   return tile_id_in;
}

static void tilemap_json_parse_scripts(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz
) {
   char iter_path[JSON_PATH_SZ];
   int i = 0,
      script_buffer_sz = 0,
      loaded = 0;
   char script_buffer[SCRIPT_STR_MAX];
   
   /* Load scripts.*/
   debug_printf( 2, "loading scripts" ); 
   for( i = 0 ; TILEMAP_SCRIPTS_MAX > i ; i++ ) {
      memory_zero_ptr( script_buffer, SCRIPT_STR_MAX );
      dio_snprintf(
         iter_path, JSON_PATH_SZ, TILEMAP_JPATH_SCRIPT, i );
      script_buffer[0] = '\0';
      script_buffer_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         script_buffer, SCRIPT_STR_MAX,
         &(tokens[0]), tokens_sz, json_buffer );
      if( 0 >= script_buffer_sz ) {
         error_printf( "invalid script returned (loaded %d)", loaded );
         break;
      }
      script_parse_str( i, script_buffer, script_buffer_sz, &(t->scripts[i]) );
      loaded++;
   }
}

static void tilemap_json_parse_strings(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz
) {
   char iter_path[JSON_PATH_SZ],
      string_buf[TILEMAP_JSON_STRBUF_SZ];
   int16_t string_sz_tmp = 0;
   int i = 0;

   strpool_init( t->strpool, TILEMAP_STRPOOL_SZ );
   
   /* Load strings.*/
   debug_printf( 2, "loading strings" ); 
   do {
      dio_snprintf(
         iter_path, JSON_PATH_SZ, TILEMAP_JPATH_STRING, i );
      string_sz_tmp = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         string_buf, TILEMAP_JSON_STRBUF_SZ,
         tokens, tokens_sz, json_buffer );
      if( 0 >= string_sz_tmp ) {
         /* Last string index was not parsed, so we're done. */
         break;
      } else {
         strpool_add_string( t->strpool, string_buf, string_sz_tmp );
         debug_printf( 1, "loaded string: %s (%d)",
            string_buf, string_sz_tmp );
         i++;
      }
   } while( 0 < string_sz_tmp );
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
   char* filename, char** json_buffer_p, RESOURCE_HANDLE* json_handle_p,
   uint32_t* json_buffer_sz_p, int16_t* tok_parsed_p, struct jsmntok* tokens
) {
   if( NULL != *json_buffer_p ) {
      debug_printf( 1, "closing tilemap resource" );
      *json_buffer_p = resource_unlock_handle( *json_handle_p );
      resource_free_handle( *json_handle_p );
   }
   debug_printf( 1, "opening tilemap resource: %s", filename );
   *json_handle_p = resource_get_handle( filename );
   if( (RESOURCE_HANDLE)0 == *json_handle_p ) {
      error_printf( "could not get handle for %s", filename );
      return 0;
   }
   debug_printf( 1, "setting up tokens and parsing..." );
   *json_buffer_sz_p = memory_sz( *json_handle_p );
   *json_buffer_p = resource_lock_handle( *json_handle_p );
   memory_zero_ptr( tokens, JSON_TOKENS_MAX * sizeof( struct jsmntok ) );
   *tok_parsed_p = json_load(
      *json_buffer_p, *json_buffer_sz_p, tokens, JSON_TOKENS_MAX );
   if( 0 > *tok_parsed_p ) {
      error_printf( "unable to parse any tokens" );
      return 0;
   }

   debug_printf( 1, "parsed %d tokens", *tok_parsed_p );

   return 1;
}

#if 0
void tilemap_json_parse_engine(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, int16_t tokens_sz
) {
   char engine_type[TILEMAP_NAME_MAX];
   int8_t i = 0;

   json_str_from_path(
      TILEMAP_JPATH_PROP_ENGINE, sizeof( TILEMAP_JPATH_PROP_ENGINE ),
      engine_type, TILEMAP_NAME_MAX, tokens, tokens_sz, json_buffer );

   t->engine_type = 0; /* ENGINE_TYPE_NONE */

   /* Loop through available engines and find matching token. */
   for( i = 0 ; '\0' != gc_engines_tokens[i][0] ; i++ ) {
      if( 0 == memory_strncmp_ptr(
         engine_type,
         gc_engines_tokens[i],
         TILEMAP_NAME_MAX
      ) ) {
         t->engine_type = i;
      }
   }

   /* XXX */
   /* t->engine_type = ENGINE_TYPE_POV; */
}
#endif

void tilemap_json_parse_weather(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, int16_t tokens_sz
) {
   char weather[TILEMAP_NAME_MAX];

   json_str_from_path(
      TILEMAP_JPATH_PROP_WEATHER, sizeof( TILEMAP_JPATH_PROP_WEATHER ),
      weather, TILEMAP_NAME_MAX, tokens, tokens_sz, json_buffer );

   if( 0 == memory_strncmp_ptr( weather, "snow", 4 ) ) {
      t->weather = TILEMAP_WEATHER_SNOW;
   } else {
      t->weather = TILEMAP_WEATHER_CLEAR;
   }
}

int16_t tilemap_json_parse_items(
   struct TILEMAP* t,
   char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, int16_t tokens_sz
) {
   char iter_path[JSON_PATH_SZ];
   int16_t i = 0,
      j = 0,
      sprite_buffer_sz = 0,
      type_buffer_sz = 0,
      name_buffer_sz = 0;
   char sprite_buffer[RESOURCE_PATH_MAX],
      type_buffer[ITEM_NAME_SZ + 1];
   
   /* Load items.*/
   debug_printf( 2, "loading items" ); 
   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      
      /* type */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_TYPE, i );
      type_buffer_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         type_buffer, ITEM_NAME_SZ,
         &(tokens[0]), tokens_sz, json_buffer );
      if( 0 >= type_buffer_sz ) {
         error_printf( "invalid item returned (loaded %d)", i );
         break;
      }

      for( j = 0 ; '\0' != gc_items_types[j][0] ; j++ ) {
         if( 0 == memory_strncmp_ptr(
            type_buffer, gc_items_types[j], type_buffer_sz )
         ) {
            debug_printf( 1, "item is type: %s (%d)", gc_items_types[j], j );
            t->items[i].type = j;
            break;
         }
      }

      /* name */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_NAME, i );
      name_buffer_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         t->items[i].name, ITEM_NAME_SZ,
         &(tokens[0]), tokens_sz, json_buffer );
      if( 0 >= name_buffer_sz ) {
         error_printf( "invalid item returned (loaded %d)", i );
         break;
      }

      /* sprite */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_SPRITE, i );
      sprite_buffer_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         sprite_buffer, RESOURCE_PATH_MAX,
         &(tokens[0]), tokens_sz, json_buffer );
      if( 0 >= sprite_buffer_sz ) {
         error_printf( "invalid item sprite returned (loaded %d)", i );
         break;
      }
      resource_assign_id( t->items[i].sprite, sprite_buffer );

      /* gid */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_GID, i );
      t->items[i].gid = json_int_from_path(
         iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

      /* flags */
      t->items[i].flags |= ITEM_FLAG_ACTIVE;

      /* count */
      /* TODO: Parse count from map. */
      t->items[i].count = 1;
   }

   return i;
}

int16_t tilemap_json_load( RESOURCE_ID id, struct TILEMAP* t ) {
   char* json_buffer = NULL;
   int16_t retval = 1;
   RESOURCE_HANDLE json_handle = (RESOURCE_HANDLE)0;
   MEMORY_HANDLE tokens_handle = (MEMORY_HANDLE)0;
   uint32_t json_buffer_sz = 0;
   int16_t ts_name_sz = 0,
      tok_parsed = 0;
   char ts_name[JSON_PATH_SZ];
   struct jsmntok* tokens = NULL;
   int i = 0;

   memory_zero_ptr( t, sizeof( struct TILEMAP ) );

   /* Allocate buffers for parsing JSON. */
   json_handle = resource_get_handle( id );
   if( (RESOURCE_HANDLE)0 == json_handle ) {
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
      retval = 0;
      goto cleanup;
   }

   /* Parse properties, first pass. */

   debug_printf( 1, "getting tilemap name and tileset path" );

   json_str_from_path(
      TILEMAP_JPATH_PROP_NAME, sizeof( TILEMAP_JPATH_PROP_NAME ),
      t->name, TILEMAP_NAME_MAX, tokens, tok_parsed, json_buffer );

#if 0
   tilemap_json_parse_engine(
      t, json_buffer, json_buffer_sz, tokens, tok_parsed );
#endif

   tilemap_json_parse_weather(
      t, json_buffer, json_buffer_sz, tokens, tok_parsed );

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
      TILEMAP_SPAWNS_MAX > i &&
      tilemap_json_parse_spawn(
         t, i, tokens, tok_parsed, json_buffer, json_buffer_sz, id
      )
   ) {
      
      /* Iterate to the next spawn. */
      i++;
   }

   tilemap_json_parse_items(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX );

   tilemap_json_parse_strings(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX );

   tilemap_json_parse_scripts(
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

   if( (RESOURCE_HANDLE)0 != json_handle ) {
      resource_free_handle( json_handle );
   }

   return retval;
}


