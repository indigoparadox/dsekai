
#define HEADPACK_C
#include "headpack.h"

#define CONVERT_C
#include "../unilayer/tools/convert.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h> /* For calloc() */

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
   unsigned char* buffer_in, int buffer_in_sz, const char* res_path,
   int id, FILE* header, int in_fmt, int out_fmt
) {
   int j = 0,
      written = 0,
      total_written = 0,
      retval = 0;

   /* Create a static const in the output header to hold this blob. */
   encode_binary_buffer_line( 
      "static RES_CONST " RES_TYPE " gsc_resource_%d[] = {\n   ", id );

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
      "static RES_CONST struct RESOURCE_HEADER_HANDLE gsc_resource_handle_%d[] = {\n   ",
      id );

   encode_binary_buffer_line( "gsc_resource_%d,\n   %d,\n   0",
      id, buffer_in_sz );

   encode_binary_buffer_line( "\n};\n\n" );

cleanup:

   return total_written;
}

struct HEADPACK_DEF* headpack_get_def( const char* filename ) {
   int i = 0;

   for( i = 0 ; g_headpack_defs_sz > i ; i++ ) {
      if( g_headpack_defs[i].prefix == filename[0] && '_' == filename[1] ) {
         return &(g_headpack_defs[i]);
      }
   }

   return NULL;
}

#define HEADPACK_WRITE_INCLUDES( inc ) fprintf( header, "#include \"" #inc "\"\n" );

int write_header(
   FILE* header, int paths_in_sz, const char* paths_in[],
   int in_fmt, int out_fmt
) {
   int i = 0,
      path_iter_fname_idx = 0,
      path_iter_ext_idx = 0,
      path_iter_sz = 0,
      retval = 0,
      cvt_retval = 0;
   /* struct TILEMAP t; */
   FILE* file_in = NULL;
   uint8_t* buffer_file_in = NULL;
   int32_t buffer_file_sz = 0;
   struct CONVERT_GRID* grid;
   struct HEADPACK_DEF* writer_def = NULL;

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

   for( i = 1 ; paths_in_sz >= i ; i++ ) {
      fprintf( header, "#define " );
      path_to_define( paths_in[i - 1], header );
      fprintf( header, " %d\n", i );
   }

   fprintf( header, "\n" );
   fprintf( header, "#ifdef PLATFORM_PALM\n" );
   fprintf( header, "#define RES_CONST\n" );
   fprintf( header, "#else\n" );
   fprintf( header, "#define RES_CONST const\n" );
   fprintf( header, "#endif\n" );

   /* Resource Data */

   /* Translation unit check start. */
   fprintf( header, "\n#ifdef " RES_C_DEF "\n\n" );

   /* Iterate through each file on the command line. */
   for( i = 1 ; paths_in_sz >= i ; i++ ) {
      path_iter_sz = strlen( paths_in[i - 1] );
      path_iter_fname_idx = dio_basename( paths_in[i - 1], path_iter_sz );
      path_iter_ext_idx = 
         dio_char_idx_r( paths_in[i - 1], path_iter_sz, '.' ) + 1;
      /* TODO: Delay maps until we have all other resources so we can map tilesets. */

      writer_def =
         headpack_get_def( &(paths_in[i - 1][path_iter_fname_idx]) );

      if( NULL != writer_def ) {
         debug_printf(
            3, "encoding %s: %s",
            writer_def->type, &(paths_in[i - 1][path_iter_fname_idx]) );
         retval = writer_def->writer( paths_in[i - 1], header );

      } else {
         debug_printf(
            3,
            "encoding resource: %s", &(paths_in[i - 1][path_iter_fname_idx]) );
         
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
            file_in = fopen( paths_in[i - 1], "rb" );
            assert( NULL != file_in );
            fseek( file_in, 0, SEEK_END );
            buffer_file_sz = ftell( file_in );
            fseek( file_in, 0, SEEK_SET );
            buffer_file_in = calloc( 1, buffer_file_sz );
            fread( buffer_file_in, 1, buffer_file_sz, file_in );
            fclose( file_in );
         }

         retval = encode_binary_buffer(
            buffer_file_in, buffer_file_sz, paths_in[i - 1],
            i, header, in_fmt, out_fmt );

         /* Convert return value. */
         if( 0 < retval ) {
            retval = 0;
         } else {
            error_printf( "unable to write binary data" );
         }
      }

      if( retval ) {
         error_printf( "unable to write file" );
      }
   }

   /* Resource Index */

   fprintf(
      header,
      "static RES_CONST struct RESOURCE_HEADER_HANDLE* gsc_resources[] = {\n   NULL,\n"
   );
   for( i = 1 ; paths_in_sz >= i ; i++ ) {
      path_iter_sz = strlen( paths_in[i - 1] );
      path_iter_fname_idx = dio_basename( paths_in[i - 1], path_iter_sz );
      writer_def =
         headpack_get_def( &(paths_in[i - 1][path_iter_fname_idx]) );
      if( NULL == writer_def ) {
         /* Use a generic resource ID. */
         fprintf( header, "   gsc_resource_handle_%d,\n", i );
      }
   }
   fprintf( header, "};\n\n" );

   /* Plugin Indexes */

   for( i = 0 ; g_headpack_defs_sz > i ; i++ ) {
      if( NULL == g_headpack_defs[i].indexer ) {
         continue;
      }

      g_headpack_defs[i].indexer( paths_in, paths_in_sz, header );
   }

   /* Header Footer */

   /* Translation unit check end. */
   fprintf( header, "#endif /* " RES_C_DEF " */\n\n" );

   /* Output header include guard end. */
   fprintf( header, "#endif /* !RESEMB_H */\n\n" );

cleanup:

   return retval;
}

int headpack_register(
   char prefix, headpack_writer writer, headpack_indexer indexer,
   const char* type
) {
   memset(
      &(g_headpack_defs[g_headpack_defs_sz]),
      0, sizeof( struct HEADPACK_DEF ) );
   g_headpack_defs[g_headpack_defs_sz].prefix = prefix;
   g_headpack_defs[g_headpack_defs_sz].writer = writer;
   g_headpack_defs[g_headpack_defs_sz].indexer = indexer;
   strncpy( g_headpack_defs[g_headpack_defs_sz].type, type, HEADPACK_TYPE_MAX );
   g_headpack_defs_sz++;
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
   retval = write_header( header, argc - args_end - 1, 
      (const char**)&(argv[args_end + 1]), in_fmt, out_fmt );

cleanup:

   fclose( header );
   
   /* return retval; */
   return 0;
}

#endif /* !HEADPACK_NOMAIN */

