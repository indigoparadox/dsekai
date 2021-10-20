
#define JSON_C
#include "dsekai.h"

int16_t json_load(
   char* json_buffer, uint16_t json_buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz
) {
   int16_t tok_parsed = 0;
   jsmn_parser parser;

   /* TODO: Move this over to json.c and dissolve this function. */
   if( '{' != json_buffer[0] ) {
      error_printf( "invalid json (must start with '{') found: %s",
         json_buffer );
      return 0;
   }

   jsmn_init( &parser );
   tok_parsed = jsmn_parse(
      &parser, json_buffer, json_buffer_sz, tokens, tokens_sz );

   /* TODO: Move this over to json.c and dissolve this function. */
   debug_printf( 2, "%d tokens parsed", tok_parsed );
   return tok_parsed;
}

int16_t json_get_token_idx(
   const char* path, uint16_t path_sz,
   struct jsmntok* tokens, uint16_t tokens_sz,
   const char* buf, uint16_t tree_depth_id
) {
   int i = 0;
   int16_t tentative_child_idx = -1,
      cmp_str_as_i = 0,
      child_idx = 0,
      path_cmp_len = 0,
      path_cmp_start = 0;
   struct jsmntok* parent = &(tokens[tree_depth_id]);

   debug_printf( 0, "(%u) parent type is: %d", tree_depth_id, parent->type );

   debug_printf( 0, "(%u) path spec: %s", tree_depth_id, path );

   if( NULL != path && JSMN_ARRAY == parent->type ) {
      cmp_str_as_i = dio_atoi( path, 10 );
      debug_printf( 0, "(%u) idx as int is: %d", tree_depth_id, cmp_str_as_i );
   }

   for( i = 0 ; tokens_sz > i ; i++ ) {
      if( NULL != path ) {
         debug_printf( 0, "(%u) str %s sz %d vs  %d, %d",
            tree_depth_id, path, path_sz,
            tokens[i].end - tokens[i].start, tokens[i].size );
      }
      if(
         /* If we're searching by sibling value. */
         NULL != path && '[' == path[0] &&
         /* If this token is subordinate to the current search root. */
         tokens[i].parent == tree_depth_id &&
         /* If this token is a type with children. */
         (JSMN_ARRAY == tokens[i].type || JSMN_OBJECT == tokens[i].type)
      ) {
         /* Up to position of = or ], else just the end of the path. */
         path_cmp_len = 
            dio_strnchr( path, dio_strnchr( path, path_sz, ']' ) - 1, '=' ) - 1;

         if( 0 > path_cmp_len ) {
            error_printf( "no = in token comparison" );
            return path_cmp_len;
         }

         /* Search for the left of the comparison. */
         tentative_child_idx = json_get_token_idx(
            &(path[1]), path_cmp_len, tokens, tokens_sz, buf, i );

         if( 0 <= tentative_child_idx ) {
            path_cmp_start = dio_strnchr( path, path_sz, '=' ) + 1;
            path_cmp_len = dio_strnchr( &(path[path_cmp_start]), path_sz, ']' );
            debug_printf( 0, "cmp starts at %d and ends %d past that",
               path_cmp_start, path_cmp_len );
            debug_printf( 0, 
               "comparing %d chars of path %s to subchild: %d: %s",
               path_cmp_len,
               &(path[path_cmp_start]),
               tentative_child_idx,
               &(buf[tokens[tentative_child_idx].start]) );
            
            /* Compare the right of the comparison. */
            if( 0 == memory_strncmp_ptr(
               &(path[path_cmp_start]),
               &(buf[tokens[tentative_child_idx].start]),
               path_cmp_len
            ) ) {
               debug_printf( 0, "token %d matches", i );
               return i;
            }
         }
      } else if(
         (
            /* If parent is array, then string key isn't relevant. */
            JSMN_ARRAY == parent->type ||
            /* If path is NULL, just go by parent. */
            (NULL == path ?
            /* Finally, go by string key comparison. */
            1 : (
               0 == memory_strncmp_ptr(
                  path,
                  &(buf[tokens[i].start]),
                  path_sz
               /* Also, make sure we're comparing the WHOLE token. */
               ) && (path_sz == (tokens[i].end - tokens[i].start))
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
               debug_printf( 1, "(%u) no tentative child found",  
                  tree_depth_id );
               return i;
            } else {
               /* It's a key, return child. */
               debug_printf( 0, "(%u) redirected to %d",
                  tree_depth_id, tentative_child_idx );
               return tentative_child_idx;
            }
         } else if( JSMN_ARRAY == parent->type ) {
            debug_printf( 0,
               "(%u) idx %d cmp to child_idx %d",
                  tree_depth_id, cmp_str_as_i, child_idx );
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

int16_t json_token_id_from_path(
   const char* path, uint16_t path_sz,
   struct jsmntok* tokens, uint16_t tokens_sz, const char* buf
) {
   int16_t i = 0,
      path_cur_tok_start = 0,
      path_cur_tok_sz = 0;

   debug_printf( 0, "path: %s", path );

   while(
      path_cur_tok_start + path_cur_tok_sz < memory_strnlen_ptr( path, path_sz )
   ) {

      /* Find the next path token in the path. */
      path_cur_tok_start += path_cur_tok_sz + 1;
      path_cur_tok_sz = 0;
      while(
         path[path_cur_tok_start + path_cur_tok_sz] != '/' &&
         path[path_cur_tok_start + path_cur_tok_sz] != '\0'
      ) {
         path_cur_tok_sz++;
      }

      debug_printf( 0, "cur_path_tok is %d (starts at %d, %d long) (%d vs %d) ",
         i, path_cur_tok_start, path_cur_tok_sz,
         path_cur_tok_start + path_cur_tok_sz,
         memory_strnlen_ptr( path, path_sz ) );

      /* Find the element that corresponds to that path token at that position.
       */
      i = json_get_token_idx(
         &(path[path_cur_tok_start]), path_cur_tok_sz,
         tokens, tokens_sz, buf, i );

      if( 0 > i ) {
         error_printf( "could not find %s", path );
         return i;
      }
   }

   return i;
}

int16_t json_int_from_path(
   const char* path, uint16_t path_sz,
   struct jsmntok* tokens, uint16_t tokens_sz, const char* buf
) {
   int16_t out = 0,
      id = 0;
   char* offset_buf = NULL;

   id = json_token_id_from_path( path, path_sz, tokens, tokens_sz, buf );
   if( 0 > id ) {
      return id;
   }
   
   assert( id < tokens_sz );
   assert( 0 <= id );

   offset_buf = &(buf[tokens[id].start]);
   out = dio_atoi( offset_buf, 10 );
   return out;
}

int16_t json_str_from_path(
   const char* path, uint16_t path_sz,
   char* buffer, uint16_t buffer_sz,
   struct jsmntok* tokens, uint16_t tokens_sz, const char* buf
) {
   int16_t id = 0,
      excerpt_sz = 0;

   debug_printf( 1, "fetching JSON path %s...", path );

   id = json_token_id_from_path( path, path_sz, tokens, tokens_sz, buf );
   if( 0 > id ) {
      return id;
   }
   
   assert( id < tokens_sz );
   assert( 0 <= id );

   excerpt_sz = tokens[id].end - tokens[id].start;
   if( buffer_sz <= excerpt_sz ) {
      error_printf( "insufficient buffer length (need %d)", excerpt_sz );
      return 0;
   }
   memory_copy_ptr( buffer, &(buf[tokens[id].start]), excerpt_sz );

   /* Enforce NULL termination. */
   buffer[excerpt_sz] = '\0';

   return excerpt_sz;
}

