
#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef JSON_TOKEN_MAX
#define JSON_TOKEN_MAX 512
#endif /* JSON_TOKEN_MAX */

struct JSON_PARSER {
   char last_c;
   char next_token[JSON_TOKEN_MAX + 1];
   uint32_t next_token_idx;
   struct JSON_OBJECT* root;
   struct JSON_BASE* iter;
};

static struct JSON_PARSER* json_parse( char c, struct JSON_PARSER* p ) {

   switch( c ) {
   case '{':
      break;

   case '}':
      break;

   case '[':
      break;

   case ']':
      break;

   case '"':
      break;

   case ',':
      break;

   case '\t':
   case '\n':
   case '\r':
   case ' ':
      
      break;

   default:
      /* Add the token to the next token and move on. */
      p->next_token[c++] = c;
      break;
   }

   return p;
}

struct JSON_OBJECT* json_parse_buffer( const uint8_t* buf, uint32_t buf_sz ) {
   struct JSON_OBJECT* root_out = NULL;
   struct JSON_BASE* iter = NULL;
   struct JSON_PARSER parser;
   int i = 0;

   memset( &parser, '\0', sizeof( struct JSON_PARSER ) );

   root_out = calloc( 1, sizeof( struct JSON_OBJECT ) );
   assert( NULL != root_out );

   parser.root = root_out;
   parser.iter = root_out;

   for( i = 0 ; buf_sz > i ; i++ ) {
      iter = json_parse( buf[i], &parser );
   }

   return root_out;
}

struct JSON_OBJECT* json_parse_file( const char* path ) {
   FILE* json_file = NULL;
   struct JSON_OBJECT* root_out = NULL;

   /* TODO */

   return root_out;
}

