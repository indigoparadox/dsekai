
#include "jmap.h"

#include "../../src/memory.h"
#include "../../src/data/dio.h"

#define JSMN_HEADER
#include "jsmn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JMAP_TOKENS_MAX 512

struct CONVERT_GRID* jmap_read_file(
   const char* path, struct CONVERT_OPTIONS* o
) {
   uint8_t* jmap_buffer = NULL;
   uint32_t jmap_buffer_sz = 0;
   struct CONVERT_GRID* grid_out = NULL;

   jmap_buffer_sz = dio_read_file( path, &jmap_buffer );
   assert( jmap_buffer_sz == strlen( (char*)jmap_buffer ) );

   grid_out = jmap_read( jmap_buffer, jmap_buffer_sz, o );

   memory_free( &jmap_buffer );

   return grid_out;
}

struct CONVERT_GRID* jmap_read(
   const uint8_t* buffer, uint32_t buffer_sz, struct CONVERT_OPTIONS* o
) {
   int retval = 0,
      tokens_parsed = 0;
   jsmn_parser parser;
   jsmntok_t tokens[JMAP_TOKENS_MAX];
   struct CONVERT_GRID* grid_out = NULL;

   grid_out = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid_out );

   jsmn_init( &parser );
   tokens_parsed = jsmn_parse(
      &parser, buffer, buffer_sz, tokens, JMAP_TOKENS_MAX );

}

