
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../src/dsekai.h"

#define MAPBUF_ASN_SEQUENCE   0x30

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
   int32_t buffer_sz = 32;
   int retval = 0,
      i = 0,
      j = 0,
      idx = 0,
      scripts_count = 0,
      script_sz_idx = 0,
      step_sz_idx = 0,
      sz_idx = 0;

   assert( 1 < argc );

   h_buffer = memory_alloc( buffer_sz, 1 );
   assert( (MEMORY_HANDLE)NULL != h_buffer );

   retval = tilemap_json_load( argv[1], &t );
   if( !retval ) {
      retval = 1;
      goto cleanup;
   }
   retval = 0;

   buffer = memory_lock( h_buffer );
   assert( NULL != buffer );

   buffer[idx++] = MAPBUF_ASN_SEQUENCE;
   buffer[idx++] = 0x82;
   idx += 2; /* Come back for these bytes at the end. */

   buffer = memory_unlock( h_buffer );
   assert( NULL == buffer );
   
   /* version */
   debug_printf( 3, "(offset 0x%02x) writing map version", idx );
   idx = asn_write_int( &h_buffer, idx, 1 );
   assert( 0 <= idx );

   /* name */
   debug_printf( 3, "(offset 0x%02x) writing map name", idx );
   idx = asn_write_string( &h_buffer, idx, t.name, TILEMAP_NAME_MAX );
   assert( 0 <= idx );

   /* engine_type */
   debug_printf( 3, "(offset 0x%02x) writing map engine type", idx );
   idx = asn_write_int( &h_buffer, idx, t.engine_type );
   assert( 0 <= idx );

   /* weather */
   debug_printf( 3, "(offset 0x%02x) writing map weather", idx );
   idx = asn_write_int( &h_buffer, idx, t.weather );
   assert( 0 <= idx );

   buffer = memory_lock( h_buffer );
   assert( NULL != buffer );

   /* tileset */
   debug_printf( 3, "(offset 0x%02x) writing map tilesets", idx );
   buffer[idx++] = MAPBUF_ASN_SEQUENCE;
   /* TODO: This will fail if there are only 1 or 2 tiles. */
   buffer[idx++] = 0x82;
   sz_idx = idx;
   idx += 2;
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      if( 0 == strlen( t.tileset[i].image ) ) {
         continue;
      }

      buffer[idx++] = MAPBUF_ASN_SEQUENCE;
      buffer[idx++] = 
         1 + /* MAPBUF_ASN_STRING */
         1 + /* string length */
         strlen( t.tileset[i].image ) + /* string */
         1 + /* MAPBUF_ASN_INT */
         1 + /* int length */
         1; /* flags */

      buffer = memory_unlock( h_buffer );
      assert( NULL == buffer );
   
      /* image */
      debug_printf( 3, "(offset 0x%02x) writing map tileset image", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.tileset[i].image, RESOURCE_PATH_MAX );
      assert( 0 <= idx );

      /* flags */
      debug_printf( 3, "(offset 0x%02x) writing map tileset flags", idx );
      idx = asn_write_int( &h_buffer, idx, t.tileset[i].flags );
      assert( 0 <= idx );

      buffer = memory_lock( h_buffer );
      assert( NULL != buffer );
   }
   debug_printf( 3, "tileset seq: %d bytes", idx - sz_idx );
   buffer_assign_short( &(buffer[sz_idx]), idx - sz_idx - 2 );

   buffer = memory_unlock( h_buffer );
   assert( NULL == buffer );

   /* tiles */
   debug_printf( 3, "(offset 0x%02x) writing map tiles", idx );
   idx = asn_write_blob(
      &h_buffer, idx, t.tiles, ((TILEMAP_TH * TILEMAP_TW) / 2) );
   #if 0
   buffer[idx++] = MAPBUF_ASN_BLOB;
   buffer[idx++] = 0x82; /* 2 size bytes. */
   buffer_assign_short( &(buffer[idx]), TILEMAP_TS );
   idx += 2;
   for( i = 0 ; ((TILEMAP_TH * TILEMAP_TW) / 2) > i ; i++ ) {
      buffer[idx++] = t.tiles[i];
   }
   #endif

   /* strings */
   for( i = 0 ; TILEMAP_STRINGS_MAX > i ; i++ ) {
      if( 0 == strlen( t.strings[i] ) ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map string", idx );
      idx = asn_write_string( &h_buffer, idx,
         t.strings[i], TILEMAP_STRINGS_SZ );
      assert( 0 <= idx );
   }

   buffer = memory_lock( h_buffer );
   assert( NULL != buffer );

   /* spawns */
   debug_printf( 3, "(offset 0x%02x) writing map spawns", idx );
   buffer[idx++] = MAPBUF_ASN_SEQUENCE;
   /* TODO: This will fail if there are only 1 or 2 spawners. */
   buffer[idx++] = 0x82;
   /* Hold on to the index of the size specifier until we know the size. */
   sz_idx = idx;
   idx += 2;
   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      if( 0 == strlen( t.spawns[i].name ) ) {
         continue;
      }

      debug_printf( 3, "(offset 0x%02x) writing map spawn", idx );
      buffer[idx++] = MAPBUF_ASN_SEQUENCE;
      buffer[idx++] = 
         1 + /* MAPBUF_ASN_STRING */
         1 + /* name size */
         strlen( t.spawns[i].name ) + /* name */
         2 + /* coords header */
         3 + /* coords.x */
         3 + /* coords.y */
         1 + /* MAPBUF_ASN_STRING */
         1 + /* type size */
         strlen( t.spawns[i].type ) + /* type */
         3; /* script_id */

      buffer = memory_unlock( h_buffer );
      assert( NULL == buffer );

      /* name */
      debug_printf( 3, "(offset 0x%02x) writing map spawn name", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.spawns[i].name, TILEMAP_SPAWN_NAME_SZ );
      assert( 0 <= idx );

      buffer = memory_lock( h_buffer );
      assert( NULL != buffer );

      /* coords */
      debug_printf( 3, "(offset 0x%02x) writing map spawn coords", idx );
      buffer[idx++] = MAPBUF_ASN_SEQUENCE;
      buffer[idx++] = 0x6;

      buffer = memory_unlock( h_buffer );
      assert( NULL == buffer );

      /* coords.x */
      debug_printf( 3, "(offset 0x%02x) writing map spawn coords x", idx );
      idx = asn_write_int( &h_buffer, idx, t.spawns[i].coords.x );
      assert( 0 <= idx );

      /* coords.y */
      debug_printf( 3, "(offset 0x%02x) writing map spawn coords y", idx );
      idx = asn_write_int( &h_buffer, idx, t.spawns[i].coords.y );
      assert( 0 <= idx );

      /* type */
      debug_printf( 3, "(offset 0x%02x) writing map spawn type", idx );
      idx = asn_write_string(
         &h_buffer, idx, t.spawns[i].type, RESOURCE_PATH_MAX );
      assert( 0 <= idx );

      /* script_id */
      debug_printf( 3, "(offset 0x%02x) writing map spawn script ID", idx );
      idx = asn_write_int( &h_buffer, idx, t.spawns[i].script_id );
      assert( 0 <= idx );

      buffer = memory_lock( h_buffer );
      assert( NULL != buffer );
   }
   debug_printf( 3, "spawners seq: %d bytes", idx - sz_idx );
   buffer_assign_short( &(buffer[sz_idx]), idx - sz_idx - 2 );

   /* scripts */
   for(
      scripts_count = 0 ; TILEMAP_SCRIPTS_MAX > scripts_count ; scripts_count++
   ) {
      if( 0 == t.scripts[scripts_count].steps[0].action ) {
         break;
      }
   }
      
   buffer[idx++] = MAPBUF_ASN_SEQUENCE;
   buffer[idx++] = 0x80;
   /* buffer_assign_short(
      &(buffer[idx]), sizeof( struct SCRIPT ) * scripts_count );
   idx += 2; */
   for( i = 0 ; scripts_count > i ; i++ ) {
      buffer[idx++] = MAPBUF_ASN_SEQUENCE;
      script_sz_idx = idx;
      buffer[idx++] = 0;

      /* steps */
      for( j = 0 ; SCRIPT_STEPS_MAX > j ; j++ ) {
         if( 0 == t.scripts[i].steps[j].action ) {
            break;
         }

         buffer[idx++] = MAPBUF_ASN_SEQUENCE;
         step_sz_idx = idx;
         buffer[idx++] = 0;

         buffer = memory_unlock( h_buffer );
         assert( NULL == buffer );

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

         buffer = memory_lock( h_buffer );
         assert( NULL != buffer );

         buffer[step_sz_idx] = idx - step_sz_idx - 1;
      }
      buffer[script_sz_idx] = idx - script_sz_idx - 1;
   }
   buffer[idx++] = 0;
   buffer[idx++] = 0;

   /* Go back and write size to header. */
   buffer_assign_short( &(buffer[2]), idx - 4 ); /* -4 for seq header. */
   debug_printf( 3, "%d bytes encoded (0x%02x 0x%02x)", idx,
      ((idx & 0xff00) >> 8), idx & 0x00ff );

   /* Write the ASN map file to disk. */
   asn_file = fopen( argv[2], "wb" );
   assert( NULL != asn_file );
   fwrite( buffer, 1, idx, asn_file );
   fclose( asn_file );
   
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

