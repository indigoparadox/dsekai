
#include "dsekai.h"

int16_t tilemap_asn_parse_short( const uint8_t* asn_buffer ) {
   int16_t n_out = 0;
   n_out |= (asn_buffer[0] << 8);
   n_out |= asn_buffer[1];
   return n_out;
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

   memory_copy_ptr( buffer, &(asn_buffer[2]), field_sz );
   debug_printf( 1, "parsed string: %s (%d)", buffer, field_sz );

   field_sz += 2; /* type and length bytes */

cleanup:

   return field_sz;
}

static int16_t tilemap_asn_parse_tileset(
   struct TILEMAP* t, const uint8_t* asn_buffer
) {
   int16_t total_read_sz = 0,
      read_sz = 0,
      ts_seq_sz = 0,
      tile_idx = 0;

   if( 0x30 != asn_buffer[0] || 0x82 != asn_buffer[1] ) {
      error_printf(
         "invalid tileset sequence header bytes: 0x%02x 0x%02x",
            asn_buffer[0], asn_buffer[1] );
      goto cleanup;
   }
   ts_seq_sz = tilemap_asn_parse_short( &(asn_buffer[2]) );
   debug_printf( 2, "tileset sequence size: %d bytes", ts_seq_sz );

   total_read_sz += 4; /* sequence type, length fields */

   while( total_read_sz - 4 < ts_seq_sz ) {
      if( 0x30 != asn_buffer[total_read_sz] ) {
         error_printf(
            "invalid tiledef sequence type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      total_read_sz += 2; /* tile sequence type, size */
      
      /* image */
      read_sz =
         tilemap_asn_parse_string( t->tileset[tile_idx].image,
            RESOURCE_PATH_MAX, &(asn_buffer[total_read_sz]) );
      if( 0 == read_sz ) {
         goto cleanup;
      }
      debug_printf( 3, "tile resource: %s (%d)",
         t->tileset[tile_idx].image, read_sz );
      total_read_sz += read_sz; /* tile image and header */

      /* flags */
      if( 0x02 != asn_buffer[total_read_sz] ) {
         error_printf( "invalid int type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      total_read_sz += 2; /* tile flags header */
      t->tileset[tile_idx].flags = asn_buffer[total_read_sz];
      total_read_sz++;
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
   t_seq_sz = tilemap_asn_parse_short( &(asn_buffer[2]) );
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
      read_sz = 0,
      str_idx = 0;

   debug_printf( 2, "parsing strings..." );
   while( 0x16 == asn_buffer[total_read_sz] ) {
      read_sz = tilemap_asn_parse_string(
         t->strings[str_idx], TILEMAP_STRINGS_SZ,
         &(asn_buffer[total_read_sz]) );
      str_idx++;
      total_read_sz += read_sz;
      if( 0 == read_sz ) {
         error_printf( "read %d strings", str_idx );
         break;
      }
      debug_printf( 2, "found string: %s", t->strings[str_idx - 1] );
   }

   t->strings_count = str_idx;

   return total_read_sz;
}

static int16_t tilemap_asn_parse_spawns(
   struct TILEMAP* t, const uint8_t* asn_buffer
) {
   int16_t total_read_sz = 0,
      read_sz = 0,
      ts_seq_sz = 0,
      spawn_idx = 0;

   if( 0x30 != asn_buffer[0] ) {
      error_printf(
         "invalid spawn sequence type byte: 0x%02x", asn_buffer[0] );
      goto cleanup;
   }
   ts_seq_sz = tilemap_asn_parse_short( &(asn_buffer[2]) );
   debug_printf( 2, "spawn sequence size: %d bytes", ts_seq_sz );

   total_read_sz += 4; /* sequence type, length fields */

   while( total_read_sz - 4 < ts_seq_sz ) {
      if( 0x30 != asn_buffer[total_read_sz] ) {
         error_printf(
            "invalid spawndef sequence type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      total_read_sz += 2; /* tile sequence type, size */
      
      /* image */
      read_sz =
         tilemap_asn_parse_string( t->spawns[spawn_idx].name,
            TILEMAP_SPAWN_NAME_SZ, &(asn_buffer[total_read_sz]) );
      if( 0 == read_sz ) {
         goto cleanup;
      }
      debug_printf( 3, "spawn name: %s (%d)",
         t->spawns[spawn_idx].name, read_sz );
      total_read_sz += read_sz; /* spawn name and header */

      /* coords header */
      if( 0x30 != asn_buffer[total_read_sz] ) {
         error_printf(
            "invalid spawn coords sequence type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      total_read_sz += 2; /* spawn coords type and size */

      /* coords.x */
      if( 0x02 != asn_buffer[total_read_sz] ) {
         error_printf( "invalid int type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      total_read_sz += 2; /* coords.x header */
      t->spawns[spawn_idx].coords.x = asn_buffer[total_read_sz];
      debug_printf( 2, "spawn X: %d", t->spawns[spawn_idx].coords.x );
      total_read_sz++;

      /* coords.y */
      if( 0x02 != asn_buffer[total_read_sz] ) {
         error_printf( "invalid int type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      total_read_sz += 2; /* coords.y header */
      t->spawns[spawn_idx].coords.y = asn_buffer[total_read_sz];
      debug_printf( 2, "spawn Y: %d", t->spawns[spawn_idx].coords.y );
      total_read_sz++;

      /* type */
      read_sz =
         tilemap_asn_parse_string( t->spawns[spawn_idx].type,
            RESOURCE_PATH_MAX, &(asn_buffer[total_read_sz]) );
      if( 0 == read_sz ) {
         goto cleanup;
      }
      debug_printf( 3, "spawn type: %s (%d)",
         t->spawns[spawn_idx].type, read_sz );
      total_read_sz += read_sz; /* spawn name and header */

      /* script_id */
      total_read_sz += 2; /* script_id header */
      t->spawns[spawn_idx].script_id = asn_buffer[total_read_sz];
      if( 0xff == t->spawns[spawn_idx].script_id ) {
         debug_printf( 2, "script ID is negative" );
         t->spawns[spawn_idx].script_id = -1;
      }
      debug_printf( 2, "script ID: %d", t->spawns[spawn_idx].script_id );
      total_read_sz++;

      spawn_idx++;
   }

   t->scripts_count = spawn_idx;

cleanup:

   return total_read_sz;
}

static int16_t tilemap_asn_parse_scripts(
   struct TILEMAP* t, const uint8_t* asn_buffer
) {
   int16_t total_read_sz = 0,
   script_seq_sz = 0,
   script_seq_start = 0,
   step_idx = 0,
   script_idx = 0,
   script_read_sz = 0;

   if( 0x30 != asn_buffer[0] ) {
      error_printf(
         "invalid scripts sequence type byte: 0x%02x", asn_buffer[0] );
      goto cleanup;
   }

   total_read_sz += 2; /* sequence type field */

   while( asn_buffer[total_read_sz] != 0 ) {
      debug_printf( 2, "script idx: %d", script_idx );

      step_idx = 0;
      script_read_sz = 0;
      script_seq_start = total_read_sz;
      if( 0x30 != asn_buffer[total_read_sz] ) {
         error_printf(
            "invalid steps sequence type byte: 0x%02x",
            asn_buffer[total_read_sz] );
         goto cleanup;
      }
      script_seq_sz = asn_buffer[total_read_sz + 1];
      if( 128 < script_seq_sz ) {
         error_printf( "invalid steps sequence size: %d", script_seq_sz );
         goto cleanup;
      }
      total_read_sz += 2; /* sequence type and size */

      while( total_read_sz - script_seq_start < script_seq_sz ) {
         debug_printf( 2, "step idx: %d", step_idx );

         if( 0x30 != asn_buffer[total_read_sz] ) {
            error_printf(
               "invalid step sequence type byte: 0x%02x",
               asn_buffer[total_read_sz] );
            goto cleanup;
         }
      
         if( 0x6 != asn_buffer[total_read_sz + 1] ) {
            error_printf(
               "invalid step size: %d", asn_buffer[total_read_sz + 1] );
            goto cleanup;
         }
         total_read_sz += 2; /* step type, size */
      
         /* step.action */
         if( 0x02 != asn_buffer[total_read_sz] ) {
            error_printf( "invalid int type byte: 0x%02x",
               asn_buffer[total_read_sz] );
            goto cleanup;
         }
         if( 2 == asn_buffer[total_read_sz + 1] ) {
            total_read_sz += 2; /* step.action header */
            /* TODO: Read short. */
         } else if( 1 == asn_buffer[total_read_sz + 1] ) {
            total_read_sz += 2; /* step.action header */
            t->scripts[script_idx].steps[step_idx].action = 
               asn_buffer[total_read_sz];
            debug_printf( 2, "script step action: %d",
               t->scripts[script_idx].steps[step_idx].action );
            total_read_sz++;
         } else {
            error_printf(
               "invalid int size: %d", asn_buffer[total_read_sz + 1] );
         }
         
         /* step.arg */
         if( 0x02 != asn_buffer[total_read_sz] ) {
            error_printf( "invalid int type byte: 0x%02x",
               asn_buffer[total_read_sz] );
            goto cleanup;
         }
         if( 2 == asn_buffer[total_read_sz + 1] ) {
            total_read_sz += 2; /* step.action header */
            /* TODO: Read short. */
         } else if( 1 == asn_buffer[total_read_sz + 1] ) {
            total_read_sz += 2; /* step.action header */
            t->scripts[script_idx].steps[step_idx].arg = 
               asn_buffer[total_read_sz];
            debug_printf( 2, "script step arg: %d",
               t->scripts[script_idx].steps[step_idx].arg );
            total_read_sz++;
         } else {
            error_printf(
               "invalid int size: %d", asn_buffer[total_read_sz + 1] );
         }

         step_idx++;
      }

      t->scripts[script_idx].steps_count = step_idx;
      script_idx++;
   }
   t->scripts_count = script_idx;

cleanup:

   return total_read_sz;
}

int16_t tilemap_asn_load( RESOURCE_ID id, struct TILEMAP* t ) {
   const uint8_t* asn_buffer = NULL;
   int16_t retval = 1,
      idx = 0;
   uint16_t read_sz = 0;
   RESOURCE_JSON_HANDLE asn_handle = (RESOURCE_JSON_HANDLE)0;

   memory_zero_ptr( t, sizeof( struct TILEMAP ) );

   /* Allocate buffers for parsing ASN.1. */
   asn_handle = resource_get_json_handle( id );
   if( (RESOURCE_JSON_HANDLE)0 == asn_handle ) {
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
   if( 0 == read_sz ) {
      goto cleanup;
   }
   debug_printf( 3, "tilemap name: %s (%d)", t->name, read_sz );
   idx += read_sz;

   /* tileset */
   read_sz = tilemap_asn_parse_tileset( t, &(asn_buffer[idx]) );
   if( 0 == read_sz ) {
      goto cleanup;
   }
   idx += read_sz;

   /* tiles */
   read_sz = tilemap_asn_parse_tiles( t, &(asn_buffer[idx]) );
   if( 0 == read_sz ) {
      goto cleanup;
   }
   idx += read_sz;

   /* strings */
   read_sz = tilemap_asn_parse_strings( t, &(asn_buffer[idx]) );
   if( 0 == read_sz ) {
      goto cleanup;
   }
   idx += read_sz;

   /* spawns */
   read_sz = tilemap_asn_parse_spawns( t, &(asn_buffer[idx]) );
   if( 0 == read_sz ) {
      goto cleanup;
   }
   idx += read_sz;

   /* scripts */
   read_sz = tilemap_asn_parse_scripts( t, &(asn_buffer[idx]) );
   if( 0 == read_sz ) {
      goto cleanup;
   }
   idx += read_sz;

cleanup:

   if( NULL != asn_buffer ) {
      resource_unlock_handle( asn_handle );
   }

   if( (MEMORY_HANDLE)NULL != asn_handle ) {
      resource_free_handle( asn_handle );
   }

   return retval;
}

