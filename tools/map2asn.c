
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../src/dsekai.h"

#define MAPBUF_ASN_STRING     0x16
#define MAPBUF_ASN_INT        0x02
#define MAPBUF_ASN_BLOB       0x04
#define MAPBUF_ASN_SEQUENCE   0x30

uint8_t* buffer_copy_bytes(
   int* p_idx, uint8_t* buffer, int* p_buffer_sz, uint8_t* source, int source_sz
) {
   int i = 0;

   while( *p_idx + TILEMAP_NAME_MAX > *p_buffer_sz ) {
      *p_buffer_sz *= 2;
      debug_printf( 2, "resizing buffer to %d", *p_buffer_sz );
      buffer = realloc( buffer, *p_buffer_sz );
      assert( NULL != buffer );
   }
   for( i = *p_idx ; *p_buffer_sz > i ; i++ ) {
      buffer[i] = 0;
   }
   if( 0 < source_sz ) {
      memcpy( &(buffer[*p_idx]), source, source_sz );
   }
   *p_idx += source_sz;

   return buffer;
}

void buffer_assign_short( uint8_t* buffer, uint16_t n ) {
   buffer[0] = ((n & 0xff00) >> 8);
   buffer[1] = (n & 0x00ff);
}

int main( int argc, char* argv[] ) {
   FILE* map_file = NULL,
      * asn_file = NULL;
   struct TILEMAP t;
   uint8_t* buffer = NULL;
   uint32_t* p_uint32;
   uint16_t* p_uint16;
   int retval = 0,
      i = 0,
      j = 0,
      buffer_sz = 32,
      idx = 0,
      scripts_count = 0,
      script_sz_idx = 0,
      step_sz_idx = 0,
      sz_idx = 0;

   assert( 1 < argc );

   buffer = calloc( buffer_sz, 1 );
   assert( NULL != buffer );

   retval = tilemap_json_load( argv[1], &t );
   if( !retval ) {
      retval = 1;
      goto cleanup;
   }
   retval = 0;

   buffer[idx++] = MAPBUF_ASN_SEQUENCE;
   buffer[idx++] = 0x82;
   idx += 2; /* Come back for these bytes at the end. */
   
   /* version */
   buffer[idx++] = MAPBUF_ASN_INT;
   buffer[idx++] = 1;
   buffer[idx++] = 1;

   /* name */
   buffer[idx++] = MAPBUF_ASN_STRING;
   buffer[idx++] = strlen( t.name );
   buffer = buffer_copy_bytes(
      &idx, buffer, &buffer_sz, t.name, strlen( t.name ) );

   /* engine_type */
   buffer[idx++] = MAPBUF_ASN_INT;
   buffer[idx++] = 1;
   buffer[idx++] = t.engine_type;

   /* tileset */
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

      /* image */
      buffer[idx++] = MAPBUF_ASN_STRING;
      buffer[idx++] = strlen( t.tileset[i].image );
      buffer = buffer_copy_bytes(
         &idx, buffer, &buffer_sz,
         t.tileset[i].image, strlen( t.tileset[i].image ) );

      /* flags */
      buffer[idx++] = MAPBUF_ASN_INT;
      buffer[idx++] = 1;
      buffer[idx++] = t.tileset[i].flags;
   }
   debug_printf( 3, "tileset seq: %d bytes", idx - sz_idx );
   buffer_assign_short( &(buffer[sz_idx]), idx - sz_idx - 2 );

   /* tiles */
   buffer[idx++] = MAPBUF_ASN_BLOB;
   buffer[idx++] = 0x82; /* 2 size bytes. */
   buffer_assign_short( &(buffer[idx]), TILEMAP_TS );
   idx += 2;
   for( i = 0 ; ((TILEMAP_TH * TILEMAP_TW) / 2) > i ; i++ ) {
      if( idx + 2 >= buffer_sz ) {
         buffer_sz *= 2;
         buffer = realloc( buffer, buffer_sz );
         assert( NULL != buffer );
      }
      buffer[idx++] = t.tiles[i];
   }

   /* strings */
   for( i = 0 ; TILEMAP_STRINGS_MAX > i ; i++ ) {
      if( 0 == strlen( t.strings[i] ) ) {
         continue;
      }

      buffer[idx++] = MAPBUF_ASN_STRING;
      buffer[idx++] = strlen( t.strings[i] );
      buffer = buffer_copy_bytes(
         &idx, buffer, &buffer_sz,
         t.strings[i], strlen( t.strings[i] ) );
   }

   /* spawns */
   buffer[idx++] = MAPBUF_ASN_SEQUENCE;
   /* TODO: This will fail if there are only 1 or 2 spawners. */
   buffer[idx++] = 0x82;
   sz_idx = idx;
   idx += 2;
   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      if( 0 == strlen( t.spawns[i].name ) ) {
         continue;
      }

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

      /* name */
      buffer[idx++] = MAPBUF_ASN_STRING;
      buffer[idx++] = strlen( t.spawns[i].name );
      buffer = buffer_copy_bytes(
         &idx, buffer, &buffer_sz,
         t.spawns[i].name, strlen( t.spawns[i].name ) );

      /* coords */
      buffer[idx++] = MAPBUF_ASN_SEQUENCE;
      buffer[idx++] = 0x6;

      /* coords.x */
      buffer[idx++] = MAPBUF_ASN_INT;
      buffer[idx++] = 1;
      buffer[idx++] = t.spawns[i].coords.x;

      /* coords.y */
      buffer[idx++] = MAPBUF_ASN_INT;
      buffer[idx++] = 1;
      buffer[idx++] = t.spawns[i].coords.y;

      /* type */
      buffer[idx++] = MAPBUF_ASN_STRING;
      buffer[idx++] = strlen( t.spawns[i].type );
      buffer = buffer_copy_bytes(
         &idx, buffer, &buffer_sz,
         t.spawns[i].type, strlen( t.spawns[i].type ) );

      /* script_id */
      buffer[idx] = MAPBUF_ASN_INT;
      if( 0 > t.spawns[i].script_id ) {
         /* Negative numbers prepended with a zero. */
         buffer[idx++] |= 0x40; /* Negative */
         buffer[idx++] = 1;
         buffer[idx++] = t.spawns[i].script_id * 1;
      } else {
         idx++;
         buffer[idx++] = 1;
         buffer[idx++] = t.spawns[i].script_id;
      }
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

         /* action */
         buffer[idx++] = MAPBUF_ASN_INT;
         if( t.scripts[i].steps[j].action > 255 ) {
            buffer[idx++] = 2;
            buffer_assign_short(
               &(buffer[idx]), t.scripts[i].steps[j].action );
            idx += 2;
         } else {
            buffer[idx++] = 1;
            buffer[idx++] = t.scripts[i].steps[j].action;
         }

         /* arg */
         buffer[idx++] = MAPBUF_ASN_INT;
         if( t.scripts[i].steps[j].arg > 255 ) {
            buffer[idx++] = 2;
            buffer_assign_short(
               &(buffer[idx]), t.scripts[i].steps[j].arg );
            idx += 2;
         } else {
            buffer[idx++] = 1;
            buffer[idx++] = t.scripts[i].steps[j].arg;
         }

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
   fwrite( buffer, idx, 1, asn_file );
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

