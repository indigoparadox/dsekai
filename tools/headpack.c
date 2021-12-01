
#include "headpack.h"

#define CONVERT_C
#include "../unilayer/tools/convert.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h> /* For calloc() */

#define ENGINES_TOKENS_ONLY
#define ENGINES_C
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



#define encode_binary_buffer_line( ... ) written = fprintf( header, __VA_ARGS__ ); if( 0 >= written ) { error_printf( "unable to write to header" ); goto cleanup; } total_written += written;

int encode_binary_buffer(
   /* TODO: Use int32 for buffer_in_sz. */
   unsigned char* buffer_in, int buffer_in_sz, char* res_path,
   int id, FILE* header, int in_fmt, int out_fmt
) {
   int j = 0,
      written = 0,
      total_written = 0,
      retval = 0;

   /* Create a static const in the output header to hold this blob. */
   encode_binary_buffer_line( 
      "static const " RES_TYPE " gsc_resource_%d[] = {\n   ", id );

   encode_binary_buffer_line( "   /* %s */\n", res_path );
   
   /* Translate each byte into a hex number in the output header. */
   for( j = 0 ; buffer_in_sz > j ; j++ ) {
      encode_binary_buffer_line( "0x%02x, ", (unsigned char)buffer_in[j] );
   }

   /* Add a null terminator. */
   if( PATH_TYPE_TXT == path_bin_or_txt( res_path ) ) {
      encode_binary_buffer_line( "0x00" );
   }

   encode_binary_buffer_line( "\n};\n\n" );

   encode_binary_buffer_line(
      "static const struct RESOURCE_HEADER_HANDLE gsc_resource_handle_%d[] = {\n   ",
      id );

   encode_binary_buffer_line( "gsc_resource_%d,\n   %d,\n   0",
      id, buffer_in_sz );

   encode_binary_buffer_line( "\n};\n\n" );

cleanup:

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
   fprintf( header_file, "   /* strings */\n   {\n      " );
   for( i = 0 ; TILEMAP_STRPOOL_SZ > i ; i++ ) {
      fprintf( header_file, "0x%02x, ", (unsigned char)(t->strpool[i] & 0xff) );
   }
   fprintf( header_file, "   },\n" );

#if 0
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
#endif

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

      fprintf( header_file, "         /* flags */\n" );
      fprintf( header_file, "         %d,\n", t->items[i].flags );

      fprintf( header_file, "      },\n" );
   }
   fprintf( header_file, "   },\n" );

   fprintf( header_file, "};\n\n" );

   return 1;
}

#define HEADPACK_WRITE_INCLUDES( inc ) fprintf( header, "#include \"" #inc "\"\n" );

int write_header(
   FILE* header, int paths_in_sz, char* paths_in[], int in_fmt, int out_fmt
) {
   char name_buffer[TILEMAP_NAME_MAX];
   int i = 0,
      path_iter_fname_idx = 0,
      path_iter_ext_idx = 0,
      path_iter_sz = 0,
      map_count = 0,
      retval = 0,
      cvt_retval = 0;
   struct TILEMAP t;
   FILE* file_in = NULL;
   uint8_t* buffer_file_in = NULL;
   int32_t buffer_file_sz = 0;
   struct CONVERT_GRID* grid;

   /* Output header include guard start. */
   fprintf(
      header, "#ifndef " HEADPACK_INCLUDE_GUARD "\n#define RESEMB_H\n\n" );

   HEADPACK_INCLUDES_TABLE( HEADPACK_WRITE_INCLUDES )

   /* TODO: Use dynamic path to resource header. */
   fprintf( header, "#ifdef RESOURCE_FILE\n" );
   fprintf( header, "#include \"../../unilayer/src/resource/header.h\"\n" );
   fprintf( header, "#endif /* RESOURCE_FILE */\n" );

   fprintf( header, "\n" );

   /* fprintf( header, "#include \"residx.h\"\n\n" ); */

   for( i = 0 ; paths_in_sz > i ; i++ ) {
      fprintf( header, "#define " );
      path_to_define( paths_in[i], header );
      fprintf( header, " %d\n", i );
   }

   /* Resource Data */

   /* Translation unit check start. */
   fprintf( header, "\n#ifdef " RES_C_DEF "\n\n" );

   /* Iterate through each file on the command line. */
   for( i = 0 ; paths_in_sz > i ; i++ ) {
      path_iter_sz = strlen( paths_in[i] );
      path_iter_fname_idx = dio_basename( paths_in[i], path_iter_sz );
      path_iter_ext_idx = dio_char_idx_r( paths_in[i], path_iter_sz, '.' ) + 1;
      /* TODO: Delay maps until we have all other resources so we can map tilesets. */

      if(
         'm' == paths_in[i][path_iter_fname_idx] &&
         '_' == paths_in[i][path_iter_fname_idx + 1]
      ) {
         /* This is a map JSON file, so preload it. */
         debug_printf(
            3, "encoding map: %s", &(paths_in[i][path_iter_fname_idx]) );

         retval = tilemap_json_load( paths_in[i], &t );
         if( !retval ) {
            error_printf( "unable to load tilemap: %s",
               &(paths_in[i][path_iter_fname_idx]) );
            printf( "unable to load tilemap: %s\n",
               &(paths_in[i][path_iter_fname_idx]) );
            retval = 1;
            goto cleanup;
         } else {
            retval = 0;
         }
      
         /* Trim first arg (prog) off reslist. */
         /* Second arg (header) will be ignored since 1-indexing. */
         map2h( &t, header );
      } else {
         debug_printf(
            3, "encoding resource: %s", &(paths_in[i][path_iter_fname_idx]) );
         
         #if 0
         /* Determine if file needs to be converted. */
         j = 0;
         while( 0 <= in_fmt && '\0' != gc_fmt_tokens[j][0] ) {
            if( 0 == strncmp(
               gc_fmt_tokens[j],
               &(paths_in[i][path_iter_ext_idx]),
               strlen( gc_fmt_tokens[j] )
            ) ) {
               if( j == in_fmt ) {
                  debug_printf( 3, "converting resource..." );
                  /* TODO: Handle NULL options? */
                  grid = gc_fmt_read_file_cbs[j]( paths_in[i], NULL );
                  assert( NULL != grid );
                  /* TODO: Get grid size. */
                  cvt_retval = gc_fmt_write_cbs[out_fmt](
                     buffer_in, uint32_t buf_sz,
   const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
                     , grid, &options_out );
                  assert( 0 < cvt_retval );

               }
               break;
            }
            j++;
         }
         #endif

         if( 0 > in_fmt ) {
            /* Read in resource contents. */
            debug_printf( 3, "reading resource..." );
            file_in = fopen( paths_in[i], "rb" );
            assert( NULL != file_in );
            fseek( file_in, 0, SEEK_END );
            buffer_file_sz = ftell( file_in );
            fseek( file_in, 0, SEEK_SET );
            buffer_file_in = calloc( 1, buffer_file_sz );
            fread( buffer_file_in, 1, buffer_file_sz, file_in );
            fclose( file_in );
         }

         encode_binary_buffer(
            buffer_file_in, buffer_file_sz, paths_in[i],
            i, header, in_fmt, out_fmt );
      }
   }

   /* Resource Index */

   fprintf( header, "static const struct RESOURCE_HEADER_HANDLE* gsc_resources[] = {\n" );
   for( i = 0 ; paths_in_sz > i ; i++ ) {
      if(
         'm' == paths_in[i][path_iter_fname_idx] &&
         '_' == paths_in[i][path_iter_fname_idx + 1]
      ) {
         /* Map structs are handled in the map index table below. */
      } else {
         /* Use a generic resource ID. */
         fprintf( header, "   gsc_resource_handle_%d,\n", i );
      }
   }
   fprintf( header, "};\n\n" );

   /* Map Index */

   fprintf( header, "const char gc_map_names[][TILEMAP_NAME_MAX] = {\n" );
   for( i = 0 ; paths_in_sz > i ; i++ ) {
      if(
         'm' == paths_in[i][path_iter_fname_idx] &&
         '_' == paths_in[i][path_iter_fname_idx + 1]
      ) {
         /* This is a map struct, so use its name. */

         map_count++;

         /* Create a copy without the file extension. */
         memory_zero_ptr( name_buffer, TILEMAP_NAME_MAX );
         strncpy(
            name_buffer,
            &(paths_in[i][path_iter_fname_idx + 2]),
            strlen( &(paths_in[i][path_iter_fname_idx + 2]) ) - 5 );

         fprintf( header, "   \"%s\",\n", name_buffer );
      }
   }
   fprintf( header, "};\n\n" );

   fprintf( header, "const struct TILEMAP* gc_map_structs[] = {\n" );
   for( i = 0 ; paths_in_sz > i ; i++ ) {
      if(
         'm' == paths_in[i][path_iter_fname_idx] &&
         '_' == paths_in[i][path_iter_fname_idx + 1]
      ) {
         /* This is a map struct, so use its name. */

         /* Create a copy without the file extension. */
         memory_zero_ptr( name_buffer, TILEMAP_NAME_MAX );
         strncpy(
            name_buffer,
            &(paths_in[i][path_iter_fname_idx + 2]),
            strlen( &(paths_in[i][path_iter_fname_idx + 2]) ) - 5 );

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

   return retval;
}

#ifndef HEADPACK_NOMAIN

int main( int argc, char* argv[] ) {
   int res_id = 0,
      i = 0,
      j = 0,
      read = 0,
      define_offset = 0,
      retval = 0,
      args_end = 0,
      state = 0,
      in_fmt = -1,
      out_fmt = -1;
   FILE* header = NULL;
   unsigned char byte_buffer = 0;
   char out_fname[HEADPACK_FNAME_MAX];

   if( 2 >= argc ) {
      printf( "usage: headpack <header path> <paths to files to encode>\n" );
      return 1;
   }

   memset( out_fname, '\0', HEADPACK_FNAME_MAX );

   for( i = 1 ; argc > i ; i++ ) {
      if( 0 == strncmp( "-i", argv[i], 2 ) ) {
         /* Input format arg. */
         state = HEADPACK_STATE_IN_FMT_ARG;
         args_end = i;

      } else if( 0 == strncmp( "-i", argv[i], 2 ) ) {
         /* Input format arg. */
         state = HEADPACK_STATE_OUT_FMT_ARG;
         args_end = i;

      } else if( HEADPACK_STATE_IN_FMT_ARG == state ) {
         /* Get the input format. */
         j = 0;
         while( '\0' != gc_fmt_tokens[j][0] ) {
            if( 0 == strncmp(
               argv[i], gc_fmt_tokens[j], strlen( gc_fmt_tokens[j] )
            ) ) {
               
               in_fmt = j;
               args_end = i;
               break;
            }
            j++;
         }
         state = 0;

      } else if( HEADPACK_STATE_OUT_FMT_ARG == state ) {
         /* Get the output format. */
         j = 0;
         while( '\0' != gc_fmt_tokens[j][0] ) {
            if( 0 == strncmp(
               argv[i], gc_fmt_tokens[j], strlen( gc_fmt_tokens[j] )
            ) ) {
               out_fmt = j;
               args_end = i;
               break;
            }
            j++;
         }
         state = 0;

      } else if( '\0' == out_fname[0] ) {
         /* Not an arg and we don't have an output filename, so this must be it!
          */

         strncpy( out_fname, argv[i], HEADPACK_FNAME_MAX );
         args_end = i;
      }
   }

   header = fopen( argv[1], "w" );
   assert( NULL != header );

   /* Pass the rest of the args left as input names. */
   retval = write_header( header, argc - args_end - 1, &(argv[args_end + 1]),
      in_fmt, out_fmt );

cleanup:

   fclose( header );
   
   return retval;
}

#endif /* !HEADPACK_NOMAIN */

