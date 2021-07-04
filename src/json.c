
#define JSON_C
#include "json.h"

#include "dio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int16_t json_get_token_idx(
   const char* contents, uint16_t contents_sz,
   jsmntok_t* tokens, uint16_t tokens_sz,
   const char* buf, uint16_t tree_depth_id
) {
   int i = 0;
   int16_t tentative_child_idx = -1,
      cmp_str_as_i = 0,
      child_idx = 0;
   jsmntok_t* parent = &(tokens[tree_depth_id]);

   debug_printf( 1, "parent type is: %d", parent->type );

   if( JSMN_ARRAY == parent->type ) {
      cmp_str_as_i = dio_atoi( contents, 10 );
      debug_printf( 1, "idx as int is: %d", cmp_str_as_i );
   }

   for( i = 0 ; tokens_sz > i ; i++ ) {
      debug_printf( 1, "str %s sz %d vs  %d, %d", contents, contents_sz,
         tokens[i].end - tokens[i].start, tokens[i].size );
      if(
         (
            /* If parent is array, then string key isn't relevant. */
            JSMN_ARRAY == parent->type ||
            /* If contents is NULL, just go by parent. */
            (NULL == contents ?
            /* Finally, go by string key comparison. */
            1 : (
               0 == strncmp(
                  contents,
                  &(buf[tokens[i].start]),
                  contents_sz
               /* Also, make sure we're comparing the WHOLE token. */
               ) && (contents_sz == (tokens[i].end - tokens[i].start))
            ) )
         ) &&
         /* Always limit to children of current tree_depth_id. */
         tree_depth_id == tokens[i].parent
      ) {
         if(
            JSMN_OBJECT == parent->type &&
            JSMN_STRING == tokens[i].type
         ) {
            /* Found a string in an object, check if it's a key. */
            tentative_child_idx = json_get_token_idx(
               NULL, 0, tokens, tokens_sz, buf, i );
            if( -1 == tentative_child_idx ) {
               return i;
            } else {
               /* It's a key, return child. */
               debug_printf( 1, "redirecting to %d", tentative_child_idx );
               return tentative_child_idx;
            }
         } else if( JSMN_ARRAY == parent->type ) {
            debug_printf( 1,
               "idx %d cmp to child_idx %d", cmp_str_as_i, child_idx );
            if( cmp_str_as_i == child_idx ) {
               /* List index matches numerically. */
               return i;
            } else {
               /* Try next list item. */
               child_idx++;
               continue;
            }
         }
         /* printf( "token #%d (type %d), parent %d, start %d, end %d",
            i, tokens[i].type, tokens[i].parent, tokens[i].start,
            tokens[i].end );
         printf( "%c%c%c%c\n",
            buf[tokens[i].start],
            buf[tokens[i].start + 1],
            buf[tokens[i].start + 2],
            buf[tokens[i].start + 3] ); */
         return i;
      }
   }

   return -1;
}

void json_print_element( const char* buffer, int16_t start, int16_t end ) {
   int j = 0;

   /* Print contents. */
   for( j = start; end > j ; j++ ) {
      printf( "%c", buffer[j] );
   }
   printf( "\n" );

}

int16_t json_token_id_from_path(
   const char* path, jsmntok_t* tokens, uint16_t tokens_sz, const char* buf
) {
   int i = 0,
      path_cur_tok_start = 0,
      path_cur_tok_sz = 0;

   debug_printf( 1, "path: %s", path );

   while( path_cur_tok_start + path_cur_tok_sz < strlen( path ) ) {

      /* Find the next path token in the path. */
      path_cur_tok_start += path_cur_tok_sz + 1;
      path_cur_tok_sz = 0;
      while(
         path[path_cur_tok_start + path_cur_tok_sz] != '/' &&
         path[path_cur_tok_start + path_cur_tok_sz] != '\0'
      ) {
         path_cur_tok_sz++;
      }

      debug_printf( 1, "curtok is %d (starts at %d, %d long) (%d vs %lu) ",
         i, path_cur_tok_start, path_cur_tok_sz,
         path_cur_tok_start + path_cur_tok_sz,
         strlen( path ) );
      /*json_print_element(
         path,
         path_cur_tok_start,
         path_cur_tok_start + path_cur_tok_sz ); */

      /* Find the element that corresponds to that path token at that position.
       */
      i = json_get_token_idx(
         &(path[path_cur_tok_start]), path_cur_tok_sz,
         tokens, tokens_sz, buf, i );

      if( 0 > i ) {
         return i;
      }
   }

   return i;
}

int16_t json_int_from_path(
   const char* path, jsmntok_t* tokens, uint16_t tokens_sz, const char* buf
) {
   int id = 0;
   id = json_token_id_from_path( path, tokens, tokens_sz, buf );
   assert( id < tokens_sz );
   assert( 0 <= id );
   return dio_atoi( &(buf[tokens[id].start]), 10 );
}

