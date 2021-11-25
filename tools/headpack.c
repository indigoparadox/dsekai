
#include "headpack.h"

#include <assert.h>
#include <string.h>

#include "../src/dsekai.h"

#include "../src/tmjson.h"

#define BIN_BUFFER_SZ 1024
#define RES_C_DEF "RESOURCE_C"
#define RES_TYPE "unsigned char"

int path_bin_or_txt( const char* path ) {
   int i = 0,
      last_sep = 0,
      path_sz = 0;

   path_sz = strlen( path );

   for( i = 0 ; path_sz > i ; i++ ) {
      if( '.' == path[i] ) {
         if( 0 == strncmp( "json", &(path[i + 1]), path_sz - (i + 1) ) ) {
            return PATH_TYPE_TXT;
         }
      }
   }

   return PATH_TYPE_BIN;
}

int path_to_define( const char* path, FILE* header ) {
   int i = 0,
      last_sep = 0,
      path_sz = 0,
      written = 0;

   path_sz = strlen( path );
   for( i = 0 ; path_sz > i ; i++ ) {
      if( '/' == path[i] ) {
         last_sep = i;
      }
   }

   for( i = last_sep > 0 ? last_sep + 1 : 0 ; path_sz > i ; i++ ) {
      if( '.' == path[i] ) {
         break;
      }
      fputc( path[i], header );
      written++;
   }

   return written;
}

#define encode_generic_file_line( ... ) written = fprintf( header, __VA_ARGS__ ); if( 0 >= written ) { error_printf( "unable to write to header" ); goto cleanup; } total_written += written;

int encode_generic_file( char* path, int id, FILE* header ) {
   unsigned char bin_buffer[BIN_BUFFER_SZ];
   int read = 0,
      j = 0,
      written = 0,
      total_written = 0,
      fsize = 0;
   FILE* bin = NULL;

   bin = fopen( path, "rb" );
   if( NULL == bin ) {
      error_printf( "unable to open resource: %s", path );
   }

   /* Create a static const in the output header to hold this blob. */
   encode_generic_file_line( 
      "static const " RES_TYPE " gsc_resource_%d[] = {\n   ",
      /* Subtract 1 since this is argv (program name and output header args),
       * but we index from 1. */
      id - 1 );

   encode_generic_file_line( "   /* %s */\n", path );
   
   /* Translate each byte into a hex number in the output header. */
   while( 0 < (read = fread( bin_buffer, 1, BIN_BUFFER_SZ, bin )) ) {
      for( j = 0 ; read > j ; j++ ) {
         encode_generic_file_line( "0x%02x, ", (unsigned char)bin_buffer[j] );
      }
   }

   /* Add a null terminator. */
   if( PATH_TYPE_TXT == path_bin_or_txt( path ) ) {
      encode_generic_file_line( "0x00" );
   }

   encode_generic_file_line( "\n};\n\n" );

   encode_generic_file_line(
      "static const struct RESOURCE_HEADER_HANDLE gsc_resource_handle_%d[] = {\n   ",
      id - 1 );

   fsize = ftell( bin );
   encode_generic_file_line( "gsc_resource_%d,\n   %d,\n   0",
      id - 1, fsize );

   encode_generic_file_line( "\n};\n\n" );

cleanup:

   if( NULL != bin ) {
      fclose( bin );
   }

   return total_written;
}

int map2h( struct TILEMAP* t, FILE* header_file ) {
   int16_t
      i = 0,
      j = 0,
      res_basename_idx = 0,
      ts_basename_idx = 0;

   /* For some reason, declaring this as const crashes Palm with unallocated
    * error? */

   fprintf( header_file, "#ifdef PLATFORM_PALM\n" );

   /* TODO: strtolower? */
   fprintf( header_file, "struct TILEMAP gc_map_%s = {\n",
      t->name );

   fprintf( header_file, "#else\n" );
   
   /* TODO: strtolower? */
   fprintf( header_file, "const struct TILEMAP gc_map_%s = {\n",
      t->name );

   fprintf( header_file, "#endif\n" );
   
   /* name */
   fprintf( header_file, "   \"%s\",\n", t->name );

   /* engine_type */
   fprintf( header_file, "   %d,\n", t->engine_type );

   /* weather */
   fprintf( header_file, "   %d,\n", t->weather );

   /* tileset */
   fprintf( header_file, "   /* tileset */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      ts_basename_idx = dio_basename(
         t->tileset[i].image, strlen( t->tileset[i].image ) );

      /* Blank out the filename extension. */
      t->tileset[i].image[strlen( t->tileset[i].image ) - 4] = '\0';

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
   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      printf( "%s\n", t->spawns[i].type );
      ts_basename_idx = dio_basename(
         t->spawns[i].type, strlen( t->spawns[i].type ) );

      /* Blank out the filename extension. */
      t->spawns[i].type[strlen( t->spawns[i].type ) - 4] = '\0';

      fprintf( header_file, "      {\n" );

      /* name */
      fprintf( header_file, "         /* name */\n" );
      fprintf( header_file, "         \"%s\",\n", t->spawns[i].name );

      /* coords */
      fprintf( header_file, "         /* coords */\n" );
      fprintf( header_file, "         { %d, %d },\n",
         t->spawns[i].coords.x, t->spawns[i].coords.y );

      /* type */
      if( 0 < strlen( t->spawns[i].type ) ) {
         fprintf( header_file, "         /* type */\n" );
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t->spawns[i].type[ts_basename_idx]) );
      } else {
         fprintf( header_file, "         0, /* not found */\n" );
      }

      /* script */
      fprintf( header_file, "         /* script_id */\n" );
      fprintf( header_file, "         %d\n", t->spawns[i].script_id );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* strings */
   fprintf( header_file, "   /* strings */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_STRINGS_MAX > i ; i++ ) {
      fprintf( header_file, "      \"" );
      for( j = 0 ; strlen( t->strings[i] ) > j ; j++ ) {
         if( '\n' == t->strings[i][j] ) {
            fprintf( header_file, "\\n" );
         } else {
            fprintf( header_file, "%c", t->strings[i][j] );
         }
      }
      fprintf( header_file, "\",\n" );
   }
   fprintf( header_file, "   },\n" );

   /* scripts */
   fprintf( header_file, "   /* scripts */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_SCRIPTS_MAX > i ; i++ ) {
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

   /* items */
   fprintf( header_file, "   /* items */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      fprintf( header_file, "      {\n" );
      fprintf( header_file, "         /* sprite */\n" );

      if( '\0' != t->items[i].sprite[0] ) {
         ts_basename_idx = dio_basename(
            t->items[i].sprite, strlen( t->items[i].sprite ) );

         /* Blank out the filename extension. */
         t->items[i].sprite[strlen( t->items[i].sprite ) - 4] = '\0';

         /* items[i].sprite */
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t->items[i].sprite[ts_basename_idx]) );
      } else {
         fprintf( header_file, "         0,\n" );
      }

      fprintf( header_file, "         /* name */\n" );
      fprintf( header_file, "         \"%s\",\n", t->items[i].name );

      fprintf( header_file, "         /* type */\n" );
      fprintf( header_file, "         %d,\n", t->items[i].type );

      fprintf( header_file, "         /* owner */\n" );
      fprintf( header_file, "         %d,\n", t->items[i].owner );

      fprintf( header_file, "         /* gid */\n" );
      fprintf( header_file, "         %d,\n", t->items[i].gid );

      fprintf( header_file, "         /* data */\n" );
      fprintf( header_file, "         %d,\n", t->items[i].data );

      fprintf( header_file, "         /* count */\n" );
      fprintf( header_file, "         %d,\n", t->items[i].count );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   fprintf( header_file, "};\n\n" );

   return 1;
}

#ifndef HEADPACK_NOMAIN

int main( int argc, char* argv[] ) {
   int res_id = 0,
      i = 0,
      j = 0,
      read = 0,
      define_offset = 0,
      path_iter_fname_idx = 0,
      path_iter_sz = 0,
      map_count = 0,
      retval = 0;
   FILE* header = NULL;
   struct TILEMAP t;
   unsigned char byte_buffer = 0;
   char name_buffer[TILEMAP_NAME_MAX];

   if( 2 >= argc ) {
      printf( "usage: headpack <header path> <paths to files to encode>\n" );
      return 1;
   }

   memset( &t, '\0', sizeof( struct TILEMAP ) );

   header = fopen( argv[1], "w" );
   assert( NULL != header );

   /* Output header include guard start. */
   fprintf( header, "#ifndef RESEMB_H\n#define RESEMB_H\n\n" );

   fprintf( header, "#include \"../../src/itstruct.h\"\n" );
   fprintf( header, "#include \"../../src/tmstruct.h\"\n" );

   /* TODO: Use dynamic path to resource header. */
   fprintf( header, "#include \"../../unilayer/src/resource/header.h\"\n" );

   fprintf( header, "\n" );

   /* fprintf( header, "#include \"residx.h\"\n\n" ); */

   for( i = 2 ; argc > i ; i++ ) {
      fprintf( header, "#define " );
      path_to_define( argv[i], header );
      fprintf( header, " %d\n", i - 1 );
   }

   /* Resource Data */

   /* Translation unit check start. */
   fprintf( header, "\n#ifdef " RES_C_DEF "\n\n" );

   /* Iterate through each file on the command line. */
   for( i = 2 ; argc > i ; i++ ) {
      path_iter_sz = strlen( argv[i] );
      path_iter_fname_idx = dio_basename( argv[i], path_iter_sz );
      debug_printf(
         3, "encoding resource: %s", &(argv[i][path_iter_fname_idx]) );
      printf(
         "encoding resource: %s\n", &(argv[i][path_iter_fname_idx]) );
      /* TODO: Delay maps until we have all other resources so we can map tilesets. */
      if(
         'm' == argv[i][path_iter_fname_idx] &&
         '_' == argv[i][path_iter_fname_idx + 1]
      ) {
         /* This is a map JSON file, so preload it. */

         retval = tilemap_json_load( argv[i], &t );
         if( !retval ) {
            error_printf( "unable to load tilemap: %s",
               &(argv[i][path_iter_fname_idx]) );
            printf( "unable to load tilemap: %s\n",
               &(argv[i][path_iter_fname_idx]) );
            retval = 1;
            goto cleanup;
         } else {
            retval = 0;
         }
      
         /* Trim first arg (prog) off reslist. */
         /* Second arg (header) will be ignored since 1-indexing. */
         map2h( &t, header );
      } else {
         encode_generic_file( &(argv[i][0]), i, header );
      }
   }

   /* Resource Index */

   fprintf( header, "static const struct RESOURCE_HEADER_HANDLE* gsc_resources[] = {\n" );
   fprintf( header, "   NULL,\n" );
   for( i = 2 ; argc > i ; i++ ) {
      if(
         'm' == argv[i][path_iter_fname_idx] &&
         '_' == argv[i][path_iter_fname_idx + 1]
      ) {
         /* Map structs are handled in the map index table below. */
      } else {
         /* Use a generic resource ID. */
         fprintf( header, "   gsc_resource_handle_%d,\n", i - 1 );
      }
   }
   fprintf( header, "};\n\n" );

   /* Map Index */

   fprintf( header, "const char gc_map_names[][TILEMAP_NAME_MAX] = {\n" );
   for( i = 2 ; argc > i ; i++ ) {
      if(
         'm' == argv[i][path_iter_fname_idx] &&
         '_' == argv[i][path_iter_fname_idx + 1]
      ) {
         /* This is a map struct, so use its name. */

         map_count++;

         /* Create a copy without the file extension. */
         memory_zero_ptr( name_buffer, TILEMAP_NAME_MAX );
         strncpy(
            name_buffer,
            &(argv[i][path_iter_fname_idx + 2]),
            strlen( &(argv[i][path_iter_fname_idx + 2]) ) - 5 );

         fprintf( header, "   \"%s\",\n", name_buffer );
      }
   }
   fprintf( header, "};\n\n" );

   fprintf( header, "const struct TILEMAP* gc_map_structs[] = {\n" );
   for( i = 2 ; argc > i ; i++ ) {
      if(
         'm' == argv[i][path_iter_fname_idx] &&
         '_' == argv[i][path_iter_fname_idx + 1]
      ) {
         /* This is a map struct, so use its name. */

         /* Create a copy without the file extension. */
         memory_zero_ptr( name_buffer, TILEMAP_NAME_MAX );
         strncpy(
            name_buffer,
            &(argv[i][path_iter_fname_idx + 2]),
            strlen( &(argv[i][path_iter_fname_idx + 2]) ) - 5 );

         fprintf( header, "   &gc_map_%s,\n", name_buffer );
      }
   }
   fprintf( header, "};\n\n" );

   fprintf( header, "const uint8_t gc_map_count = %d;\n\n", map_count );

   /* Header Footer */

   /* Translation unit check end. */
   fprintf( header, "#endif /* " RES_C_DEF " */\n\n" );

   /* Output header include guard end. */
   fprintf( header, "#endif /* !RESEMB_H */\n\n" );

cleanup:

   fclose( header );
   
   return retval;
}

#endif /* !HEADPACK_NOMAIN */

