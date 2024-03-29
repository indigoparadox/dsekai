
#include "tmjson.h"

#ifndef RESOURCE_FILE
#error Loading maps from JSON requires file resources!
#endif /* !RESOURCE_FILE */

/**
 * \brief Given a relative path (e.g. assets/16x16x16/s_mob.bmp), write the
 *        extracted resource name (s_mob, in this case) into a buffer.
 */
static int16_t tilemap_json_parse_resource_name(
   RESOURCE_NAME name_buffer, const char* json_path,
   struct jsmntok* tokens, int16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   const RESOURCE_NAME map_path
) {
   char file_path_buffer[RESOURCE_PATH_MAX + 1] = { 0 };
   int16_t name_idx = 0,
      file_path_buffer_sz = 0,
      name_sz = 0,
      i = 0,
      retval = 1;

#ifndef NO_FIX_ASSET_PATH
   /* Prepend asset path. */
   /* file_path_buffer_sz = tilemap_fix_asset_path(
      file_path_buffer, RESOURCE_PATH_MAX, map_path ); */
#endif /* NO_FIX_ASSET_PATH */

   debug_printf( 3, "parsing path from: %s", json_path );

   /* Parse the sprite into the path buffer after any potential fixes. */
   file_path_buffer_sz = json_str_from_path(
      json_path, JSON_PATH_SZ,
      &(file_path_buffer[file_path_buffer_sz]),
      RESOURCE_PATH_MAX - file_path_buffer_sz,
      tokens, tokens_sz, json_buffer );

   if( 0 >= file_path_buffer_sz ) {
      error_printf( "could not parse string from: %s", json_path );
      retval = 0;
      goto cleanup;
   }

   /* Get the position/size of the name in the path. */
   while( '\0' != file_path_buffer[i] ) {
      /* TODO: Check for various separators? */
      if( '/' == file_path_buffer[i] ) {
         name_idx = i + 1;
      } else if( '.' == file_path_buffer[i] ) {
         name_sz = i - name_idx;
      }
      i++;
   }

   memory_zero_ptr( name_buffer, RESOURCE_NAME_MAX );
   memory_strncpy_ptr( name_buffer, &(file_path_buffer[name_idx]), name_sz );

   debug_printf( 3, "parsed resource name: %s", name_buffer );

cleanup:

   return retval;
}

static int16_t tilemap_json_parse_spawn(
   struct TILEMAP* t, int16_t spawn_idx,
   struct jsmntok* tokens, int16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   const RESOURCE_NAME map_path
) {
   struct TILEMAP_SPAWN* spawn = (struct TILEMAP_SPAWN*)&(t->spawns[spawn_idx]);
   char iter_path[JSON_PATH_SZ] = { 0 };
   int16_t ascii_buffer_sz = 0,
      x_px_in = 0,
      y_px_in = 0;
   uint8_t mobile_flag = 0;
   char ascii_buffer[2] = { 0 };
   int32_t gid_buffer = 0;

   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_SPRITE, spawn_idx );

   if(
      !tilemap_json_parse_resource_name(
         spawn->sprite_name, iter_path, tokens, tokens_sz,
         json_buffer, json_buffer_sz, map_path )
   ) {
      return 0;
   }

   /* Parse ASCII */
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_ASCII, spawn_idx );
   ascii_buffer_sz = json_str_from_path(
      iter_path, JSON_PATH_SZ, ascii_buffer, 2,
      tokens, tokens_sz, json_buffer );
   if( 0 < ascii_buffer_sz ) {
      spawn->ascii = ascii_buffer[0];
      debug_printf( 2, "mobile ASCII: %c", spawn->ascii );
   }

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

   debug_printf( 1, "spawn \"%s\" coords: %d, %d",
      spawn->name, spawn->coords.x, spawn->coords.y );

   /* Parse Script */
   spawn->script_id = -1;
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_SCRIPT, spawn_idx );
   spawn->script_id = json_int_from_path(
      iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

   /* Parse Type (part of Flags) */
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_TYPE_FLAG, spawn_idx );
   mobile_flag = json_int_from_path(
      iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );
   if( 1 == mobile_flag ) {
      debug_printf( 1, "spawn \"%s\" mobile type: %d",
         spawn->name, mobile_flag );
      spawn->flags |= ((mobile_flag << 3) & MOBILE_TYPE_MASK);
   }

   /* Parse Player (part of Flags) */
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_PLAYER_FLAG, spawn_idx );
   mobile_flag = json_int_from_path(
      iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );
   if( 1 == mobile_flag ) {
      debug_printf( 1, "spawn \"%s\" mobile player: %d",
         spawn->name, mobile_flag );
      spawn->flags |= MOBILE_FLAG_PLAYER;
   }

   /* Parse GID */
   dio_snprintf(
      iter_path, JSON_PATH_SZ, TILEMAP_JPATH_MOB_GID, spawn_idx );
   gid_buffer = json_int_from_path(
      iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

   if( MOBILE_FLAG_PLAYER == (MOBILE_FLAG_PLAYER & spawn->flags) ) {
      /* Make sure the player GID is correct, regardless of the map format. */
      spawn->gid = SPAWN_GID_PLAYER;
   } else {
      /* Make sure the GID fits within the system GID size. */
      spawn->gid = gid_buffer;
      debug_printf( 1, "%d into %d", gid_buffer, spawn->gid );
      assert( 0 > gid_buffer || spawn->gid == gid_buffer );
   }

   debug_printf( 1, "spawn %d flags: 0x%04x", spawn->gid, spawn->flags );

   if( 0 > spawn->script_id || spawn->script_id  >= TILEMAP_SCRIPTS_MAX ) {
      error_printf( "spawn \"%s\" uses invalid script ID: %d",
         spawn->name, spawn->script_id );
   }

   debug_printf( 2, "%s spawn at %d, %d (script %d)",
      spawn->sprite_name, spawn->coords.x, spawn->coords.y, spawn->script_id );

   return 1;
}

static int16_t tilemap_json_parse_tileset_tile(
   struct TILEMAP* t, int tile_idx,
   struct jsmntok* tokens, int16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   const RESOURCE_NAME map_path
) {
   char tile_json_path[JSON_PATH_SZ] = { 0 };
   char tile_ascii[2] = { 0 };
   int16_t tile_ascii_sz = 0;

   /* Load each tile bitmap. */
   dio_snprintf(
      tile_json_path, JSON_PATH_SZ, TILEMAP_JPATH_TS_TILE, tile_idx );

   if(
      !tilemap_json_parse_resource_name(
         t->tileset[tile_idx].image_name, tile_json_path, tokens, tokens_sz,
         json_buffer, json_buffer_sz, map_path )
   ) {
      return 0;
   }

   t->tileset[tile_idx].image_cache_id = -1;

   /* Parse ASCII */
   dio_snprintf(
      tile_json_path, JSON_PATH_SZ, TILEMAP_JPATH_TS_ASCII, tile_idx );
   tile_ascii_sz = json_str_from_path(
      tile_json_path, JSON_PATH_SZ, tile_ascii, 2,
      tokens, tokens_sz, json_buffer );
   if( 0 < tile_ascii_sz ) {
      t->tileset[tile_idx].ascii = tile_ascii[0];
      debug_printf( 2, "tile ASCII: %c", t->tileset[tile_idx].ascii );
   }

   /* Load tile flags. */
   dio_snprintf(
      tile_json_path, JSON_PATH_SZ, TILEMAP_JPATH_TS_FLAGS, tile_idx );
   t->tileset[tile_idx].flags = json_int_from_path(
      tile_json_path, JSON_PATH_SZ, tokens, tokens_sz, json_buffer );
   debug_printf(
      2, "tile flags: %s, %d\n", tile_json_path, t->tileset[tile_idx].flags );

   t->tileset[tile_idx].flags |= TILESET_FLAG_ACTIVE;

   return 1;
}

static int16_t tilemap_json_parse_tileset(
   struct TILEMAP* t,
   struct jsmntok* tokens, uint16_t tokens_sz,
   char* json_buffer, uint16_t json_buffer_sz,
   const RESOURCE_ID map_path
) {
   int16_t i = 0,
      tile_retval = 0;

   do {
      tile_retval = tilemap_json_parse_tileset_tile(
         t, i, tokens, tokens_sz, json_buffer, json_buffer_sz, map_path );
      i++;
   } while( tile_retval );

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
   tile_id_in = json_int_from_path(
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

static int16_t tilemap_json_parse_scripts(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz, const RESOURCE_ID map_path
) {
   char iter_path[JSON_PATH_SZ];
   int16_t i = 0,
      j = 0,
      script_buffer_sz = 0,
      script_path_sz = 0,
      loaded = 0;
   char* script_buffer = NULL;
   char script_path[RESOURCE_PATH_MAX + 1] = { 0 };
   struct SCRIPT_COMPILE_STATE s;
   RESOURCE_HANDLE script_buffer_h = (RESOURCE_HANDLE)0;

   /* Load scripts.*/
   debug_printf( 2, "loading scripts" ); 
   for( i = 0 ; TILEMAP_SCRIPTS_MAX > i ; i++ ) {
      memory_zero_ptr( script_path, RESOURCE_PATH_MAX + 1 );
      memory_zero_ptr( &s, sizeof( struct SCRIPT_COMPILE_STATE ) );
      memory_zero_ptr(
         &(t->scripts[i]), sizeof( struct SCRIPT_STEP ) * SCRIPT_STEPS_MAX );

#ifndef NO_FIX_ASSET_PATH
      script_path_sz = tilemap_fix_asset_path(
         script_path, RESOURCE_PATH_MAX, map_path );
#endif /* NO_FIX_ASSET_PATH */

      /* Grab the script file path from the tilemap. */
      dio_snprintf(
         iter_path, JSON_PATH_SZ, TILEMAP_JPATH_SCRIPT, i );
      script_path_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         &(script_path[script_path_sz]), RESOURCE_PATH_MAX - script_path_sz,
         &(tokens[0]), tokens_sz, json_buffer );
      if( 0 >= script_path_sz ) {
         error_printf( "invalid script path returned (loaded %d)", loaded );
         goto cleanup;
      }

      debug_printf( 1, "opening script %s...", script_path );
      script_buffer_h = resource_get_handle( script_path );
      if( (RESOURCE_HANDLE)0 == script_buffer_h ) {
         error_printf( "could not get handle for %s!", script_path );
         loaded = -1;
         goto cleanup;
      }

      debug_printf( 2, "compiling script #%d: %s...", i, script_path );

      script_buffer_sz = memory_sz( script_buffer_h );
      script_buffer = memory_lock( script_buffer_h );
      assert( NULL != script_buffer );
      s.steps = t->scripts[i].steps;

      for( j = 0 ; script_buffer_sz > j ; j++ ) {
         script_parse_src( script_buffer[j], &s );
      }

      debug_printf( 1, "script #%d is %d steps long", i, s.steps_sz );
      t->scripts[i].steps_count = s.steps_sz;

      script_buffer = memory_unlock( script_buffer_h );
      memory_free( script_buffer_h );
      script_buffer_h = (RESOURCE_HANDLE)0;

      /*
      script_parse_str( i, script_buffer, script_buffer_sz, &(t->scripts[i]) );
      */

      loaded++;
   }

cleanup:

   if( NULL != script_buffer ) {
      script_buffer = memory_unlock( script_buffer_h );
   }

   if( (RESOURCE_HANDLE)0 != script_buffer_h ) {
      memory_free( script_buffer_h );
   }

   return loaded;
}

static void tilemap_json_parse_strings(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz
) {
   char iter_path[JSON_PATH_SZ],
      string_buf[TILEMAP_JSON_STRBUF_SZ];
   int16_t string_sz_tmp = 0;
   int8_t str_idx = 0;

   strpool_init( t->strpool, TILEMAP_STRPOOL_SZ );
   
   /* Load strings.*/
   debug_printf( 2, "loading strings" ); 
   do {
      dio_snprintf(
         iter_path, JSON_PATH_SZ, TILEMAP_JPATH_STRING, str_idx );
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
         str_idx++;
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
   const char* filename, char** json_buffer_p, RESOURCE_HANDLE* json_handle_p,
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
   t->engine_type = 2 /* POV */;
}
#endif

void tilemap_json_parse_flags(
   struct TILEMAP* t, char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, int16_t tokens_sz
) {
   char weather[TILEMAP_NAME_MAX];
   int16_t editable = 0;

   json_str_from_path(
      TILEMAP_JPATH_PROP_WEATHER, sizeof( TILEMAP_JPATH_PROP_WEATHER ),
      weather, TILEMAP_NAME_MAX, tokens, tokens_sz, json_buffer );

   if( 0 == memory_strncmp_ptr( weather, "snow", 4 ) ) {
      t->flags |= TILEMAP_FLAG_WEATHER_SNOW;
   } else if( 0 == memory_strncmp_ptr( weather, "rain", 4 ) ) {
      t->flags |= TILEMAP_FLAG_WEATHER_RAIN;
   }

   editable = json_int_from_path(
      TILEMAP_JPATH_PROP_EDITABLE,
      JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );
   if( 0 < editable ) {
      t->flags |= TILEMAP_FLAG_EDITABLE;
   }
}

int16_t tilemap_json_parse_items(
   struct TILEMAP* t,
   char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, int16_t tokens_sz,
   const RESOURCE_ID map_path
) {
   char iter_path[JSON_PATH_SZ];
   int16_t i = 0,
      j = 0,
      sprite_buffer_sz = 0,
      type_buffer_sz = 0,
      name_buffer_sz = 0;
   char sprite_buffer[RESOURCE_PATH_MAX + 1] = { 0 },
      type_buffer[ITEM_NAME_SZ + 1];
   
   /* Load items.*/
   debug_printf( 2, "loading items" ); 
   /* TODO: Load the number of items in the map, not TILEMAP_ITEMS_MAX! */
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
            item_set_type_flag( &(t->item_defs[i]), j );
            break;
         }
      }

      /* name */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_NAME, i );
      name_buffer_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         t->item_defs[i].name, ITEM_NAME_SZ,
         &(tokens[0]), tokens_sz, json_buffer );
      if( 0 >= name_buffer_sz ) {
         error_printf( "invalid item returned (loaded %d)", i );
         break;
      }
      
      debug_printf( 1, "found item: %s", t->item_defs[i].name );

      /* sprite */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_SPRITE, i );
      if(
         !tilemap_json_parse_resource_name(
            t->item_defs[i].sprite_name, iter_path, tokens, tokens_sz,
            json_buffer, json_buffer_sz, map_path )
      ) {
         error_printf( "invalid item sprite returned (loaded %d)", i );
         break;
      }
      t->item_defs[i].sprite_cache_id = -1;

      /* gid */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_GID, i );
      t->item_defs[i].gid = json_int_from_path(
         iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

      /* data */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_DATA, i );
      t->item_defs[i].data = json_int_from_path(
         iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

      /* craftable  */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_ITEM_CRAFTABLE, i );
      /* -1 or 1 is true, only 0 is false. This way flag is optional/defaults
       * to true. */
      if( json_bool_from_path(
         iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer
      ) ) {
         debug_printf( 1, "item %d is craftable", i );
         t->item_defs[i].flags |= ITEM_FLAG_CRAFTABLE;
      } else {
         debug_printf( 1, "item %d is NOT craftable", i );
         t->item_defs[i].flags &= ~ITEM_FLAG_CRAFTABLE;
      }

      /* active */
      t->item_defs[i].flags |= ITEM_FLAG_ACTIVE | ITEM_FLAG_NOT_LAST;
   }

   return i;
}

int16_t tilemap_json_parse_crop_defs(
   struct TILEMAP* t,
   char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, int16_t tokens_sz,
   const RESOURCE_ID map_path
) {
   char iter_path[JSON_PATH_SZ];
   int16_t i = 0,
      sprite_buffer_sz = 0,
      name_buffer_sz = 0;
   char sprite_buffer[RESOURCE_PATH_MAX + 1] = { 0 };
   
   /* Load crop definitions.*/
   debug_printf( 2, "loading crop definitions" ); 
   /* TODO: Load number of crop defs in the map, not TILEMAP_CROP_DEFS_MAX! */
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      
      /* name */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_CROP_DEF_NAME, i );
      name_buffer_sz = json_str_from_path(
         iter_path, JSON_PATH_SZ,
         t->crop_defs[i].name, CROP_NAME_MAX,
         &(tokens[0]), tokens_sz, json_buffer );
      if( 0 >= name_buffer_sz ) {
         error_printf( "invalid crop definition returned (loaded %d)", i );
         break;
      }

      debug_printf( 1, "found crop definition: %s", t->crop_defs[i].name );

      /* sprite */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_CROP_DEF_SPRITE, i );
      if(
         !tilemap_json_parse_resource_name(
            t->crop_defs[i].sprite_name, iter_path, tokens, tokens_sz,
            json_buffer, json_buffer_sz, map_path )
      ) {
         error_printf(
            "invalid crop definition sprite returned (loaded %d)", i );
         break;
      }
      t->crop_defs[i].sprite_cache_id = -1;

      /* gid */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_CROP_DEF_GID, i );
      t->crop_defs[i].gid = json_int_from_path(
         iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

      debug_printf( 1, "crop gid: %d", t->crop_defs[i].gid );

      /* cycle */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_CROP_DEF_CYCLE, i );
      t->crop_defs[i].cycle = json_int_from_path(
         iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

      debug_printf( 1, "crop crow cycle: %d ticks", t->crop_defs[i].cycle );

      t->crop_defs[i].flags = 0;

      /* regrows */
      dio_snprintf( iter_path, JSON_PATH_SZ, TILEMAP_JPATH_CROP_DEF_CYCLE, i );
      if( json_bool_from_path(
         iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer
      ) ) {
         debug_printf( 1, "crop %s regrows", t->crop_defs[i].name );
         t->crop_defs[i].flags |= CROP_DEF_FLAG_REGROWS;
      } else {
         debug_printf( 1, "crop %s does NOT regrow", t->crop_defs[i].name );
      }

      /* activate */
      t->crop_defs[i].flags |= CROP_DEF_FLAG_ACTIVE;

      /* produce gid */
      dio_snprintf(
         iter_path, JSON_PATH_SZ, TILEMAP_JPATH_CROP_DEF_PRODUCE_GID, i );
      t->crop_defs[i].produce_gid = json_int_from_path(
         iter_path, JSON_PATH_SZ, &(tokens[0]), tokens_sz, json_buffer );

   }

   return i;
}

int16_t tilemap_json_load( const RESOURCE_ID id, struct TILEMAP* t ) {
   char* json_buffer = NULL;
   int16_t retval = 1;
   RESOURCE_HANDLE json_handle = (RESOURCE_HANDLE)0;
   MEMORY_HANDLE tokens_handle = (MEMORY_HANDLE)0;
   uint32_t json_buffer_sz = 0;
   int16_t ts_name_sz = 0,
      tok_parsed = 0;
   char ts_name[JSON_PATH_SZ];
   struct jsmntok* tokens = NULL;
   int8_t spawn_idx = 0;
   int32_t gid_buffer = 0;

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

   gid_buffer = json_int_from_path(
      TILEMAP_JPATH_GID, JSON_PATH_SZ, tokens, tok_parsed, json_buffer );

   /* Make sure the GID fits within the system GID size. */
   t->gid = gid_buffer;
   debug_printf( 1, "%d into %d", gid_buffer, t->gid );
   assert( 0 > gid_buffer || t->gid == gid_buffer );

   json_str_from_path(
      TILEMAP_JPATH_PROP_NAME, sizeof( TILEMAP_JPATH_PROP_NAME ),
      t->name, TILEMAP_NAME_MAX, tokens, tok_parsed, json_buffer );

   tilemap_json_parse_flags(
      t, json_buffer, json_buffer_sz, tokens, tok_parsed );

   debug_printf( 2, "tilemap %s (%d) flags: %02x", t->name, t->gid, t->flags );

   /* XXX: Is fix_asset_path being used correctly here? */
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
      TILEMAP_SPAWNS_MAX > spawn_idx &&
      tilemap_json_parse_spawn(
         t, spawn_idx, tokens, tok_parsed, json_buffer, json_buffer_sz, id
      )
   ) {
      
      /* Iterate to the next spawn. */
      spawn_idx++;
   }

   tilemap_json_parse_items(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX, id );

   tilemap_json_parse_crop_defs(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX, id );

   tilemap_json_parse_strings(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX );

   if( 0 > tilemap_json_parse_scripts(
      t, json_buffer, json_buffer_sz, tokens, JSON_TOKENS_MAX, id )
   ) {
      error_printf( "error parsing tilemap scripts!" );
      retval = 0;
      goto cleanup;
   }

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


