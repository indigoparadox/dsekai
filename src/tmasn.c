
#include "dsekai.h"

#ifndef RESOURCE_FILE
#error Loading maps from ASN requires file resources!
#endif /* !RESOURCE_FILE */

#include "tmasn.h"

static int16_t tilemap_asn_parse_tileset(
   struct TILEMAP* t, const uint8_t* asn_buffer
) {
   int32_t ts_seq_sz = 0;
   uint8_t type_buf = 0;
   int16_t total_read_sz = 0,
      read_sz = 0,
      tile_idx = 0;

   total_read_sz = asn_read_meta_ptr(
      asn_buffer, total_read_sz, &type_buf, &ts_seq_sz );
   if( ASN_SEQUENCE != type_buf ) {
      error_printf( "invalid tileset sequence type: 0x%02x", type_buf );
      goto cleanup;
   }
   debug_printf( 2, "tileset sequence size: %d bytes", ts_seq_sz );

   while( total_read_sz - 4 < ts_seq_sz ) {
      if( 0x30 != asn_buffer[total_read_sz] ) {
         error_printf(
            "invalid tiledef sequence type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      total_read_sz += 2; /* tile sequence type, size */
      
      /* image */
      read_sz = asn_read_string( t->tileset[tile_idx].image,
         RESOURCE_PATH_MAX, asn_buffer, total_read_sz );
      if( 0 == read_sz ) {
         goto cleanup;
      }
      debug_printf( 2, "tile resource: %s (%d)",
         t->tileset[tile_idx].image, read_sz );
      t->tileset[tile_idx].image_id = -1;
      total_read_sz += read_sz; /* tile image and header */

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

static int16_t tilemap_asn_parse_tiles(
   struct TILEMAP* t, const uint8_t* asn_buffer
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

static int16_t tilemap_asn_parse_strings(
   struct TILEMAP* t, const uint8_t* asn_buffer
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

static int16_t tilemap_asn_parse_spawns(
   struct TILEMAP* t, const uint8_t* asn_buffer
) {
   uint8_t type_buf = 0;
   int32_t total_read_sz = 0,
      read_sz = 0,
      all_spawns_seq_start = 0,
      all_spawns_seq_sz = 0,
      spawn_def_seq_sz = 0,
      coords_seq_sz = 0,
      spawn_idx = 0;

   total_read_sz = asn_read_meta_ptr(
      asn_buffer, total_read_sz, &type_buf, &all_spawns_seq_sz );
   if( ASN_SEQUENCE != type_buf ) {
      error_printf(
         "invalid spawn sequence type byte: 0x%02x", type_buf );
      goto cleanup;
   }
   debug_printf( 1, "spawn seq is %d bytes...", all_spawns_seq_sz );

   all_spawns_seq_start = total_read_sz;

   while( total_read_sz - all_spawns_seq_start < all_spawns_seq_sz ) {
      total_read_sz = asn_read_meta_ptr(
         asn_buffer, total_read_sz, &type_buf, &spawn_def_seq_sz );
      if( ASN_SEQUENCE != type_buf ) {
         error_printf(
            "invalid spawndef sequence type byte: 0x%02x", type_buf );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      
      /* image */
      read_sz =
         asn_read_string( t->spawns[spawn_idx].name,
            TILEMAP_SPAWN_NAME_SZ, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "spawn name: %s (%d)",
         t->spawns[spawn_idx].name, read_sz );
      total_read_sz += read_sz; /* spawn name and header */

      /* coords header */
      total_read_sz = asn_read_meta_ptr(
         asn_buffer, total_read_sz, &type_buf, &coords_seq_sz );
      if( ASN_SEQUENCE != type_buf ) {
         error_printf(
            "invalid spawn coords sequence type byte: 0x%02x", type_buf );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }

      /* coords.x */
      read_sz = asn_read_int(
         &(t->spawns[spawn_idx].coords.x), 1, 0, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading spawn X coord" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "spawn X: %d", t->spawns[spawn_idx].coords.x );
      total_read_sz += read_sz;

      /* coords.y */
      read_sz = asn_read_int(
         &(t->spawns[spawn_idx].coords.y), 1, 0, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading spawn Y coord" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "spawn Y: %d", t->spawns[spawn_idx].coords.y );
      total_read_sz += read_sz;

      /* type */
      read_sz = asn_read_string( t->spawns[spawn_idx].type,
            RESOURCE_PATH_MAX, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading spawn type" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "spawn type: %s (%d)",
         t->spawns[spawn_idx].type, read_sz );
      total_read_sz += read_sz; /* spawn name and header */

      /* flags */
      read_sz = asn_read_int(
         (uint8_t*)&(t->spawns[spawn_idx].flags), 2, 0,
         asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         error_printf( "error reading spawn flags" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "spawn flags: %04x", t->spawns[spawn_idx].flags );
      total_read_sz += read_sz;

      /* script_id */
      total_read_sz += 2; /* script_id header */
      t->spawns[spawn_idx].script_id = asn_buffer[total_read_sz];
      if( 0xff == t->spawns[spawn_idx].script_id ) {
         debug_printf( 1, "script ID is negative" );
         t->spawns[spawn_idx].script_id = -1;
      }
      debug_printf( 1, "script ID: %d", t->spawns[spawn_idx].script_id );
      total_read_sz++;

      spawn_idx++;
   }

cleanup:

   return total_read_sz;
}

static int16_t tilemap_asn_parse_items(
   struct TILEMAP* t, const uint8_t* asn_buffer
) {
   uint8_t type_buf = 0;
   int32_t total_read_sz = 0,
      read_sz = 0,
      all_items_seq_start = 0,
      all_items_seq_sz = 0,
      item_def_seq_sz = 0;
   int16_t item_idx = 0;

   total_read_sz = asn_read_meta_ptr(
      asn_buffer, total_read_sz, &type_buf, &all_items_seq_sz );
   if( ASN_SEQUENCE != type_buf ) {
      error_printf(
         "invalid item sequence type byte: 0x%02x", type_buf );
      goto cleanup;
   }
   debug_printf( 1, "item seq is %d bytes...", all_items_seq_sz );

   all_items_seq_start = total_read_sz;

   while( total_read_sz - all_items_seq_start < all_items_seq_sz ) {
      total_read_sz = asn_read_meta_ptr(
         asn_buffer, total_read_sz, &type_buf, &item_def_seq_sz );
      if( ASN_SEQUENCE != type_buf ) {
         error_printf(
            "invalid item def sequence type byte: 0x%02x", type_buf );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      
      /* index */
      read_sz = asn_read_int(
         (uint8_t*)&item_idx, 2, 0, asn_buffer, total_read_sz );
      total_read_sz += read_sz;

      /* sprite */
      read_sz = asn_read_string( t->item_defs[item_idx].sprite,
         RESOURCE_PATH_MAX, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d sprite: %s (%d)",
         item_idx, t->item_defs[item_idx].sprite, read_sz );
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

static int16_t tilemap_asn_parse_crop_defs(
   struct TILEMAP* t, const uint8_t* asn_buffer
) {
   uint8_t type_buf = 0;
   int32_t total_read_sz = 0,
      read_sz = 0,
      all_crops_seq_start = 0,
      all_crops_seq_sz = 0,
      crop_def_seq_sz = 0;
   int16_t crop_idx = 0;

   total_read_sz = asn_read_meta_ptr(
      asn_buffer, total_read_sz, &type_buf, &all_crops_seq_sz );
   if( ASN_SEQUENCE != type_buf ) {
      error_printf(
         "invalid crop def sequence type byte: 0x%02x", type_buf );
      goto cleanup;
   }
   debug_printf( 1, "crop def seq is %d bytes...", all_crops_seq_sz );

   all_crops_seq_start = total_read_sz;

   while( total_read_sz - all_crops_seq_start < all_crops_seq_sz ) {
      total_read_sz = asn_read_meta_ptr(
         asn_buffer, total_read_sz, &type_buf, &crop_def_seq_sz );
      if( ASN_SEQUENCE != type_buf ) {
         error_printf(
            "invalid crop def sequence type byte: 0x%02x", type_buf );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      
      /* index */
      read_sz = asn_read_int(
         (uint8_t*)&crop_idx, 2, 0, asn_buffer, total_read_sz );
      total_read_sz += read_sz;

      /* sprite */
      read_sz = asn_read_string( t->crop_defs[crop_idx].sprite,
         RESOURCE_PATH_MAX, asn_buffer, total_read_sz );
      if( 0 >= read_sz ) {
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d sprite: %s (%d)",
         crop_idx, t->crop_defs[crop_idx].sprite, read_sz );
      total_read_sz += read_sz; /* crop def sprite and header */
      t->crop_defs[crop_idx].sprite_id = -1;

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

static int32_t tilemap_asn_parse_scripts(
   struct TILEMAP* t, const uint8_t* asn_buffer
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

   /* Read scripts sequence header. */
   total_read_sz = asn_read_meta_ptr(
      asn_buffer, total_read_sz, &type_buf, &all_scripts_seq_sz );
   if( ASN_SEQUENCE != type_buf ) {
      error_printf(
         "invalid all scripts sequence type byte: 0x%02x", type_buf );
      goto cleanup;
   }
   debug_printf(
      1, "all scripts sequence is %d bytes long...", all_scripts_seq_sz );

   while( total_read_sz < all_scripts_seq_sz ) {
      debug_printf( 1, "script idx: %d", script_idx );

      step_idx = 0;
      script_seq_start = total_read_sz;

      /* Read a script's step sequence header. */
      total_read_sz = asn_read_meta_ptr(
         asn_buffer, total_read_sz, &type_buf, &script_seq_sz );
      if( ASN_SEQUENCE != type_buf ) {
         error_printf( "invalid script sequence type byte: 0x%02x", type_buf );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }

      debug_printf( 1, "script seq is %d bytes...", script_seq_sz );

      while( total_read_sz - script_seq_start < script_seq_sz ) {

         total_read_sz = asn_read_meta_ptr(
            asn_buffer, total_read_sz, &type_buf, &step_seq_sz );
         if( ASN_SEQUENCE != type_buf ) {
            error_printf( "invalid step sequence type byte: 0x%02x", type_buf );
            total_read_sz = TILEMAP_ASN_ERROR_READ;
            goto cleanup;
         }

         debug_printf( 1, "step seq is %d bytes...", step_seq_sz );

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

int16_t tilemap_asn_load( RESOURCE_ID id, struct TILEMAP* t ) {
   const uint8_t* asn_buffer = NULL;
   uint8_t tm_type = 0;
   uint16_t tm_version = 0;
   int16_t retval = 1,
      idx = 0;
   int32_t read_sz = 0,
      tm_sz = 0;
   RESOURCE_HANDLE asn_handle = (RESOURCE_HANDLE)0;

   memory_zero_ptr( (MEMORY_PTR)t, sizeof( struct TILEMAP ) );

   /* Allocate buffers for parsing ASN.1. */
   asn_handle = resource_get_handle( id );
   if( (RESOURCE_HANDLE)0 == asn_handle ) {
      error_printf( "could not get tilemap resource handle" );
      retval = 0;
      goto cleanup;
   }

   asn_buffer = resource_lock_handle( asn_handle );
   if( NULL == asn_buffer ) {
      error_printf( "could not get tilemap resource pointer" );
      retval = 0;
      goto cleanup;
   }

   read_sz = asn_read_meta_ptr( asn_buffer, idx, &tm_type, &tm_sz );
   idx += read_sz;
   if( 0x30 == tm_type ) {
      debug_printf( 3, "tilemap sequence found in resource" );
   } else {
      error_printf( "no tilemap sequence found! (found %d instead)",
         tm_type );
      goto cleanup;
   }

   /* version */
   read_sz = asn_read_int( (uint8_t*)&tm_version, 2, 0, asn_buffer, idx );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   debug_printf( 3, "tilemap version %d", tm_version );
   idx += read_sz;

   /* gid */
   read_sz = asn_read_int( (uint8_t*)&(t->gid), 2, 0, asn_buffer, idx );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   debug_printf( 3, "tilemap gid %d", t->gid );
   idx += read_sz;

   /* name */
   read_sz =
      asn_read_string( t->name, TILEMAP_NAME_MAX, asn_buffer, idx );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   debug_printf( 3, "tilemap name: %s (%d)", t->name, read_sz );
   idx += read_sz;

   /* flags */
   read_sz = asn_read_int( &t->flags, 1, 0, asn_buffer, idx );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* tileset */
   read_sz = tilemap_asn_parse_tileset( t, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* tiles */
   read_sz = tilemap_asn_parse_tiles( t, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* strings */
   read_sz = tilemap_asn_parse_strings( t, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* spawns */
   read_sz = tilemap_asn_parse_spawns( t, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* scripts */
   read_sz = tilemap_asn_parse_scripts( t, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* items */
   read_sz = tilemap_asn_parse_items( t, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;

   /* crop defs */
   read_sz = tilemap_asn_parse_crop_defs( t, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;

cleanup:

   if( NULL != asn_buffer ) {
      resource_unlock_handle( asn_handle );
   }

   if( (RESOURCE_HANDLE)NULL != asn_handle ) {
      resource_free_handle( asn_handle );
   }

   return retval;
}

int32_t tilemap_asn_save(
   MEMORY_HANDLE h_buffer, int32_t idx, struct TILEMAP* t
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

   idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_main );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }
   
   /* version */
   debug_printf( 3, "(offset 0x%02x) writing map version", idx );
   idx = asn_write_int( &h_buffer, idx, 1 );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

   /* version */
   debug_printf( 3, "(offset 0x%02x) writing map gid", idx );
   idx = asn_write_int( &h_buffer, idx, t->gid );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

   /* name */
   debug_printf( 3, "(offset 0x%02x) writing map name", idx );
   idx = asn_write_string( &h_buffer, idx, t->name, TILEMAP_NAME_MAX );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

#if 0
   /* engine_type */
   debug_printf( 3, "(offset 0x%02x) writing map engine type", idx );
   idx = asn_write_int( &h_buffer, idx, t->engine_type );
   assert( 0 <= idx );
#endif

   /* flags */
   debug_printf( 3, "(offset 0x%02x) writing map flags", idx );
   idx = asn_write_int( &h_buffer, idx, t->flags );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

   /* tileset */
   debug_printf( 3, "(offset 0x%02x) writing map tilesets", idx );
   idx = asn_write_seq_start( &h_buffer, idx, &sz_idx );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      if( 0 == memory_strnlen_ptr(
         t->tileset[i].image, sizeof( RESOURCE_ID ) )
      ) {
         continue;
      }

      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_ts );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }
   
      /* image */
      debug_printf( 3, "(offset 0x%02x) writing map tileset image", idx );
      idx = asn_write_string(
         &h_buffer, idx, t->tileset[i].image, RESOURCE_PATH_MAX );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* flags */
      debug_printf( 3, "(offset 0x%02x) writing map tileset flags", idx );
      idx = asn_write_int( &h_buffer, idx, t->tileset[i].flags );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_ts );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }
   }
   idx = asn_write_seq_end( &h_buffer, idx, &sz_idx );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

   /* tiles */
   debug_printf( 3, "(offset 0x%02x) writing map tiles", idx );
   idx = asn_write_blob(
      &h_buffer, idx, t->tiles, ((TILEMAP_TH * TILEMAP_TW) / 2) );

   /* strings */
   idx = asn_write_blob( &h_buffer, idx,
      (unsigned char*)t->strpool, TILEMAP_STRPOOL_SZ );
#if 0
   for( i = 0 ; TILEMAP_STRINGS_MAX > i ; i++ ) {
      if( 0 == strlen( t->strings[i] ) ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map string", idx );
      idx = asn_write_string( &h_buffer, idx,
         t->strings[i], DIALOG_TEXT_SZ );
      assert( 0 <= idx );
   }
#endif

   /* spawns */
   debug_printf( 3, "(offset 0x%02x) writing map spawns", idx );
   idx = asn_write_seq_start( &h_buffer, idx, &sz_idx );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }
   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      if( 0 == memory_strnlen_ptr(
         t->spawns[i].name, TILEMAP_SPAWN_NAME_SZ
      ) ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map spawn", idx );
      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_spawn );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* name */
      debug_printf( 3, "(offset 0x%02x) writing map spawn name", idx );
      idx = asn_write_string(
         &h_buffer, idx, t->spawns[i].name, TILEMAP_SPAWN_NAME_SZ );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* coords */
      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_spawn_coords );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* coords.x */
      debug_printf( 3, "(offset 0x%02x) writing map spawn coords x", idx );
      idx = asn_write_int( &h_buffer, idx, t->spawns[i].coords.x );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* coords.y */
      debug_printf( 3, "(offset 0x%02x) writing map spawn coords y", idx );
      idx = asn_write_int( &h_buffer, idx, t->spawns[i].coords.y );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_spawn_coords );

      /* type */
      debug_printf( 3, "(offset 0x%02x) writing map spawn type", idx );
      idx = asn_write_string(
         &h_buffer, idx, t->spawns[i].type, RESOURCE_PATH_MAX );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* flags */
      debug_printf( 3, "(offset 0x%02x) writing map spawn flags", idx );
      idx = asn_write_int( &h_buffer, idx, t->spawns[i].flags );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* script_id */
      debug_printf( 3, "(offset 0x%02x) writing map spawn script ID", idx );
      idx = asn_write_int( &h_buffer, idx, t->spawns[i].script_id );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_spawn );
   }
   idx = asn_write_seq_end( &h_buffer, idx, &sz_idx );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

   /* scripts */
   for(
      scripts_count = 0 ; TILEMAP_SCRIPTS_MAX > scripts_count ; scripts_count++
   ) {
      if( 0 == t->scripts[scripts_count].steps[0].action ) {
         break;
      }
   }

   idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_scripts );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }
   for( i = 0 ; scripts_count > i ; i++ ) {
      idx = asn_write_seq_start( &h_buffer, idx, &script_sz_idx );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* steps */
      for( j = 0 ; SCRIPT_STEPS_MAX > j ; j++ ) {
         if( 0 == t->scripts[i].steps[j].action ) {
            break;
         }

         idx = asn_write_seq_start( &h_buffer, idx, &step_sz_idx );
         if( 0 > idx ) {
            error_printf( "error" );
            idx = -1;
            goto cleanup;
         }

         /* action */
         debug_printf(
            3, "(offset 0x%02x) writing map spawn script step action", idx );
         idx = asn_write_int( &h_buffer, idx, t->scripts[i].steps[j].action );
         if( 0 > idx ) {
            error_printf( "error" );
            idx = -1;
            goto cleanup;
         }

         /* arg */
         debug_printf(
            3, "(offset 0x%02x) writing map spawn script step arg", idx );
         idx = asn_write_int( &h_buffer, idx, t->scripts[i].steps[j].arg );
         if( 0 > idx ) {
            error_printf( "error" );
            idx = -1;
            goto cleanup;
         }

         idx = asn_write_seq_end( &h_buffer, idx, &step_sz_idx );
         if( 0 > idx ) {
            error_printf( "error" );
            idx = -1;
            goto cleanup;
         }
      }
      idx = asn_write_seq_end( &h_buffer, idx, &script_sz_idx );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }
   }
   idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_scripts );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

   /* items */
   debug_printf( 3, "(offset 0x%02x) writing map items", idx );
   idx = asn_write_seq_start( &h_buffer, idx, &sz_idx );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }
   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      if( ITEM_FLAG_ACTIVE != (t->item_defs[i].flags & ITEM_FLAG_ACTIVE) ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map item", idx );
      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_item );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* index */
      debug_printf( 3, "(offset 0x%02x) writing item index", idx );
      idx = asn_write_int( &h_buffer, idx, i );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* sprite */
      debug_printf( 3, "(offset 0x%02x) writing item sprite path", idx );
      idx = asn_write_string(
         &h_buffer, idx, t->item_defs[i].sprite, RESOURCE_PATH_MAX );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* name */
      debug_printf( 3, "(offset 0x%02x) writing item name", idx );
      idx = asn_write_string( &h_buffer, idx, t->item_defs[i].name, ITEM_NAME_SZ );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* owner */
      debug_printf( 3, "(offset 0x%02x) writing item owner", idx );
      idx = asn_write_int( &h_buffer, idx, t->item_defs[i].owner );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }
 
      /* gid */
      debug_printf( 3, "(offset 0x%02x) writing item gid", idx );
      idx = asn_write_int( &h_buffer, idx, t->item_defs[i].gid );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* data */
      debug_printf( 3, "(offset 0x%02x) writing item data", idx );
      idx = asn_write_int( &h_buffer, idx, t->item_defs[i].data );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* flags */
      debug_printf( 3, "(offset 0x%02x) writing item flags", idx );
      idx = asn_write_int( &h_buffer, idx, t->item_defs[i].flags );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_item );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }
   }
   idx = asn_write_seq_end( &h_buffer, idx, &sz_idx );

   /* crop defs */
   debug_printf( 3, "(offset 0x%02x) writing map crop defs", idx );
   idx = asn_write_seq_start( &h_buffer, idx, &sz_idx );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      if(
         CROP_DEF_FLAG_ACTIVE != (t->crop_defs[i].flags & CROP_DEF_FLAG_ACTIVE)
      ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map crop def", idx );
      /* Reuse mark_seq_item for crop def since it's not used in this scope. */
      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_item );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* index */
      debug_printf( 3, "(offset 0x%02x) writing crop def index", idx );
      idx = asn_write_int( &h_buffer, idx, i );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* sprite */
      debug_printf( 3, "(offset 0x%02x) writing crop def sprite path", idx );
      idx = asn_write_string(
         &h_buffer, idx, t->crop_defs[i].sprite, RESOURCE_PATH_MAX );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* name */
      debug_printf( 3, "(offset 0x%02x) writing crop def name", idx );
      idx = asn_write_string(
         &h_buffer, idx, t->crop_defs[i].name, CROP_NAME_MAX );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* gid */
      debug_printf( 3, "(offset 0x%02x) writing crop def gid", idx );
      idx = asn_write_int( &h_buffer, idx, t->crop_defs[i].gid );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* flags */
      debug_printf( 3, "(offset 0x%02x) writing crop def flags", idx );
      idx = asn_write_int( &h_buffer, idx, t->crop_defs[i].flags );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* cycle */
      debug_printf( 3, "(offset 0x%02x) writing crop def cycle", idx );
      idx = asn_write_int( &h_buffer, idx, t->crop_defs[i].cycle );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      /* produce gid */
      debug_printf( 3, "(offset 0x%02x) writing crop produce gid", idx );
      idx = asn_write_int( &h_buffer, idx, t->crop_defs[i].produce_gid );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_item );
      if( 0 > idx ) {
         error_printf( "error" );
         idx = -1;
         goto cleanup;
      }
   }
   idx = asn_write_seq_end( &h_buffer, idx, &sz_idx );

   /* End the main sequence. */
   idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_main );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

cleanup:

   return idx;
}

