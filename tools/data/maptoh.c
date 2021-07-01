
#include "maptoh.h"

#define JSMN_HEADER
#include "jsmn.h"

#include <stdio.h>
#include <stdlib.h>

#define MAPTOH_TOKENS_MAX 10240

int header_map_write_file(
   const char* path_out, const char* path_in, struct CONVERT_OPTIONS* o
) {
   int retval = 0,
      file_in_sz = 0,
      read = 0;
   FILE* file_in = NULL,
      * file_out = NULL;
   char* buffer_in = NULL;

   file_in = fopen( path_in, "r" );
   fseek( file_in, 0, SEEK_END );
   file_in_sz = ftell( file_in );

   fseek( file_in, 0, SEEK_SET );
   buffer_in = calloc( 1, file_in_sz );
   assert( NULL != buffer_in );

   read = fread( buffer_in, 1, file_in_sz, file_in );
   assert( read == file_in_sz );

   fclose( file_in );

   header_map_write( NULL, 0, buffer_in, file_in_sz, o );

   return retval;
}

int header_map_write(
   uint8_t* buf_out, uint32_t buf_out_sz,
   uint8_t* buf_in, uint32_t buf_in_sz, struct CONVERT_OPTIONS* o
) {
   int retval = 0,
      tokens_parsed = 0;
   jsmn_parser parser;
   jsmntok_t tokens[MAPTOH_TOKENS_MAX];

   jsmn_init( &parser );
   tokens_parsed = jsmn_parse(
      &parser, buf_in, buf_in_sz, tokens, MAPTOH_TOKENS_MAX );

   return retval;
}

