
#include "../src/dsekai.h"

#include "map2h.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
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

#endif

int map2h(
   struct TILEMAP* t, FILE* header_file, int map_idx
) {
   int16_t
      i = 0,
      j = 0,
      res_basename_idx = 0,
      ts_basename_idx = 0;

   /* TODO: strtolower? */
   fprintf( header_file, "const struct TILEMAP gc_map_%s = {\n",
      t->name );
   
   /* name */
   fprintf( header_file, "   \"%s\",\n", t->name );

   /* tileset */
   fprintf( header_file, "   /* tileset */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      ts_basename_idx = dio_basename(
         t->tileset[i].image, strlen( t->tileset[i].image ) );

      /* Blank out the filename extension. */
      t->tileset[i].image[strlen( t->tileset[i].image ) - 4] = '\0';

      /* Get resource ID for tile image. */
      /* This assumes map2h was compiled with RESOURCE_FILE. */
      #if 0
      res_idx = -1;
      for( j = res_list_sz - 1 ; 0 <= j ; j-- ) {
         res_basename_idx = dio_basename(
            res_list_names[j], strlen( res_list_names[j] ) );
         printf( "%d out of %d: %s vs %s\n",
            j,
            res_list_sz,
            &(res_list_names[j][res_basename_idx]),
            &(t->tileset[i].image[ts_basename_idx]) );
         if( 0 == strncmp(
            &(res_list[j][res_basename_idx]),
            &(t->tileset[i].image[ts_basename_idx]),
            strlen( res_list_names[j] ) - res_basename_idx - 4 /* Extension */
         ) ) {
            res_idx = j;
            printf( "found: %d (%s)\n", res_idx,
               &(res_list_names[res_idx][res_basename_idx]) );
            break;
         }
      }
      #endif

      fprintf( header_file, "      {\n" );

      if( 0 < strlen( t->tileset[i].image ) ) {
         /* tileset[i].image */
         fprintf( header_file, "         /* image */\n" );
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t->tileset[i].image[ts_basename_idx]) );

         /* tileset[i].flags */
         fprintf( header_file, "         /* flags */\n" );
         fprintf( header_file, "         0x%02x,\n", t->tileset[i].flags );
      } else {
         fprintf( header_file, "         0, 0 /* not found */\n" );
      }

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* tiles */
   fprintf( header_file, "   /* tiles */\n" );
   fprintf( header_file, "   {\n      " );
   for( i = 0 ; ((TILEMAP_TH * TILEMAP_TW) / 2) > i ; i++ ) {
      fprintf( header_file, "0x%02x, ", t->tiles[i] );
      if( 0 == ((i + 1) % (TILEMAP_TW / 2)) ) {
         fprintf( header_file, "\n" );
      }
   }
   fprintf( header_file, "   },\n" );

   /* tiles_flags */ 
   fprintf( header_file, "   /* tiles_flags */\n" );
   fprintf( header_file, "   {\n      " );
   for( i = 0 ; (TILEMAP_TH * TILEMAP_TW) > i ; i++ ) {
      fprintf( header_file, "0x%02x, ", t->tiles_flags[i] );
      if( 0 == ((i + 1) % (TILEMAP_TW)) ) {
         fprintf( header_file, "\n" );
      }
   }
   fprintf( header_file, "   },\n" );

   /* spawns */
   fprintf( header_file, "   /* spawns */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; t->spawns_count > i ; i++ ) {
      fprintf( header_file, "      {\n" );

      /* coords */
      fprintf( header_file, "         /* coords */\n" );
      fprintf( header_file, "         { %d, %d },\n",
         t->spawns[i].coords.x, t->spawns[i].coords.y );

      /* type */
      fprintf( header_file, "         /* type */\n" );
      fprintf( header_file, "         %d,\n", t->spawns[i].type );

      /* script */
      /*fprintf( header_file, "         {\n" );
      for( j = 0 ; MOBILE_SCRIPT_STEPS_MAX > j ; j++ ) {
         fprintf( header_file, "            { %d, %d },\n",
            t->spawns[i].script[j].action, t->spawns[i].script[j].arg );
      }
      fprintf( header_file, "         },\n" ); */
      fprintf( header_file, "         /* script_id */\n" );
      fprintf( header_file, "         %d\n", t->spawns[i].script_id );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* strings */
   fprintf( header_file, "   /* strings */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; t->strings_count > i ; i++ ) {
      fprintf( header_file, "      \"%s\",\n", t->strings[i] );
   }
   fprintf( header_file, "   },\n" );

   /* string_szs */
   fprintf( header_file, "   /* string_szs */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; t->strings_count > i ; i++ ) {
      fprintf( header_file, "      %d,\n", t->string_szs[i] );
   }
   fprintf( header_file, "   },\n" );

   /* strings_count */
   fprintf( header_file, "   /* strings_count */\n" );
   fprintf( header_file, "   %d,\n", t->strings_count );

   /* spawns_count */
   fprintf( header_file, "   /* spawns_count */\n" );
   fprintf( header_file, "   %d,\n", t->spawns_count );

   /* scripts */
   fprintf( header_file, "   /* scripts */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; t->scripts_count > i ; i++ ) {
      fprintf( header_file, "      {\n" );
      /* steps */
      fprintf( header_file, "         /* steps */\n" );
      fprintf( header_file, "         {\n" );
      for( j = 0 ; t->scripts[i].steps_count > j ; j++ ) {
         fprintf( header_file, "            {\n" );
         fprintf( header_file, "               /* action */\n" );
         fprintf( header_file, "               %d,\n",
            t->scripts[i].steps[j].action );
         fprintf( header_file, "               /* arg */\n" );
         fprintf( header_file, "               %d,\n",
            t->scripts[i].steps[j].arg );
         fprintf( header_file, "            },\n" );
      }
      fprintf( header_file, "         },\n" );

      /* steps_count */
      fprintf( header_file, "         /* steps_count */\n" );
      fprintf( header_file, "         %d\n", t->scripts[i].steps_count );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* scripts_count */
   fprintf( header_file, "   /* scripts_count */\n" );
   fprintf( header_file, "   %d\n", t->scripts_count );

   fprintf( header_file, "};\n\n" );

   return 1;
}

