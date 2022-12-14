
#include <headpack.h>

#include <assert.h>
#include <string.h>

#define ENGINES_TOKENS_ONLY
#define ENGINES_C
#include "../src/dsekai.h"

#include "../src/tmjson.h"

#define MAPPACK_TYPE "tilemap"
#define MAPPACK_TYPE_SZ 7

#define FILE_EXT_LEN 4
#define FILE_PREFIX_OFFSET 2

#define BIN_BUFFER_SZ 1024

int mappack_strip_ext( char* path ) {
   int16_t path_len = 0,
      basename_idx = 0;

   path_len = memory_strnlen_ptr( path, RESOURCE_PATH_MAX );

   if( 0 >= path_len ) {
      goto cleanup;
   }

   debug_printf( 2, "trimming asset path %s (%d chars)...", path, path_len );

   /* Remove the file extension if applicable. */
   if( FILE_EXT_LEN >= path_len || '.' != path[path_len - FILE_EXT_LEN] ) {
      goto cleanup;
   }

   debug_printf( 2, "removing file extension %s...",
      &(path[path_len - FILE_EXT_LEN]) );

   /* Blank out the filename extension. */
   path[path_len - FILE_EXT_LEN] = '\0';
   path_len -= FILE_EXT_LEN;

   if( 0 >= path_len ) {
      goto cleanup;
   }

   /* Show the short asset path (also determined below). */
   basename_idx = dio_basename( path, path_len );
   debug_printf( 2, "short asset path is: %s", &(path[basename_idx]) );

cleanup:
   return path_len;
}

int mappack_write( const char* map_json_path, FILE* header_file ) {
   int16_t
      i = 0,
      j = 0,
      res_basename_idx = 0,
      basename_idx = 0,
      path_iter_fname_idx = 0,
      asset_path_len = 0;
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

   /* flags */
   fprintf( header_file, "   %d,\n", t.flags );

   /* gid */
   fprintf( header_file, "   %d,\n", t.gid );

   /* tileset */
   fprintf( header_file, "   /* tileset */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_TILESETS_MAX > i ; i++ ) {

      fprintf( header_file, "      {\n" );

      asset_path_len = mappack_strip_ext( t.tileset[i].image );
      basename_idx = dio_basename( t.tileset[i].image, asset_path_len );

      if( 0 < asset_path_len  ) {
         /* tileset[i].image */
         fprintf( header_file, "         /* image */\n" );
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t.tileset[i].image[basename_idx]) );

         /* tileset[i].ascii */
         fprintf( header_file, "         /* ascii */\n" );
         if( 0 != t.tileset[i].ascii ) {
            fprintf( header_file, "         '%c',\n", t.tileset[i].ascii );
         } else {
            fprintf( header_file, "         '0',\n" );
         }

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

      fprintf( header_file, "      {\n" );

      asset_path_len = mappack_strip_ext( t.spawns[i].sprite );
      basename_idx = dio_basename( t.spawns[i].sprite, asset_path_len );

      /* name */
      fprintf( header_file, "         /* name */\n" );
      fprintf( header_file, "         \"%s\",\n", t.spawns[i].name );

      /* coords */
      fprintf( header_file, "         /* coords */\n" );
      fprintf( header_file, "         { %d, %d },\n",
         t.spawns[i].coords.x, t.spawns[i].coords.y );

      /* type */
      if( 0 < asset_path_len ) {
         fprintf( header_file, "         /* type */\n" );
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t.spawns[i].sprite[basename_idx]) );
      } else {
         fprintf( header_file, "         0, /* not found */\n" );
      }

      /* ascii */
      fprintf( header_file, "         /* ascii */\n" );
      if( 0 != t.spawns[i].ascii ) {
         fprintf( header_file, "         '%c',\n", t.spawns[i].ascii );
      } else {
         fprintf( header_file, "         '0',\n" );
      }

      /* flags */
      fprintf( header_file, "         /* flags */\n" );
      fprintf( header_file, "         %d,\n", t.spawns[i].flags );

      /* gid */
      fprintf( header_file, "         /* gid */\n" );
      fprintf( header_file, "         %d,\n", t.spawns[i].gid );

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

   /* item_defs */
   fprintf( header_file, "   /* item_defs */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      fprintf( header_file, "      {\n" );
      fprintf( header_file, "         /* sprite */\n" );

      asset_path_len = mappack_strip_ext( t.item_defs[i].sprite );
      basename_idx = dio_basename( t.item_defs[i].sprite, asset_path_len );

      if( 0 < asset_path_len ) {
         /* items[i].sprite */
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t.item_defs[i].sprite[basename_idx]) );
      } else {
         fprintf( header_file, "         0,\n" );
      }

      fprintf( header_file, "         /* sprite_id */\n" );
      fprintf( header_file, "         -1,\n" );

      fprintf( header_file, "         /* name */\n" );
      fprintf( header_file, "         \"%s\",\n", t.item_defs[i].name );

      fprintf( header_file, "         /* owner */\n" );
      fprintf( header_file, "         %d,\n", t.item_defs[i].owner );

      fprintf( header_file, "         /* gid */\n" );
      fprintf( header_file, "         %d,\n", t.item_defs[i].gid );

      fprintf( header_file, "         /* data */\n" );
      fprintf( header_file, "         %d,\n", t.item_defs[i].data );

      fprintf( header_file, "         /* flags */\n" );
      fprintf( header_file, "         %d,\n", t.item_defs[i].flags );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   /* crop_defs */
   fprintf( header_file, "   /* crop_defs */\n" );
   fprintf( header_file, "   {\n" );
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      fprintf( header_file, "      {\n" );
      fprintf( header_file, "         /* sprite */\n" );

      asset_path_len = mappack_strip_ext( t.crop_defs[i].sprite );
      basename_idx = dio_basename( t.crop_defs[i].sprite, asset_path_len );

      if( 0 < asset_path_len ) {
         /* Let the preprocessor figure it out. */
         fprintf( header_file, "         %s,\n",
            &(t.crop_defs[i].sprite[basename_idx]) );
      } else {
         fprintf( header_file, "         0,\n" );
      }

      fprintf( header_file, "         /* sprite_id */\n" );
      fprintf( header_file, "         -1,\n" );

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

   return retval;
}

int mappack_index( const char* paths_in[], int paths_in_sz, FILE* header ) {
   int16_t map_count = 0,
      path_iter_fname_idx = 0,
      path_iter_sz = 0,
      map_name_sz = 0,
      i = 0,
      retval = 0;
   struct HEADPACK_DEF* writer_def = NULL;
   char name_buffer[TILEMAP_NAME_MAX]; 

   fprintf( header, "RES_CONST char gc_map_names[][TILEMAP_NAME_MAX] = {\n" );
   for( i = 0 ; paths_in_sz > i ; i++ ) {
      path_iter_sz = memory_strnlen_ptr( paths_in[i], RESOURCE_PATH_MAX );
      path_iter_fname_idx = dio_basename( paths_in[i], path_iter_sz );

      writer_def =
         headpack_get_def( &(paths_in[i][path_iter_fname_idx]) );
      if(
         NULL == writer_def ||
         0 != memory_strncmp_ptr(
            MAPPACK_TYPE, writer_def->type, MAPPACK_TYPE_SZ )
      ) {
         continue;
      }

      map_count++;

      /* This is a map struct, so use its name. */
      /* Create a copy without the file extension. */
      memory_zero_ptr( name_buffer, TILEMAP_NAME_MAX );
      map_name_sz =
         path_iter_sz - path_iter_fname_idx -
         FILE_PREFIX_OFFSET - FILE_EXT_LEN - 1;
      debug_printf( 2, "copying input path (%d chars): %s",
         path_iter_sz - path_iter_fname_idx - FILE_PREFIX_OFFSET,
         &(paths_in[i][path_iter_fname_idx + FILE_PREFIX_OFFSET]) );
      memory_strncpy_ptr(
         name_buffer,
         &(paths_in[i][path_iter_fname_idx + FILE_PREFIX_OFFSET]),
         map_name_sz );
      debug_printf( 2, "trimmed copied path (%d chars): %s",
         map_name_sz, name_buffer );

      fprintf( header, "   \"%s\",\n", name_buffer );
   }
   fprintf( header, "};\n\n" );

   fprintf( header, "RES_CONST struct TILEMAP* gc_map_structs[] = {\n" );
   for( i = 0 ; paths_in_sz > i ; i++ ) {
      path_iter_sz = memory_strnlen_ptr( paths_in[i], RESOURCE_PATH_MAX );
      path_iter_fname_idx = dio_basename( paths_in[i], path_iter_sz );

      writer_def =
         headpack_get_def( &(paths_in[i][path_iter_fname_idx]) );
      if(
         NULL == writer_def ||
         0 != memory_strncmp_ptr(
            MAPPACK_TYPE, writer_def->type, MAPPACK_TYPE_SZ )
      ) {
         continue;
      }

      /* This is a map struct, so use its name. */

      /* Create a copy without the file extension. */
      memory_zero_ptr( name_buffer, TILEMAP_NAME_MAX );
      map_name_sz =
         path_iter_sz - path_iter_fname_idx -
         FILE_PREFIX_OFFSET - FILE_EXT_LEN - 1;
      debug_printf( 2, "copying input path (%d chars): %s",
         path_iter_sz - path_iter_fname_idx - FILE_PREFIX_OFFSET,
         &(paths_in[i][path_iter_fname_idx + FILE_PREFIX_OFFSET]) );
      memory_strncpy_ptr(
         name_buffer,
         &(paths_in[i][path_iter_fname_idx + FILE_PREFIX_OFFSET]),
         map_name_sz );
      debug_printf( 2, "trimmed copied path (%d chars): %s",
         map_name_sz, name_buffer );


      fprintf( header, "   &gc_map_%s,\n", name_buffer );
   }
   fprintf( header, "};\n\n" );

   fprintf( header, "RES_CONST uint8_t gc_map_count = %d;\n\n", map_count );

   return retval;
}

int main( int argc, char* argv[] ) {
   /* TODO: Pull this out to a format-agnostic main to grab multiple formats.
    */
   debug_printf( 3, "registering tilemap handler..." );
   headpack_register( 'm', mappack_write, mappack_index, MAPPACK_TYPE );
   return headpack_main( argc, argv );
}

