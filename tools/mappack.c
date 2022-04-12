
#include "headpack.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h> /* For calloc() */

#define ENGINES_TOKENS_ONLY
#define ENGINES_C
#include "../src/dsekai.h"

#include "../src/tmjson.h"

#define MAPPACK_TYPE "tilemap"
#define MAPPACK_TYPE_SZ 7

#define BIN_BUFFER_SZ 1024
int mappack_write( const char* map_json_path, FILE* header_file ) {
   int16_t
      i = 0,
      j = 0,
      res_basename_idx = 0,
      ts_basename_idx = 0,
      path_iter_fname_idx = 0;
   int retval = 0;
   struct TILEMAP t;

   retval = tilemap_json_load( map_json_path, &t );
   if( !retval ) {
      error_printf( "unable to load tilemap: %s", map_json_path );
      retval = 1;
      goto cleanup;
   } else {
      retval = 0;
   }

   /* For some reason, declaring this as const crashes Palm with unallocated
    * error? */
   /* TODO: strtolower? */
   fprintf( header_file, "RES_CONST struct TILEMAP gc_map_%s = {\n",
      t.name );

   /* name */
   fprintf( header_file, "   \"%s\",\n", t.name );

#if 0
   /* engine_type */
   fprintf( header_file, "   %d,\n", t.engine_type );
#endif

   /* flags */
   fprintf( header_file, "   %d,\n", t.flags );

   /* tileset */
   fprintf( header_file, "   /* tileset */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {
      ts_basename_idx = dio_basename(
         t.tileset[i].image, strlen( t.tileset[i].image ) );

      /* Blank out the filename extension. */
      t.tileset[i].image[strlen( t.tileset[i].image ) - 4] = '\0';

      fprintf( header_file, "      {\n" );

      if( 0 < strlen( t.tileset[i].image ) ) {
         /* tileset[i].image */
         fprintf( header_file, "         /* image */\n" );
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t.tileset[i].image[ts_basename_idx]) );

         /* tileset[i].flags */
         fprintf( header_file, "         /* flags */\n" );
         fprintf( header_file, "         0x%02x,\n", t.tileset[i].flags );
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
   for( i = 0 ; TILEMAP_SPAWNS_MAX > i ; i++ ) {
      printf( "%s\n", t.spawns[i].type );
      ts_basename_idx = dio_basename(
         t.spawns[i].type, strlen( t.spawns[i].type ) );

      /* Blank out the filename extension. */
      t.spawns[i].type[strlen( t.spawns[i].type ) - 4] = '\0';

      fprintf( header_file, "      {\n" );

      /* name */
      fprintf( header_file, "         /* name */\n" );
      fprintf( header_file, "         \"%s\",\n", t.spawns[i].name );

      /* coords */
      fprintf( header_file, "         /* coords */\n" );
      fprintf( header_file, "         { %d, %d },\n",
         t.spawns[i].coords.x, t.spawns[i].coords.y );

      /* type */
      if( 0 < strlen( t.spawns[i].type ) ) {
         fprintf( header_file, "         /* type */\n" );
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t.spawns[i].type[ts_basename_idx]) );
      } else {
         fprintf( header_file, "         0, /* not found */\n" );
      }

      /* script */
      fprintf( header_file, "         /* script_id */\n" );
      fprintf( header_file, "         %d\n", t.spawns[i].script_id );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* strings */
   fprintf( header_file, "   /* strings */\n   {\n      " );
   for( i = 0 ; TILEMAP_STRPOOL_SZ > i ; i++ ) {
      fprintf( header_file, "0x%02x, ", (unsigned char)(t.strpool[i] & 0xff) );
   }
   fprintf( header_file, "   },\n" );

#if 0
   /* strings */
   fprintf( header_file, "   /* strings */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_STRINGS_MAX > i ; i++ ) {
      fprintf( header_file, "      \"" );
      for( j = 0 ; strlen( t.strings[i] ) > j ; j++ ) {
         if( '\n' == t.strings[i][j] ) {
            fprintf( header_file, "\\n" );
         } else {
            fprintf( header_file, "%c", t.strings[i][j] );
         }
      }
      fprintf( header_file, "\",\n" );
   }
   fprintf( header_file, "   },\n" );
#endif

   /* scripts */
   fprintf( header_file, "   /* scripts */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_SCRIPTS_MAX > i ; i++ ) {
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

   /* items */
   fprintf( header_file, "   /* items */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      fprintf( header_file, "      {\n" );
      fprintf( header_file, "         /* sprite */\n" );

      if( '\0' != t.items[i].sprite[0] ) {
         ts_basename_idx = dio_basename(
            t.items[i].sprite, strlen( t.items[i].sprite ) );

         /* Blank out the filename extension. */
         t.items[i].sprite[strlen( t.items[i].sprite ) - 4] = '\0';

         /* items[i].sprite */
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t.items[i].sprite[ts_basename_idx]) );
      } else {
         fprintf( header_file, "         0,\n" );
      }

      fprintf( header_file, "         /* name */\n" );
      fprintf( header_file, "         \"%s\",\n", t.items[i].name );

      fprintf( header_file, "         /* type */\n" );
      fprintf( header_file, "         %d,\n", t.items[i].type );

      fprintf( header_file, "         /* owner */\n" );
      fprintf( header_file, "         %d,\n", t.items[i].owner );

      fprintf( header_file, "         /* gid */\n" );
      fprintf( header_file, "         %d,\n", t.items[i].gid );

      fprintf( header_file, "         /* data */\n" );
      fprintf( header_file, "         %d,\n", t.items[i].data );

      fprintf( header_file, "         /* count */\n" );
      fprintf( header_file, "         %d,\n", t.items[i].count );

      fprintf( header_file, "         /* flags */\n" );
      fprintf( header_file, "         %d,\n", t.items[i].flags );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* crop_defs */
   fprintf( header_file, "   /* crop_defs */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      fprintf( header_file, "      {\n" );
      fprintf( header_file, "         /* sprite */\n" );

      if( '\0' != t.crop_defs[i].sprite[0] ) {
         ts_basename_idx = dio_basename(
            t.crop_defs[i].sprite, strlen( t.crop_defs[i].sprite ) );

         /* Blank out the filename extension. */
         t.crop_defs[i].sprite[strlen( t.crop_defs[i].sprite ) - 4] = '\0';

         /* items[i].sprite */
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t.crop_defs[i].sprite[ts_basename_idx]) );
      } else {
         fprintf( header_file, "         0,\n" );
      }

      fprintf( header_file, "         /* name */\n" );
      fprintf( header_file, "         \"%s\",\n", t.crop_defs[i].name );

      fprintf( header_file, "         /* gid */\n" );
      fprintf( header_file, "         %d,\n", t.crop_defs[i].gid );

      fprintf( header_file, "         /* flags */\n" );
      fprintf( header_file, "         %d,\n", t.crop_defs[i].flags );

      fprintf( header_file, "         /* cycle */\n" );
      fprintf( header_file, "         %d,\n", t.crop_defs[i].cycle );

      fprintf( header_file, "         /* produce gid */\n" );
      fprintf( header_file, "         %d\n", t.crop_defs[i].produce_gid );

      fprintf( header_file, "      },\n" );
   }


   fprintf( header_file, "   },\n" );

   fprintf( header_file, "};\n\n" );

cleanup:

   return 0;
}

int mappack_index( const char* paths_in[], int paths_in_sz, FILE* header ) {
   int map_count = 0,
      path_iter_fname_idx = 0,
      path_iter_sz = 0,
      i = 0,
      retval = 0;
   struct HEADPACK_DEF* writer_def = NULL;
   char name_buffer[TILEMAP_NAME_MAX]; 

   fprintf( header, "RES_CONST char gc_map_names[][TILEMAP_NAME_MAX] = {\n" );
   for( i = 0 ; paths_in_sz > i ; i++ ) {
      path_iter_sz = strlen( paths_in[i] );
      path_iter_fname_idx = dio_basename( paths_in[i], path_iter_sz );

      writer_def =
         headpack_get_def( &(paths_in[i][path_iter_fname_idx]) );
      if(
         NULL == writer_def ||
         0 != strncmp( MAPPACK_TYPE, writer_def->type, MAPPACK_TYPE_SZ )
      ) {
         continue;
      }

      map_count++;

      /* This is a map struct, so use its name. */
      /* Create a copy without the file extension. */
      memory_zero_ptr( name_buffer, TILEMAP_NAME_MAX );
      strncpy(
         name_buffer,
         &(paths_in[i][path_iter_fname_idx + 2]),
         strlen( &(paths_in[i][path_iter_fname_idx + 2]) ) - 5 );

      fprintf( header, "   \"%s\",\n", name_buffer );
   }
   fprintf( header, "};\n\n" );

   fprintf( header, "RES_CONST struct TILEMAP* gc_map_structs[] = {\n" );
   for( i = 0 ; paths_in_sz > i ; i++ ) {
      path_iter_sz = strlen( paths_in[i] );
      path_iter_fname_idx = dio_basename( paths_in[i], path_iter_sz );

      writer_def =
         headpack_get_def( &(paths_in[i][path_iter_fname_idx]) );
      if(
         NULL == writer_def ||
         0 != strncmp( MAPPACK_TYPE, writer_def->type, MAPPACK_TYPE_SZ )
      ) {
         continue;
      }

      /* This is a map struct, so use its name. */

      /* Create a copy without the file extension. */
      memory_zero_ptr( name_buffer, TILEMAP_NAME_MAX );
      strncpy(
         name_buffer,
         &(paths_in[i][path_iter_fname_idx + 2]),
         strlen( &(paths_in[i][path_iter_fname_idx + 2]) ) - 5 );

      fprintf( header, "   &gc_map_%s,\n", name_buffer );
   }
   fprintf( header, "};\n\n" );

   fprintf( header, "RES_CONST uint8_t gc_map_count = %d;\n\n", map_count );

   return retval;
}

__attribute__((constructor))
void mappack_register() {
   debug_printf( 3, "registering tilemap handler..." );
   headpack_register( 'm', mappack_write, mappack_index, MAPPACK_TYPE );
}

