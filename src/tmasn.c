
#include "dsekai.h"

#include "tmasn.h"

/* TODO: Use semantic variable names from serial/asn.c (e.g. p_asn_buffer_h). */

/* TODO: Convert to use new serial_* macros. */

/* TODO: Merge into new serial/asn.c. */

/* Private Prototypes w/ Sections */

int16_t tilemap_asn_parse_tileset(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) SECTION_ASN;
int16_t tilemap_asn_parse_tiles(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) SECTION_ASN;
int16_t tilemap_asn_parse_strings(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) SECTION_ASN;
int16_t tilemap_asn_parse_spawns(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) SECTION_ASN;
int16_t tilemap_asn_parse_items(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) SECTION_ASN;
int16_t tilemap_asn_parse_crop_defs(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) SECTION_ASN;
int32_t tilemap_asn_parse_scripts(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) SECTION_ASN;

/* Function Definitions */

int16_t tilemap_asn_parse_tileset(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) {
   int32_t ts_seq_sz = 0;
   uint8_t type_buf = 0;
   int16_t total_read_sz = 0,
      read_sz = 0,
      tile_idx = 0;

   serial_asn_read_seq( asn_buffer, &type_buf, &ts_seq_sz,
      "tilemap tileset", total_read_sz, read_sz, cleanup )

   while( total_read_sz - 4 < ts_seq_sz ) {
      if( 0x30 != asn_buffer[total_read_sz] ) {
         error_printf(
            "invalid tiledef sequence type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      total_read_sz += 2; /* tile sequence type, size */
      
      /* image name */
      read_sz = asn_read_string( t->tileset[tile_idx].image_name,
         RESOURCE_NAME_MAX, asn_buffer, total_read_sz );
      if( 0 == read_sz ) {
         goto cleanup;
      }
      debug_printf( 2, "tile image name: %s (%d)",
         t->tileset[tile_idx].image_name, read_sz );
      t->tileset[tile_idx].image_cache_id = -1;
      total_read_sz += read_sz; /* tile image and header */

      /* ascii */
      read_sz = asn_read_int(
         &(t->tileset[tile_idx].ascii), 1, 0, asn_buffer, total_read_sz );
      if( 0 == read_sz ) {
         goto cleanup;
      }
      debug_printf( 2, "tile ASCII: %d", t->tileset[tile_idx].ascii );
      total_read_sz += read_sz;


      /* flags */
      read_sz = asn_read_int(
         &(t->tileset[tile_idx].flags), 1, 0, asn_buffer, total_read_sz );
      if( 0 == read_sz ) {
         goto cleanup;
      }
      debug_printf( 2, "tile flags: %d", t->tileset[tile_idx].flags );
      total_read_sz += read_sz;

      tile_idx++;
   }

   assert( total_read_sz - 4 == ts_seq_sz );

cleanup:

   return total_read_sz;
}

int16_t tilemap_asn_parse_tiles(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) {
   int16_t total_read_sz = 0,
      t_seq_sz = 0;

   if( 0x04 != asn_buffer[0] ) {
      error_printf(
         "invalid tile blob type byte: 0x%02x", asn_buffer[0] );
      goto cleanup;
   }
   t_seq_sz = asn_read_short( asn_buffer, 2 );
   if( t_seq_sz != TILEMAP_TS ) {
      error_printf( "invalid tile sequence size: %d", t_seq_sz );
      goto cleanup;
   }
   debug_printf( 2, "tile sequence size: %d bytes", t_seq_sz );

   total_read_sz += 4; /* sequence type, length fields */

   while( total_read_sz - 4 < t_seq_sz ) {
      t->tiles[total_read_sz - 4] = asn_buffer[total_read_sz];
      total_read_sz++;
      assert( total_read_sz - 4 <= TILEMAP_TS );
   }

cleanup:

   return total_read_sz;
}

int16_t tilemap_asn_parse_strings(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) {
   int16_t total_read_sz = 0,
      strpool_sz = 0;

   debug_printf( 2, "parsing strings..." );

   if( 0x04 != asn_buffer[0] ) {
      error_printf(
         "invalid tile blob type byte: 0x%02x", asn_buffer[0] );
      goto cleanup;
   }
   strpool_sz = asn_read_short( asn_buffer, 2 );
   if( strpool_sz != TILEMAP_STRPOOL_SZ ) {
      error_printf( "invalid strpool sequence size: %d", strpool_sz );
      goto cleanup;
   }
   debug_printf( 2, "strpool sequence size: %d bytes", strpool_sz );

   total_read_sz += 4; /* sequence type, length fields */

   while( total_read_sz - 4 < strpool_sz ) {
      t->strpool[total_read_sz - 4] = asn_buffer[total_read_sz];
      total_read_sz++;
   }

cleanup:

   return total_read_sz;
}

int16_t tilemap_asn_parse_spawns(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) {
   uint8_t type_buf = 0;
   int32_t total_read_sz = 0,
      read_sz = 0,
      all_spawns_seq_start = 0,
      all_spawns_seq_sz = 0,
      spawn_def_seq_sz = 0,
      coords_seq_sz = 0,
      spawn_idx = 0;

   serial_asn_read_seq( asn_buffer, &type_buf, &all_spawns_seq_sz,
      "tilemap spawn defs", total_read_sz, read_sz, cleanup )

   all_spawns_seq_start = total_read_sz;

   while( total_read_sz - all_spawns_seq_start < all_spawns_seq_sz ) {
      serial_asn_read_seq( asn_buffer, &type_buf, &spawn_def_seq_sz,
         "spawn def", total_read_sz, read_sz, cleanup )

      serial_asn_read_string(
         asn_buffer, t->spawns[spawn_idx].name, TILEMAP_SPAWN_NAME_SZ,
         "spawn def name", total_read_sz, read_sz, cleanup )

      serial_asn_read_seq( asn_buffer, &type_buf, &coords_seq_sz,
         "spawn def coords", total_read_sz, read_sz, cleanup )

      serial_asn_read_int(
         asn_buffer, (&t->spawns[spawn_idx].coords.x), 1, 0,
         "spawn def coords X", total_read_sz, read_sz, cleanup )

      serial_asn_read_int(
         asn_buffer, (&t->spawns[spawn_idx].coords.y), 1, 0,
         "spawn def coords Y", total_read_sz, read_sz, cleanup )

      serial_asn_read_string(
         asn_buffer, t->spawns[spawn_idx].sprite_name, RESOURCE_NAME_MAX,
         "spawn def sprite name", total_read_sz, read_sz, cleanup )

      serial_asn_read_int(
         asn_buffer, (&t->spawns[spawn_idx].flags), 1, 0,
         "spawn def ASCII", total_read_sz, read_sz, cleanup )

      serial_asn_read_int(
         asn_buffer, (&t->spawns[spawn_idx].flags), 2, 0,
         "spawn flags", total_read_sz, read_sz, cleanup )

      serial_asn_read_int(
         asn_buffer, (&t->spawns[spawn_idx].gid), sizeof( SPAWN_GID ), 0,
         "spawn def GID", total_read_sz, read_sz, cleanup )

      serial_asn_read_int(
         asn_buffer, (&t->spawns[spawn_idx].script_id), 2, ASN_FLAG_SIGNED,
         "spawn def script ID", total_read_sz, read_sz, cleanup )

      spawn_idx++;
   }

cleanup:

   return total_read_sz;
}

int16_t tilemap_asn_parse_items(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) {
   uint8_t type_buf = 0;
   int32_t total_read_sz = 0,
      read_sz = 0,
      all_items_seq_start = 0,
      all_items_seq_sz = 0,
      item_def_seq_sz = 0;
   int16_t item_idx = 0;

   serial_asn_read_seq( asn_buffer, &type_buf, &all_items_seq_sz,
      "tilemap item defs", total_read_sz, read_sz, cleanup )

   all_items_seq_start = total_read_sz;

   while( total_read_sz - all_items_seq_start < all_items_seq_sz ) {
      serial_asn_read_seq( asn_buffer, &type_buf, &item_def_seq_sz,
         "item def", total_read_sz, read_sz, cleanup )
      
      /* index */
      read_sz = asn_read_int(
         (uint8_t*)&item_idx, 2, 0, asn_buffer, total_read_sz );
      total_read_sz += read_sz;

      /* sprite name */
      read_sz = asn_read_string( t->item_defs[item_idx].sprite_name,
         RESOURCE_NAME_MAX, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d sprite name: %s (%d)",
         item_idx, t->item_defs[item_idx].sprite_name, read_sz );
      total_read_sz += read_sz; /* item sprite and header */

      /* name */
      read_sz = asn_read_string( t->item_defs[item_idx].name,
            ITEM_NAME_SZ, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading item name" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d name: %s (%d)",
         item_idx, t->item_defs[item_idx].name, read_sz );
      total_read_sz += read_sz; /* item name and header */

      /* owner */
      read_sz = asn_read_int(
         (uint8_t*)&(t->item_defs[item_idx].owner), 1, 0,
         asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading item owner" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d owner: %d (%d)",
         item_idx, t->item_defs[item_idx].owner, read_sz );
      total_read_sz += read_sz;

      /* gid */
      read_sz = asn_read_int(
         (uint8_t*)&(t->item_defs[item_idx].gid), 2, 0, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading item gid" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d gid: %d (%d)",
         item_idx, t->item_defs[item_idx].gid, read_sz );
      total_read_sz += read_sz;

      /* data */
      read_sz = asn_read_int(
         &(t->item_defs[item_idx].data), 1, 0, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading item data" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d data: %d (%d)",
         item_idx, t->item_defs[item_idx].data, read_sz );
      total_read_sz += read_sz;

      /* flags */
      read_sz = asn_read_int(
        (uint8_t*)&(t->item_defs[item_idx].flags), 2, 0,
        asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading item flags" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d flags: %d (%d)",
         item_idx, t->item_defs[item_idx].flags, read_sz );
      total_read_sz += read_sz;

   }

cleanup:

   return total_read_sz;
}

int16_t tilemap_asn_parse_crop_defs(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) {
   uint8_t type_buf = 0;
   int32_t total_read_sz = 0,
      read_sz = 0,
      all_crops_seq_start = 0,
      all_crops_seq_sz = 0,
      crop_def_seq_sz = 0;
   int16_t crop_idx = 0;

   serial_asn_read_seq( asn_buffer, &type_buf, &all_crops_seq_sz,
      "tilemap crop defs", total_read_sz, read_sz, cleanup )

   all_crops_seq_start = total_read_sz;

   while( total_read_sz - all_crops_seq_start < all_crops_seq_sz ) {
      serial_asn_read_seq( asn_buffer, &type_buf, &crop_def_seq_sz,
         "crop def", total_read_sz, read_sz, cleanup )
      
      /* index */
      read_sz = asn_read_int(
         (uint8_t*)&crop_idx, 2, 0, asn_buffer, total_read_sz );
      total_read_sz += read_sz;

      /* sprite name */
      read_sz = asn_read_string( t->crop_defs[crop_idx].sprite_name,
         RESOURCE_PATH_MAX, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d sprite: %s (%d)",
         crop_idx, t->crop_defs[crop_idx].sprite_name, read_sz );
      total_read_sz += read_sz; /* crop def sprite and header */
      t->crop_defs[crop_idx].sprite_cache_id = -1;

      /* name */
      read_sz = asn_read_string( t->crop_defs[crop_idx].name,
            CROP_NAME_MAX, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def name" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop %d name: %s (%d)",
         crop_idx, t->crop_defs[crop_idx].name, read_sz );
      total_read_sz += read_sz; /* crop def name and header */

      /* gid */
      read_sz = asn_read_int(
         &(t->crop_defs[crop_idx].gid), 1, 0, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def gid" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d gid: %d (%d)",
         crop_idx, t->crop_defs[crop_idx].gid, read_sz );
      total_read_sz += read_sz;

      /* flags */
      read_sz = asn_read_int(
         &(t->crop_defs[crop_idx].flags), 1, 0, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def flags" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d flags: %d (%d)",
         crop_idx, t->crop_defs[crop_idx].flags, read_sz );
      total_read_sz += read_sz;

      /* cycle */
      read_sz = asn_read_int(
         (uint8_t*)&(t->crop_defs[crop_idx].cycle), 2, 0,
         asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def cycle" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d cycle: %d (%d)",
         crop_idx, t->crop_defs[crop_idx].cycle, read_sz );
      total_read_sz += read_sz;

      /* produce gid */
      read_sz = asn_read_int(
         (uint8_t*)&(t->crop_defs[crop_idx].produce_gid),
         2, 0, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def produce gid" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d produce gid: %d (%d)",
         crop_idx, t->crop_defs[crop_idx].produce_gid, read_sz );
      total_read_sz += read_sz;

   }

cleanup:

   return total_read_sz;
}

int32_t tilemap_asn_parse_scripts(
   struct TILEMAP* t, const uint8_t* asn_buffer, int32_t asn_buffer_sz
) {
   uint8_t type_buf = 0,
      step_idx = 0,
      script_idx = 0;
   int32_t script_seq_sz = 0,
      step_seq_sz = 0,
      all_scripts_seq_sz = 0,
      total_read_sz = 0,
      script_seq_start = 0,
      read_sz = 0;

   serial_asn_read_seq( asn_buffer, &type_buf, &all_scripts_seq_sz,
      "tilemap scripts", total_read_sz, read_sz, cleanup )

   while( total_read_sz < all_scripts_seq_sz ) {
      debug_printf( 1, "script idx: %d", script_idx );

      step_idx = 0;
      script_seq_start = total_read_sz;

      serial_asn_read_seq( asn_buffer, &type_buf, &script_seq_sz,
         "script", total_read_sz, read_sz, cleanup )

      while( total_read_sz - script_seq_start < script_seq_sz ) {

         serial_asn_read_seq( asn_buffer, &type_buf, &step_seq_sz,
            "script step", total_read_sz, read_sz, cleanup )

         /* step.action */
         read_sz = asn_read_int(
            (uint8_t*)&(t->scripts[script_idx].steps[step_idx].action),
            2, 0, asn_buffer, total_read_sz );
         if( 0 == read_sz ) {
            error_printf( "unable to read action" );
            total_read_sz = TILEMAP_ASN_ERROR_READ;
            goto cleanup;
         }
         total_read_sz += read_sz;

         /* step.arg */
         read_sz = asn_read_int(
            (uint8_t*)&(t->scripts[script_idx].steps[step_idx].arg),
            2, 1, asn_buffer, total_read_sz );
         if( 0 == read_sz ) {
            error_printf( "unable to read arg" );
            total_read_sz = TILEMAP_ASN_ERROR_READ;
            goto cleanup;
         }
         total_read_sz += read_sz;

         debug_printf( 1, "step idx: %d, action: %d, arg: %d",
            step_idx,
            t->scripts[script_idx].steps[step_idx].action,
            t->scripts[script_idx].steps[step_idx].arg );

         step_idx++;
      }

      t->scripts[script_idx].steps_count = step_idx;
      script_idx++;
   }

cleanup:

   return total_read_sz;
}

int32_t tilemap_asn_load_res( RESOURCE_ID id, struct TILEMAP* t ) {
   const uint8_t* asn_buffer = NULL;
   int32_t asn_buffer_sz = 0,
      read_sz = 0;
   RESOURCE_HANDLE asn_handle = (RESOURCE_HANDLE)0;

   /* Allocate buffers for parsing ASN.1. */
   asn_handle = resource_get_handle( id );
   if( (RESOURCE_HANDLE)0 == asn_handle ) {
      error_printf( "could not get tilemap resource handle" );
      read_sz = 0;
      goto cleanup;
   }

   asn_buffer_sz = resource_sz_handle( asn_handle );
   if( 0 > asn_buffer_sz ) {
      error_printf( "invalid asn buffer size: %d", asn_buffer_sz );
      read_sz = 0;
      goto cleanup;
   }

   asn_buffer = resource_lock_handle( asn_handle );
   if( NULL == asn_buffer ) {
      error_printf( "could not get tilemap resource pointer" );
      read_sz = 0;
      goto cleanup;
   }

   read_sz = tilemap_asn_load( asn_buffer, asn_buffer_sz, t );

cleanup:

   if( NULL != asn_buffer ) {
      resource_unlock_handle( asn_handle );
   }

   if( (RESOURCE_HANDLE)NULL != asn_handle ) {
      resource_free_handle( asn_handle );
   }

   return read_sz;
}

int32_t tilemap_asn_load(
   const uint8_t* asn_buffer, int32_t asn_buffer_sz, struct TILEMAP* t
) {
   uint8_t tm_type = 0;
   uint16_t tm_version = 0;
   int32_t idx = 0;
   int32_t read_sz = 0,
      tm_sz = 0;

   memory_zero_ptr( (MEMORY_PTR)t, sizeof( struct TILEMAP ) );

   serial_asn_read_seq(
      asn_buffer, &tm_type, &tm_sz, "tilemap", idx, read_sz, cleanup )

   serial_asn_read_int(
      asn_buffer, &tm_version, 2, 0, "tilemap version", idx, read_sz, cleanup )

   serial_asn_read_int(
      asn_buffer, &(t->gid), sizeof( TILEMAP_GID ), 0,
      "tilemap GID", idx, read_sz, cleanup )

   serial_asn_read_string(
      asn_buffer, t->name, TILEMAP_NAME_MAX,
      "tilemap name", idx, read_sz, cleanup )

   serial_asn_read_int(
      asn_buffer, &(t->flags), 1, 0, "tilemap flags", idx, read_sz, cleanup )

   /* tileset */
   read_sz = tilemap_asn_parse_tileset( t, &(asn_buffer[idx]),
      asn_buffer_sz - idx );
   if( 0 >= read_sz ) {
      idx = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* tiles */
   read_sz = tilemap_asn_parse_tiles( t, &(asn_buffer[idx]),
      asn_buffer_sz - idx );
   if( 0 >= read_sz ) {
      idx = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* strings */
   read_sz = tilemap_asn_parse_strings( t, &(asn_buffer[idx]),
      asn_buffer_sz - idx );
   if( 0 >= read_sz ) {
      idx = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* spawns */
   read_sz = tilemap_asn_parse_spawns( t, &(asn_buffer[idx]),
      asn_buffer_sz - idx );
   if( 0 >= read_sz ) {
      idx = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* scripts */
   debug_printf( 1, "scripts sequence at %d", idx );
   read_sz = tilemap_asn_parse_scripts( t, &(asn_buffer[idx]),
      asn_buffer_sz - idx );
   if( 0 >= read_sz ) {
      idx = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* items */
   read_sz = tilemap_asn_parse_items( t, &(asn_buffer[idx]),
      asn_buffer_sz - idx );
   if( 0 >= read_sz ) {
      idx = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* crop defs */
   read_sz = tilemap_asn_parse_crop_defs( t, &(asn_buffer[idx]),
      asn_buffer_sz - idx );
   if( 0 >= read_sz ) {
      idx = read_sz;
      goto cleanup;
   }
   idx += read_sz;

cleanup:

   return idx;
}

int32_t tilemap_asn_save(
   MAUG_MHANDLE h_buffer, int32_t idx, struct TILEMAP* t
) {
   int16_t scripts_count = 0,
      i = 0,
      j = 0;
   int32_t mark_seq_main = 0,
      mark_seq_ts = 0,
      mark_seq_spawn = 0,
      mark_seq_spawn_coords = 0,
      mark_seq_scripts = 0,
      mark_seq_item = 0,
      sz_idx = 0,
      script_sz_idx = 0,
      step_sz_idx = 0;

   serial_asn_write_seq_start(
      &h_buffer, &mark_seq_main, "tilemap", idx, cleanup );
   
   serial_asn_write_int( &h_buffer, 1, x, "tilemap version", idx, cleanup );

   serial_asn_write_int( &h_buffer, t->gid, x, "tilemap GID", idx, cleanup );

   serial_asn_write_string(
      &h_buffer, t->name, TILEMAP_NAME_MAX, "tilemap name", idx, cleanup );

   serial_asn_write_int(
      &h_buffer, t->flags, x, "tilemap flags", idx, cleanup );

   /* tileset */
   serial_asn_write_seq_start(
      &h_buffer, &sz_idx, "tilemap tilesets", idx, cleanup )
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      if( 0 == memory_strnlen_ptr(
         t->tileset[i].image_name, sizeof( RESOURCE_NAME ) )
      ) {
         continue;
      }

      serial_asn_write_seq_start(
         &h_buffer, &mark_seq_ts, "tileset", idx, cleanup )
   
      serial_asn_write_string(
         &h_buffer, t->tileset[i].image_name, RESOURCE_NAME_MAX,
         "tileset image name", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->tileset[i].ascii, x, "tileset ASCII", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->tileset[i].flags, x, "tileset flags", idx, cleanup );

      serial_asn_write_seq_end(
         &h_buffer, &mark_seq_ts, "tileset", idx, cleanup )
   }
   serial_asn_write_seq_end(
      &h_buffer, &sz_idx, "tilemap tilesets", idx, cleanup )

   /* tiles */
   debug_printf( 2, "(offset 0x%02x) writing map tiles", idx );
   idx = asn_write_blob(
      &h_buffer, idx, t->tiles, ((TILEMAP_TH * TILEMAP_TW) / 2) );
   /* TODO: Error checking! */

   /* strings */
   idx = asn_write_blob( &h_buffer, idx,
      (unsigned char*)t->strpool, TILEMAP_STRPOOL_SZ );
   /* TODO: Error checking! */

   /* spawns */
   serial_asn_write_seq_start(
      &h_buffer, &sz_idx, "tilemap spawns", idx, cleanup )
   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      if( 0 == memory_strnlen_ptr(
         t->spawns[i].name, TILEMAP_SPAWN_NAME_SZ
      ) ) {
         continue;
      }

      serial_asn_write_seq_start(
         &h_buffer, &mark_seq_spawn, "spawn", idx, cleanup )

      serial_asn_write_string(
         &h_buffer, t->spawns[i].name, TILEMAP_SPAWN_NAME_SZ,
         "spawn name", idx, cleanup );

      /* TODO: Don't make this a seq! */
      serial_asn_write_seq_start(
         &h_buffer, &mark_seq_spawn_coords, "spawn coords", idx, cleanup )

      serial_asn_write_int(
         &h_buffer, t->spawns[i].coords.x, x, "spawn coords X", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->spawns[i].coords.y, x, "spawn coords Y", idx, cleanup );

      serial_asn_write_seq_end(
         &h_buffer, &mark_seq_spawn_coords, "spawn coords end", idx, cleanup )

      serial_asn_write_string(
         &h_buffer, t->spawns[i].sprite_name, RESOURCE_NAME_MAX,
         "spawn sprite name", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->spawns[i].ascii, x, "spawn ASCII", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->spawns[i].flags, x, "spawn flags", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->spawns[i].gid, x, "spawn GID", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->spawns[i].script_id, x,
         "spawn script ID", idx, cleanup );

      serial_asn_write_seq_end(
         &h_buffer, &mark_seq_spawn, "spawn", idx, cleanup )
   }
   serial_asn_write_seq_end(
      &h_buffer, &sz_idx, "tilemap spawns", idx, cleanup )

   /* scripts */
   /* TODO: Serialize ALL script slots to preserve indexes. */
   for(
      scripts_count = 0 ; TILEMAP_SCRIPTS_MAX > scripts_count ; scripts_count++
   ) {
      if( 0 == t->scripts[scripts_count].steps[0].action ) {
         break;
      }
   }

   serial_asn_write_seq_start(
      &h_buffer, &mark_seq_scripts, "tilemap scripts", idx, cleanup )
   for( i = 0 ; scripts_count > i ; i++ ) {
      serial_asn_write_seq_start(
         &h_buffer, &script_sz_idx, "scripts", idx, cleanup )

      /* steps */
      for( j = 0 ; SCRIPT_STEPS_MAX > j ; j++ ) {
         /* TODO: Serialize ALL steps. */
         if( 0 == t->scripts[i].steps[j].action ) {
            break;
         }

         serial_asn_write_seq_start(
            &h_buffer, &step_sz_idx, "script step", idx, cleanup )

         serial_asn_write_int(
            &h_buffer, t->scripts[i].steps[j].action, x,
            "script step action", idx, cleanup );

         serial_asn_write_int(
            &h_buffer, t->scripts[i].steps[j].arg, x,
            "script step arg", idx, cleanup );

         serial_asn_write_seq_end(
            &h_buffer, &step_sz_idx, "script step", idx, cleanup )
      }

      serial_asn_write_seq_end(
         &h_buffer, &script_sz_idx, "scripts", idx, cleanup )
   }
   serial_asn_write_seq_end(
      &h_buffer, &mark_seq_scripts, "tilemap scripts", idx, cleanup )

   serial_asn_write_seq_start(
      &h_buffer, &sz_idx, "tilemap item defs", idx, cleanup )
   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      /* TODO: Serialize ALL items, or copy smart active-item-only code to
       *       scripts above? */
      item_break_if_last( t->item_defs, i );
      if( ITEM_FLAG_ACTIVE != (t->item_defs[i].flags & ITEM_FLAG_ACTIVE) ) {
         continue;
      }

      serial_asn_write_seq_start(
         &h_buffer, &mark_seq_item, "item def", idx, cleanup )

      serial_asn_write_int( &h_buffer, i, x, "item def index", idx, cleanup );

      serial_asn_write_string(
         &h_buffer, t->item_defs[i].sprite_name, RESOURCE_NAME_MAX,
         "item def sprite name", idx, cleanup );

      serial_asn_write_string(
         &h_buffer, t->item_defs[i].name, RESOURCE_NAME_MAX,
         "item def name", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->item_defs[i].owner, x, "item def owner", idx, cleanup );
 
      serial_asn_write_int(
         &h_buffer, t->item_defs[i].gid, x, "item def GID", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->item_defs[i].data, x, "item def data", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->item_defs[i].flags, x, "item def flags", idx, cleanup );

      serial_asn_write_seq_end(
         &h_buffer, &mark_seq_item, "item def", idx, cleanup )
   }
   serial_asn_write_seq_end(
      &h_buffer, &sz_idx, "tilemap item defs", idx, cleanup )

   /* crop defs */
   serial_asn_write_seq_start(
      &h_buffer, &sz_idx, "tilemap crop defs", idx, cleanup )
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      if(
         CROP_DEF_FLAG_ACTIVE != (t->crop_defs[i].flags & CROP_DEF_FLAG_ACTIVE)
      ) {
         continue;
      }

      /* Reuse mark_seq_item for crop def since it's not used in this scope. */
      serial_asn_write_seq_start(
         &h_buffer, &mark_seq_item, "crop def", idx, cleanup )

      serial_asn_write_int( &h_buffer, i, x, "crop def index", idx, cleanup );

      serial_asn_write_string(
         &h_buffer, t->crop_defs[i].sprite_name, RESOURCE_NAME_MAX,
         "crop def sprite name", idx, cleanup );

      serial_asn_write_string(
         &h_buffer, t->crop_defs[i].name, CROP_NAME_MAX,
         "crop def name", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->crop_defs[i].gid, x, "crop def GID", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->crop_defs[i].flags, x, "crop def flags", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->crop_defs[i].cycle, x, "crop def cycle", idx, cleanup );

      serial_asn_write_int(
         &h_buffer, t->crop_defs[i].produce_gid, x,
         "crop def produce GID", idx, cleanup );

      serial_asn_write_seq_end(
         &h_buffer, &mark_seq_item, "crop def", idx, cleanup )
   }
   serial_asn_write_seq_end(
      &h_buffer, &sz_idx, "tilemap crop defs", idx, cleanup )

   /* End the main sequence. */
   serial_asn_write_seq_end(
      &h_buffer, &mark_seq_main, "tilemap", idx, cleanup )

cleanup:

   return idx;
}

