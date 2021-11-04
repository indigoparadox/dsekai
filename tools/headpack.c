
#include "headpack.h"

#include <assert.h>
#include <string.h>

#include "../src/dsekai.h"

#include "../src/tmjson.h"

#include "map2h.h"

#define BIN_BUFFER_SZ 1024
#define RES_C_DEF "RESOURCE_C"
#define RES_TYPE "unsigned char"

#define PATH_TYPE_BIN 0
#define PATH_TYPE_TXT 1

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
      path_sz = 0;

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
   }

   if( 0 < last_sep ) {
      return last_sep + 1;
   } else {
      return 0;
   }
}

void encode_generic_file( char* path, int id, FILE* header ) {
   unsigned char bin_buffer[BIN_BUFFER_SZ];
   int read = 0,
      j = 0;
   FILE* bin = NULL;

   bin = fopen( path, "rb" );
   if( NULL == bin ) {
      error_printf( "unable to open resource: %s", path );
   }

   /* Create a static const in the output header to hold this blob. */
   fprintf( header, "static const " RES_TYPE " gsc_resource_%d[] = {\n   ",
      /* Subtract 1 since this is argv (program name and output header args),
       * but we index from 1. */
      id - 1 );

   fprintf( header, "   /* %s */\n", path );
   
   /* Translate each byte into a hex number in the output header. */
   while( 0 < (read = fread( bin_buffer, 1, BIN_BUFFER_SZ, bin )) ) {
      for( j = 0 ; read > j ; j++ ) {
         fprintf( header, "0x%02x, ", (unsigned char)bin_buffer[j] );
      }
   }

   /* Add a null terminator. */
   if( PATH_TYPE_TXT == path_bin_or_txt( path ) ) {
      fprintf( header, "0x00" );
   }

   fprintf( header, "\n};\n\n" );

cleanup:

   if( NULL != bin ) {
      fclose( bin );
   }

   return;
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

   /* TODO: Implement separate map index that can be searched by name. */

   assert( 2 < argc );

   memset( &t, '\0', sizeof( struct TILEMAP ) );

   header = fopen( argv[1], "w" );
   assert( NULL != header );

   /* Output header include guard start. */
   fprintf( header, "#ifndef RESEMB_H\n#define RESEMB_H\n\n" );

   fprintf( header, "#include \"../../src/tmstruct.h\"\n\n" );

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

   /* Filename Index */

   #if 0
   fprintf( header, "static const char* gsc_resource_names[] = {\n" );
   fprintf( header, "   NULL,\n" );
   for( i = 2 ; argc > i ; i++ ) {
      if(
         'm' == argv[i][path_iter_fname_idx] &&
         '_' == argv[i][path_iter_fname_idx + 1]
      ) {
         /* This is a map JSON file. */
      } else {
         /* Use a generic resource ID. */
         fprintf( header, "   \"%s\",\n", argv[i] );
      }
   }
   fprintf( header, "};\n\n" );
   #endif

   /* Resource Index */

   fprintf( header, "static const " RES_TYPE "* gsc_resources[] = {\n" );
   fprintf( header, "   NULL,\n" );
   for( i = 2 ; argc > i ; i++ ) {
      if(
         'm' == argv[i][path_iter_fname_idx] &&
         '_' == argv[i][path_iter_fname_idx + 1]
      ) {
         /* Map structs are handled in the map index table below. */
      } else {
         /* Use a generic resource ID. */
         fprintf( header, "   gsc_resource_%d,\n", i - 1 );
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

