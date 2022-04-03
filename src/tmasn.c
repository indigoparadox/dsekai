
#include "dsekai.h"

#ifndef RESOURCE_FILE
#error Loading maps from ASN requires file resources!
#endif /* !RESOURCE_FILE */

int16_t tilemap_asn_read_short( const uint8_t* asn_buffer ) {
   int16_t n_out = 0;
   n_out |= (asn_buffer[0] << 8);
   n_out |= asn_buffer[1];
   return n_out;
}

int16_t tilemap_asn_parse_int(
   uint8_t* buffer, uint8_t buffer_sz, uint8_t sign, const uint8_t* asn_buffer
) {
   int16_t field_sz = 0;
   int16_t* int16_buffer = (int16_t*)buffer;
   uint16_t* uint16_buffer = (uint16_t*)buffer;
   int8_t* int8_buffer = (int8_t*)buffer;

   if( 0x02 != asn_buffer[0] ) {
      error_printf( "invalid integer type byte: 0x%02x", asn_buffer[0] );
      goto cleanup;
   }

   if( 2 < buffer_sz ) {
      error_printf( "integer buffer too large" );
      goto cleanup;
   }

   if( asn_buffer[1] == 1 ) {
      if( sign ) {
         *int8_buffer = asn_buffer[2];
      } else {
         *buffer = asn_buffer[2];
      }
      field_sz += 1;
   } else if( asn_buffer[1] == 2 ) {
      if( sign ) {
         *int16_buffer = tilemap_asn_read_short( &(asn_buffer[2]) );
      } else {
         *uint16_buffer = tilemap_asn_read_short( &(asn_buffer[2]) );
      }
      field_sz += 2;
   } else {
      error_printf( "unable to process integer: size 0x%02x",
         asn_buffer[1] );
      goto cleanup;
   }

   field_sz += 2; /* type and size bytes */

cleanup:

   return field_sz;
}

static int16_t tilemap_asn_parse_string(
   char* buffer, int16_t buffer_sz, const uint8_t* asn_buffer
) {
   int16_t field_sz = 0;

   if( 0x16 != asn_buffer[0] ) {
      error_printf( "invalid string type byte: 0x%02x", asn_buffer[0] );
      goto cleanup;
   }
   
   field_sz = asn_buffer[1];
   if( buffer_sz <= field_sz ) {
      error_printf( "string too long to fit in buffer: %d", field_sz );
      goto cleanup;
   }

   memory_copy_ptr( (MEMORY_PTR)buffer, (const MEMORY_PTR)&(asn_buffer[2]),
      field_sz );
   debug_printf( 1, "parsed string: %s (%d)", buffer, field_sz );

   field_sz += 2; /* type and length bytes */

cleanup:

   return field_sz;
}

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
      read_sz = tilemap_asn_parse_string( t->tileset[tile_idx].image,
         RESOURCE_PATH_MAX, &(asn_buffer[total_read_sz]) );
      if( 0 == read_sz ) {
         goto cleanup;
      }
      debug_printf( 3, "tile resource: %s (%d)",
         t->tileset[tile_idx].image, read_sz );
      total_read_sz += read_sz; /* tile image and header */

      /* flags */
      read_sz = tilemap_asn_parse_int(
         &(t->tileset[tile_idx].flags), 1, 0, &(asn_buffer[total_read_sz]) );
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
   t_seq_sz = tilemap_asn_read_short( &(asn_buffer[2]) );
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

#if 0
   while( 0x16 == asn_buffer[total_read_sz] ) {
      read_sz = tilemap_asn_parse_string(
         t->strings[str_idx], DIALOG_TEXT_SZ,
         &(asn_buffer[total_read_sz]) );
      str_idx++;
      total_read_sz += read_sz;
      if( 0 == read_sz ) {
         error_printf( "read %d strings", str_idx );
         break;
      }
      debug_printf( 2, "found string: %s", t->strings[str_idx - 1] );
   }
#endif

   if( 0x04 != asn_buffer[0] ) {
      error_printf(
         "invalid tile blob type byte: 0x%02x", asn_buffer[0] );
      goto cleanup;
   }
   strpool_sz = tilemap_asn_read_short( &(asn_buffer[2]) );
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
         tilemap_asn_parse_string( t->spawns[spawn_idx].name,
            TILEMAP_SPAWN_NAME_SZ, &(asn_buffer[total_read_sz]) );
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
      read_sz = tilemap_asn_parse_int(
         &(t->spawns[spawn_idx].coords.x), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading spawn X coord" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "spawn X: %d", t->spawns[spawn_idx].coords.x );
      total_read_sz += read_sz;

      /* coords.y */
      read_sz = tilemap_asn_parse_int(
         &(t->spawns[spawn_idx].coords.y), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading spawn Y coord" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "spawn Y: %d", t->spawns[spawn_idx].coords.y );
      total_read_sz += read_sz;

      /* type */
      read_sz = tilemap_asn_parse_string( t->spawns[spawn_idx].type,
            RESOURCE_PATH_MAX, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading spawn type" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "spawn type: %s (%d)",
         t->spawns[spawn_idx].type, read_sz );
      total_read_sz += read_sz; /* spawn name and header */

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
      read_sz = tilemap_asn_parse_int(
         (uint8_t*)&item_idx, 2, 0, &(asn_buffer[total_read_sz]) );
      total_read_sz += read_sz;

      /* sprite */
      read_sz = tilemap_asn_parse_string( t->items[item_idx].sprite,
         RESOURCE_PATH_MAX, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d sprite: %s (%d)",
         item_idx, t->items[item_idx].sprite, read_sz );
      total_read_sz += read_sz; /* item sprite and header */

      /* name */
      read_sz = tilemap_asn_parse_string( t->items[item_idx].name,
            ITEM_NAME_SZ, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading item name" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d name: %s (%d)",
         item_idx, t->items[item_idx].name, read_sz );
      total_read_sz += read_sz; /* item name and header */

      /* type */
      read_sz = tilemap_asn_parse_int(
         &(t->items[item_idx].type), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading item type" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d type: %d (%d)",
         item_idx, t->items[item_idx].type, read_sz );
      total_read_sz += read_sz;

      /* owner */
      read_sz = tilemap_asn_parse_int(
         (uint8_t*)&(t->items[item_idx].owner), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading item owner" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d owner: %d (%d)",
         item_idx, t->items[item_idx].owner, read_sz );
      total_read_sz += read_sz;

      /* gid */
      read_sz = tilemap_asn_parse_int(
         (uint8_t*)&(t->items[item_idx].gid), 2, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading item gid" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d gid: %d (%d)",
         item_idx, t->items[item_idx].gid, read_sz );
      total_read_sz += read_sz;

      /* data */
      read_sz = tilemap_asn_parse_int(
         &(t->items[item_idx].data), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading item data" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d data: %d (%d)",
         item_idx, t->items[item_idx].data, read_sz );
      total_read_sz += read_sz;

      /* count */
      read_sz = tilemap_asn_parse_int(
         &(t->items[item_idx].count), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading item count" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d count: %d (%d)",
         item_idx, t->items[item_idx].count, read_sz );
      total_read_sz += read_sz;

      /* flags */
      read_sz = tilemap_asn_parse_int(
         &(t->items[item_idx].flags), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading item flags" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "item %d flags: %d (%d)",
         item_idx, t->items[item_idx].flags, read_sz );
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
      read_sz = tilemap_asn_parse_int(
         (uint8_t*)&crop_idx, 2, 0, &(asn_buffer[total_read_sz]) );
      total_read_sz += read_sz;

      /* sprite */
      read_sz = tilemap_asn_parse_string( t->crop_defs[crop_idx].sprite,
         RESOURCE_PATH_MAX, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d sprite: %s (%d)",
         crop_idx, t->crop_defs[crop_idx].sprite, read_sz );
      total_read_sz += read_sz; /* crop def sprite and header */

      /* name */
      read_sz = tilemap_asn_parse_string( t->crop_defs[crop_idx].name,
            CROP_NAME_MAX, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def name" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop %d name: %s (%d)",
         crop_idx, t->crop_defs[crop_idx].name, read_sz );
      total_read_sz += read_sz; /* crop def name and header */

      /* gid */
      read_sz = tilemap_asn_parse_int(
         &(t->crop_defs[crop_idx].gid), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def gid" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d gid: %d (%d)",
         crop_idx, t->crop_defs[crop_idx].gid, read_sz );
      total_read_sz += read_sz;

      /* flags */
      read_sz = tilemap_asn_parse_int(
         &(t->crop_defs[crop_idx].flags), 1, 0, &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def flags" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d flags: %d (%d)",
         crop_idx, t->crop_defs[crop_idx].flags, read_sz );
      total_read_sz += read_sz;

      /* cycle */
      read_sz = tilemap_asn_parse_int(
         (uint8_t*)&(t->crop_defs[crop_idx].cycle), 4, 0,
         &(asn_buffer[total_read_sz]) );
      if( 0 >= read_sz ) {
         error_printf( "error reading crop def cycle" );
         total_read_sz = TILEMAP_ASN_ERROR_READ;
         goto cleanup;
      }
      debug_printf( 2, "crop def %d cycle: %d (%d)",
         crop_idx, t->crop_defs[crop_idx].cycle, read_sz );
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
         read_sz = tilemap_asn_parse_int(
            (uint8_t*)&(t->scripts[script_idx].steps[step_idx].action),
            2, 0, &(asn_buffer[total_read_sz]) );
         if( 0 == read_sz ) {
            error_printf( "unable to read action" );
            total_read_sz = TILEMAP_ASN_ERROR_READ;
            goto cleanup;
         }
         total_read_sz += read_sz;

         /* step.arg */
         read_sz = tilemap_asn_parse_int(
            (uint8_t*)&(t->scripts[script_idx].steps[step_idx].arg),
            2, 1, &(asn_buffer[total_read_sz]) );
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
   int16_t retval = 1,
      idx = 0;
   int32_t read_sz = 0;
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

   if( 0x30 == asn_buffer[0] ) {
      debug_printf( 3, "tilemap sequence found in resource" );
   } else {
      error_printf( "no tilemap sequence found! (found %d instead)",
         asn_buffer[0] );
      goto cleanup;
   }
   idx++;

   idx += 3; /* Skip size bytes. */

   idx += 2; /* Skip version tag. */
   debug_printf( 3, "tilemap version %d", asn_buffer[idx++] );

   /* name */
   read_sz =
      tilemap_asn_parse_string( t->name, TILEMAP_NAME_MAX, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   debug_printf( 3, "tilemap name: %s (%d)", t->name, read_sz );
   idx += read_sz;

#if 0
   /* engine_type */
   read_sz = tilemap_asn_parse_int( &t->engine_type, 1, 0, &(asn_buffer[idx]) );
   if( 0 >= read_sz ) {
      retval = read_sz;
      goto cleanup;
   }
   idx += read_sz;
#endif

   /* flags */
   read_sz = tilemap_asn_parse_int( &t->flags, 1, 0, &(asn_buffer[idx]) );
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

