
#include "header.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../src/dio.h"
#include "../convert.h"

int header_img_write_file(
   const char* path, const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   FILE* header_file = NULL;
   int retval = 0,
      i = 0,
      extension_idx = 0,
      path_sz = 0,
      basename_sz = 0,
      basename_start = 0,
      bit_idx = 0,
      byte_idx = 0;
   uint8_t byte_buffer = 0;
   char* basename = NULL;

   header_file = fopen( path, "w" );
   assert( NULL != header_file );

   path_sz = strlen( path );

   /* Remove the path of the basename during copy. */
   basename_start = dio_basename( path, path_sz  );
   basename_sz = (path_sz - basename_start);
   basename = calloc( basename_sz + 1, 1 );
   strncpy( basename, &(path[basename_start]), basename_sz );

   /* Remove the extension of the basename. */
   extension_idx = dio_char_idx_r( basename, basename_sz, '.' );
   if( 0 < extension_idx ) {
      /* Turn the . into a NULL to chop off the extension. */
      basename[extension_idx] = '\0';
   }

   fprintf( header_file, "#ifndef %s_h\n#define %s_h\n\n", basename, basename );

   fprintf( header_file, "#ifdef MASKS_C\n\n" );

   fprintf( header_file, "const uint32_t %s_sz = %u;\n\n", basename,
      grid->data_sz);

   fprintf( header_file, "const uint8_t %s[%u] = {\n",
      basename, grid->data_sz / 8 );

   for( i = 0 ; grid->data_sz > i ; i++ ) {

      printf( "x%02d, y%02d, byte %02d, bit %02d\n",
         i % grid->sz_x, i / grid->sz_x, byte_idx, bit_idx );
      assert( byte_idx < grid->data_sz / 8 );

      byte_buffer <<= 1;
      byte_buffer |= grid->data[i];

      if( 0 == bit_idx % 7 && 0 != bit_idx ) {
         bit_idx = 0;
         byte_idx++;
         fprintf( header_file, "0x%02x", byte_buffer );

         if( grid->data_sz - 1 <= i ) {
            fprintf( header_file, "\n" );
         } else if( 0 == (i + 1) % 64 )  {
            printf( "%d\n", i+1 );
            fprintf( header_file, ",\n" );
         } else {
            fprintf( header_file, ", " );
         }
      } else {
         bit_idx++;
      }
   }

   fprintf( header_file, "};\n\n" );

   fprintf( header_file, "#else\n\n" );

   fprintf( header_file, "extern const uint32_t %s_sz;\n\n", basename );
   fprintf( header_file, "extern const uint8_t* %s;\n\n", basename );

   fprintf( header_file, "#endif /* MASKS_C */\n\n" );

   fprintf( header_file, "#endif /* %s_h */\n\n", basename );

   fclose( header_file );

   return retval;
}

int header_img_write(
   uint8_t* buffer, uint32_t buffer_sz,
   const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   /* TODO */
}

