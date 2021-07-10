
#include <stdio.h>
#include <assert.h>
#include <string.h>

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

int main( int argc, char* argv[] ) {
   int res_id = 0,
      i = 0,
      j = 0,
      read = 0,
      define_offset = 0;
   FILE* header = NULL;
   FILE* bin = NULL;
   unsigned char bin_buffer[BIN_BUFFER_SZ],
      byte_buffer = 0;

   assert( 2 < argc );

   header = fopen( argv[1], "w" );
   assert( NULL != header );

   fprintf( header, "#ifndef RESEXT_H\n#define RESEXT_H\n\n" );

   for( i = 2 ; argc > i ; i++ ) {
      fprintf( header, "#define " );
      path_to_define( argv[i], header );
      fprintf( header, " %d\n", i - 2 );
   }

   /* Resource Data */

   fprintf( header, "\n#ifdef " RES_C_DEF "\n\n" );

   for( i = 2 ; argc > i ; i++ ) {
      bin = fopen( argv[i], "rb" );
      assert( NULL != bin );

      fprintf( header, "static const " RES_TYPE " gsc_resource_%d[] = {\n   ",
         i - 2 );
      while( 0 < (read = fread( bin_buffer, 1, BIN_BUFFER_SZ, bin )) ) {
         for( j = 0 ; read > j ; j++ ) {
            fprintf( header, "0x%02x, ", (unsigned char)bin_buffer[j] );
         }
      }
      if( PATH_TYPE_TXT == path_bin_or_txt( argv[i] ) ) {
         fprintf( header, "0x00" );
      }
      fprintf( header, "\n};\n\n" );
   }

   /* Resource Index */

   fprintf( header, "static const " RES_TYPE "* gsc_resources[] = {\n" );
   for( i = 2 ; argc > i ; i++ ) {
      fprintf( header, "   gsc_resource_%d,\n", i - 2 );
   }
   fprintf( header, "};\n" );

   /* Header Footer */

   fprintf( header, "\n#endif /* " RES_C_DEF
      " */\n\n#endif /* !RESEXT_H */\n\n" );

   fclose( header );
   
   return 0;
}

