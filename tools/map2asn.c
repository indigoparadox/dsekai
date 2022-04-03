
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define ENGINES_TOKENS_ONLY
#define ENGINES_C
#include "../src/dsekai.h"

#include "../src/tmjson.h"

void buffer_assign_short( uint8_t* buffer, uint16_t n ) {
   buffer[0] = ((n & 0xff00) >> 8);
   buffer[1] = (n & 0x00ff);
}

int main( int argc, char* argv[] ) {
   FILE* map_file = NULL,
      * asn_file = NULL;
   struct TILEMAP t;
   MEMORY_HANDLE h_buffer = (MEMORY_HANDLE)NULL;
   uint8_t* buffer = NULL;
   int32_t buffer_sz = 2048,
      mark_seq_main = 0,
      mark_seq_ts = 0,
      mark_seq_spawn = 0,
      mark_seq_spawn_coords = 0,
      mark_seq_scripts = 0,
      mark_seq_item = 0,
      sz_idx = 0,
      script_sz_idx = 0,
      step_sz_idx = 0;
   int retval = 0,
      i = 0,
      j = 0,
      idx = 0,
      scripts_count = 0;

   assert( 1 < argc );

   h_buffer = memory_alloc( buffer_sz, 1 );
   assert( (MEMORY_HANDLE)NULL != h_buffer );

   retval = tilemap_json_load( argv[1], &t );
   if( !retval ) {
      retval = 1;
      goto cleanup;
   }
   retval = 0;

   idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_main );
   assert( 0 <= idx );
   
   /* version */
   debug_printf( 3, "(offset 0x%02x) writing map version", idx );
   idx = asn_write_int( &h_buffer, idx, 1 );
   assert( 0 <= idx );

   /* name */
   debug_printf( 3, "(offset 0x%02x) writing map name", idx );
   idx = asn_write_string( &h_buffer, idx, t.name, TILEMAP_NAME_MAX );
   assert( 0 <= idx );

#if 0
   /* engine_type */
   debug_printf( 3, "(offset 0x%02x) writing map engine type", idx );
   idx = asn_write_int( &h_buffer, idx, t.engine_type );
   assert( 0 <= idx );
#endif

   /* flags */
   debug_printf( 3, "(offset 0x%02x) writing map flags", idx );
   idx = asn_write_int( &h_buffer, idx, t.flags );
   assert( 0 <= idx );

   /* tileset */
   debug_printf( 3, "(offset 0x%02x) writing map tilesets", idx );
   idx = asn_write_seq_start( &h_buffer, idx, &sz_idx );
   assert( 0 < idx );
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      if( 0 == strlen( t.tileset[i].image ) ) {
         continue;
      }

      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_ts );
      assert( 0 < idx );
   
      /* image */
      debug_printf( 3, "(offset 0x%02x) writing map tileset image", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.tileset[i].image, RESOURCE_PATH_MAX );
      assert( 0 <= idx );

      /* flags */
      debug_printf( 3, "(offset 0x%02x) writing map tileset flags", idx );
      idx = asn_write_int( &h_buffer, idx, t.tileset[i].flags );
      assert( 0 <= idx );

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_ts );
      assert( 0 < idx );
   }
   idx = asn_write_seq_end( &h_buffer, idx, &sz_idx );
   assert( 0 < idx );

   /* tiles */
   debug_printf( 3, "(offset 0x%02x) writing map tiles", idx );
   idx = asn_write_blob(
      &h_buffer, idx, t.tiles, ((TILEMAP_TH * TILEMAP_TW) / 2) );

   /* strings */
   idx = asn_write_blob( &h_buffer, idx, t.strpool, TILEMAP_STRPOOL_SZ );
#if 0
   for( i = 0 ; TILEMAP_STRINGS_MAX > i ; i++ ) {
      if( 0 == strlen( t.strings[i] ) ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map string", idx );
      idx = asn_write_string( &h_buffer, idx,
         t.strings[i], DIALOG_TEXT_SZ );
      assert( 0 <= idx );
   }
#endif

   /* spawns */
   debug_printf( 3, "(offset 0x%02x) writing map spawns", idx );
   idx = asn_write_seq_start( &h_buffer, idx, &sz_idx );
   assert( 0 < idx );
   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      if( 0 == strlen( t.spawns[i].name ) ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map spawn", idx );
      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_spawn );
      assert( 0 < idx );

      /* name */
      debug_printf( 3, "(offset 0x%02x) writing map spawn name", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.spawns[i].name, TILEMAP_SPAWN_NAME_SZ );
      assert( 0 <= idx );

      /* coords */
      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_spawn_coords );
      assert( 0 < idx );

      /* coords.x */
      debug_printf( 3, "(offset 0x%02x) writing map spawn coords x", idx );
      idx = asn_write_int( &h_buffer, idx, t.spawns[i].coords.x );
      assert( 0 <= idx );

      /* coords.y */
      debug_printf( 3, "(offset 0x%02x) writing map spawn coords y", idx );
      idx = asn_write_int( &h_buffer, idx, t.spawns[i].coords.y );
      assert( 0 <= idx );

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_spawn_coords );

      /* type */
      debug_printf( 3, "(offset 0x%02x) writing map spawn type", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.spawns[i].type, RESOURCE_PATH_MAX );
      assert( 0 <= idx );

      /* script_id */
      debug_printf( 3, "(offset 0x%02x) writing map spawn script ID", idx );
      idx = asn_write_int( &h_buffer, idx, t.spawns[i].script_id );
      assert( 0 <= idx );

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_spawn );
   }
   idx = asn_write_seq_end( &h_buffer, idx, &sz_idx );
   assert( 0 < idx );

   /* scripts */
   for(
      scripts_count = 0 ; TILEMAP_SCRIPTS_MAX > scripts_count ; scripts_count++
   ) {
      if( 0 == t.scripts[scripts_count].steps[0].action ) {
         break;
      }
   }

   idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_scripts );
   assert( 0 < idx );
   for( i = 0 ; scripts_count > i ; i++ ) {
      idx = asn_write_seq_start( &h_buffer, idx, &script_sz_idx );
      assert( 0 < idx );

      /* steps */
      for( j = 0 ; SCRIPT_STEPS_MAX > j ; j++ ) {
         if( 0 == t.scripts[i].steps[j].action ) {
            break;
         }

         idx = asn_write_seq_start( &h_buffer, idx, &step_sz_idx );
         assert( 0 < idx );

         /* action */
         debug_printf(
            3, "(offset 0x%02x) writing map spawn script step action", idx );
         idx = asn_write_int( &h_buffer, idx, t.scripts[i].steps[j].action );
         assert( 0 <= idx );

         /* arg */
         debug_printf(
            3, "(offset 0x%02x) writing map spawn script step arg", idx );
         idx = asn_write_int( &h_buffer, idx, t.scripts[i].steps[j].arg );
         assert( 0 <= idx );

         idx = asn_write_seq_end( &h_buffer, idx, &step_sz_idx );
         assert( 0 < idx );
      }
      idx = asn_write_seq_end( &h_buffer, idx, &script_sz_idx );
      assert( 0 < idx );
   }
   idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_scripts );
   assert( 0 < idx );

   /* items */
   debug_printf( 3, "(offset 0x%02x) writing map items", idx );
   idx = asn_write_seq_start( &h_buffer, idx, &sz_idx );
   assert( 0 < idx );
   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      if( ITEM_FLAG_ACTIVE != (t.items[i].flags & ITEM_FLAG_ACTIVE) ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map item", idx );
      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_item );
      assert( 0 < idx );

      /* index */
      debug_printf( 3, "(offset 0x%02x) writing item index", idx );
      idx = asn_write_int( &h_buffer, idx, i );
      assert( 0 <= idx );

      /* sprite */
      debug_printf( 3, "(offset 0x%02x) writing item sprite path", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.items[i].sprite, RESOURCE_PATH_MAX );
      assert( 0 <= idx );

      /* name */
      debug_printf( 3, "(offset 0x%02x) writing item name", idx );
      idx = asn_write_string( &h_buffer, idx, t.items[i].name, ITEM_NAME_SZ );
      assert( 0 <= idx );

      /* type */
      debug_printf( 3, "(offset 0x%02x) writing item type", idx );
      idx = asn_write_int( &h_buffer, idx, t.items[i].type );
      assert( 0 <= idx );

      /* owner */
      debug_printf( 3, "(offset 0x%02x) writing item owner", idx );
      idx = asn_write_int( &h_buffer, idx, t.items[i].owner );
      assert( 0 <= idx );
 
      /* gid */
      debug_printf( 3, "(offset 0x%02x) writing item gid", idx );
      idx = asn_write_int( &h_buffer, idx, t.items[i].gid );
      assert( 0 <= idx );

      /* data */
      debug_printf( 3, "(offset 0x%02x) writing item data", idx );
      idx = asn_write_int( &h_buffer, idx, t.items[i].data );
      assert( 0 <= idx );

      /* count */
      debug_printf( 3, "(offset 0x%02x) writing item count", idx );
      idx = asn_write_int( &h_buffer, idx, t.items[i].count );
      assert( 0 <= idx );

      /* flags */
      debug_printf( 3, "(offset 0x%02x) writing item flags", idx );
      idx = asn_write_int( &h_buffer, idx, t.items[i].flags );
      assert( 0 <= idx );

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_item );
      assert( 0 < idx );
   }
   idx = asn_write_seq_end( &h_buffer, idx, &sz_idx );

   /* crop defs */
   debug_printf( 3, "(offset 0x%02x) writing map crop defs", idx );
   idx = asn_write_seq_start( &h_buffer, idx, &sz_idx );
   assert( 0 < idx );
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      if(
         CROP_DEF_FLAG_ACTIVE != (t.crop_defs[i].flags & CROP_DEF_FLAG_ACTIVE)
      ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map crop def", idx );
      /* Reuse mark_seq_item for crop def since it's not used in this scope. */
      idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_item );
      assert( 0 < idx );

      /* index */
      debug_printf( 3, "(offset 0x%02x) writing crop def index", idx );
      idx = asn_write_int( &h_buffer, idx, i );
      assert( 0 <= idx );

      /* sprite */
      debug_printf( 3, "(offset 0x%02x) writing crop def sprite path", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.crop_defs[i].sprite, RESOURCE_PATH_MAX );
      assert( 0 <= idx );

      /* name */
      debug_printf( 3, "(offset 0x%02x) writing crop def name", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.crop_defs[i].name, CROP_NAME_MAX );
      assert( 0 <= idx );

      /* gid */
      debug_printf( 3, "(offset 0x%02x) writing crop def gid", idx );
      idx = asn_write_int( &h_buffer, idx, t.crop_defs[i].gid );
      assert( 0 <= idx );

      /* flags */
      debug_printf( 3, "(offset 0x%02x) writing crop def flags", idx );
      idx = asn_write_int( &h_buffer, idx, t.crop_defs[i].flags );
      assert( 0 <= idx );

      /* cycle */
      debug_printf( 3, "(offset 0x%02x) writing crop def cycle", idx );
      idx = asn_write_int( &h_buffer, idx, t.crop_defs[i].cycle );
      assert( 0 <= idx );

      idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_item );
      assert( 0 < idx );
   }
   idx = asn_write_seq_end( &h_buffer, idx, &sz_idx );

   /* End the main sequence. */
   idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_main );
   assert( 0 < idx );

   buffer = memory_lock( h_buffer );

   /* Write the ASN map file to disk. */
   asn_file = fopen( argv[2], "wb" );
   assert( NULL != asn_file );
   fwrite( buffer, 1, idx, asn_file );
   fclose( asn_file );

   buffer = memory_unlock( h_buffer );
   
   /*
   for( i = 0 ; idx > i ; i++ ) {
      printf( "0x%02x ", buffer[i] );
   }
   */

cleanup:

   if( NULL != buffer ) {
      free( buffer );
   }

   return retval;
}

