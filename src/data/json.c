
#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct JSON_BASE* 
json_parse( char c, struct JSON_OBJECT* root, struct JSON_BASE* iter ) {
   

   return iter;
}

struct JSON_OBJECT* json_parse_buffer(
   const uint8_t* buffer, uint32_t buffer_sz
) {
   struct JSON_OBJECT* root_out = NULL;
   struct JSON_BASE* iter = NULL;
   int i = 0;

   root_out = calloc( 1, sizeof( struct JSON_OBJECT ) );
   assert( NULL != root_out );

   for( i = 0 ; buffer_sz > i ; i++ ) {
      iter = json_parse( buffer[i], root_out, iter );
   }

   return root_out;
}

struct JSON_OBJECT* json_parse_file( const char* path ) {
   FILE* json_file = NULL;
   struct JSON_OBJECT* root_out = NULL;

   /* TODO */

   return root_out;
}

