
#include "../src/dsekai.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char* argv[] ) {
   FILE* json_file = NULL,
      * header_file = NULL;
   int16_t json_file_sz = 0,
      i = 0,
      j = 0;
   char* json_buffer = 0;
   struct TILEMAP t;
   jsmntok_t tokens[2048];

   assert( argc == 3 );

   memset( &t, '\0', sizeof( struct TILEMAP ) );
   memset( tokens, '\0', 2048 * sizeof( jsmntok_t ) );

   /* Parse tilemap in. */
   json_file = fopen( argv[1], "r" );
   assert( NULL != json_file );

   fseek( json_file, 0, SEEK_END );
   json_file_sz = ftell( json_file );
   fseek( json_file, 0, SEEK_SET );

   json_buffer = calloc( json_file_sz + 1, 1 );
   assert( NULL != json_buffer );
   fread( json_buffer, json_file_sz, 1, json_file );

   tilemap_parse( &t, json_buffer, json_file_sz, &(tokens[0]), 2048 );

   fclose( json_file );

   /* Write tilemap header out. */
   header_file = fopen( argv[2], "w" );
   assert( NULL != header_file );

   /* TODO: strtolower */
   fprintf( header_file, "const struct TILEMAP gc_map_%s = {\n",
      t.name );
   
   /* name */
   fprintf( header_file, "   \"%s\",\n", t.name );

   /* tileset */
   fprintf( header_file, "   /* tileset */\n" );
   fprintf( header_file, "   { 0 },\n" );

   /* tiles */
   fprintf( header_file, "   /* tiles */\n" );
   fprintf( header_file, "   {\n      " );
   for( i = 0 ; ((TILEMAP_TH * TILEMAP_TW) / 2) > i ; i++ ) {
      fprintf( header_file, "0x%02x, ", t.tiles[i] );
      if( 0 == ((i + 1) % (TILEMAP_TW / 2)) ) {
         fprintf( header_file, "\n" );
      }
   }
   fprintf( header_file, "   },\n" );

   /* tiles_flags */ 
   fprintf( header_file, "   /* tiles_flags */\n" );
   fprintf( header_file, "   {\n      " );
   for( i = 0 ; (TILEMAP_TH * TILEMAP_TW) > i ; i++ ) {
      fprintf( header_file, "0x%02x, ", t.tiles_flags[i] );
      if( 0 == ((i + 1) % (TILEMAP_TW)) ) {
         fprintf( header_file, "\n" );
      }
   }
   fprintf( header_file, "   },\n" );

   /* spawns */
   fprintf( header_file, "   /* spawns */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; t.spawns_count > i ; i++ ) {
      fprintf( header_file, "      {\n" );

      /* coords */
      fprintf( header_file, "         /* coords */\n" );
      fprintf( header_file, "         { %d, %d },\n",
         t.spawns[i].coords.x, t.spawns[i].coords.y );

      /* type */
      fprintf( header_file, "         /* type */\n" );
      fprintf( header_file, "         %d,\n", t.spawns[i].type );

      /* script */
      /*fprintf( header_file, "         {\n" );
      for( j = 0 ; MOBILE_SCRIPT_STEPS_MAX > j ; j++ ) {
         fprintf( header_file, "            { %d, %d },\n",
            t.spawns[i].script[j].action, t.spawns[i].script[j].arg );
      }
      fprintf( header_file, "         },\n" ); */
      fprintf( header_file, "         /* script_id */\n" );
      fprintf( header_file, "         %d\n", t.spawns[i].script_id );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* strings */
   fprintf( header_file, "   /* strings */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; t.strings_count > i ; i++ ) {
      fprintf( header_file, "      \"%s\",\n", t.strings[i] );
   }
   fprintf( header_file, "   },\n" );

   /* string_szs */
   fprintf( header_file, "   /* string_szs */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; t.strings_count > i ; i++ ) {
      fprintf( header_file, "      %d,\n", t.string_szs[i] );
   }
   fprintf( header_file, "   },\n" );

   /* strings_count */
   fprintf( header_file, "   /* strings_count */\n" );
   fprintf( header_file, "   %d,\n", t.strings_count );

   /* spawns_count */
   fprintf( header_file, "   /* spawns_count */\n" );
   fprintf( header_file, "   %d,\n", t.spawns_count );

   /* scripts */
   fprintf( header_file, "   /* scripts */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; t.scripts_count > i ; i++ ) {
      fprintf( header_file, "      {\n" );
      /* steps */
      fprintf( header_file, "         /* steps */\n" );
      fprintf( header_file, "         {\n" );
      for( j = 0 ; t.scripts[i].steps_count > j ; j++ ) {
         fprintf( header_file, "            {\n" );
         fprintf( header_file, "               /* action */\n" );
         fprintf( header_file, "               %d,\n",
            t.scripts[i].steps[j].action );
         fprintf( header_file, "               /* arg */\n" );
         fprintf( header_file, "               %d,\n",
            t.scripts[i].steps[j].arg );
         fprintf( header_file, "            },\n" );
      }
      fprintf( header_file, "         },\n" );

      /* steps_count */
      fprintf( header_file, "         /* steps_count */\n" );
      fprintf( header_file, "         %d\n", t.scripts[i].steps_count );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* scripts_count */
   fprintf( header_file, "   /* scripts_count */\n" );
   fprintf( header_file, "   %d\n", t.scripts_count );

   fprintf( header_file, "};\n" );

   return 0;
}

