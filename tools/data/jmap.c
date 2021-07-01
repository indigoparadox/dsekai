
#include "jmap.h"

#include "../../src/memory.h"
#include "../../src/data/dio.h"
#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JMAP_TOKENS_MAX 10240

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
      tokens_parsed = 0,
      id = 0;
   jsmn_parser parser;
   jsmntok_t tokens[JMAP_TOKENS_MAX];
   struct CONVERT_GRID* grid_out = NULL;

   grid_out = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid_out );

   jsmn_init( &parser );
   tokens_parsed = jsmn_parse(
      &parser, buffer, buffer_sz, tokens, JMAP_TOKENS_MAX );

   debug_printf( 2, "%d tokens parsed", tokens_parsed );

   /*json_children( 7, buffer, &tokens, tokens_parsed );*/

   id = json_token_id_from_path(
      "/layers/0/data/0", &(tokens[0]), tokens_parsed, buffer );
   assert( id >= 0 );
   debug_printf( 1, "found id: %d", id );

#if 0
   for( i = 0 ; tokens_parsed > i ; i++ ) {
      if( 0 != tokens[i].parent || JSMN_STRING != tokens[i].type ) {
         continue;
      }

      if(
         (0 == dio_strncmp( "version", &(buffer[tokens[i].start]), 7, '"' ) &&
         0 == tokens[i].parent &&
         JSMN_STRING == tokens[i].type) ||
         tokens[i].parent == 12707
      ) {
         printf( "token %d, parent %d, start %d, end %d\n",
            tokens[i].type, tokens[i].parent, tokens[i].start, tokens[i].end );
         printf( "%c%c%c%c\n",
            buffer[tokens[i].start],
            buffer[tokens[i].start + 1],
            buffer[tokens[i].start + 2],
            buffer[tokens[i].start + 3] );
      }
   }
#endif
}

