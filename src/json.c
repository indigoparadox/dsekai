
#define JSON_C
#include "dstypes.h"

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

   debug_printf( 0, "(%u) parent type is: %d", tree_depth_id, parent->type );

   debug_printf( 1, "(%u) path spec: %s", tree_depth_id, contents );

   if( NULL != contents && JSMN_ARRAY == parent->type ) {
      cmp_str_as_i = dio_atoi( contents, 10 );
      debug_printf( 0, "(%u) idx as int is: %d", tree_depth_id, cmp_str_as_i );
   }

   for( i = 0 ; tokens_sz > i ; i++ ) {
      if( NULL != contents ) {
         debug_printf( 0, "(%u) str %s sz %d vs  %d, %d",
            tree_depth_id, contents, contents_sz,
            tokens[i].end - tokens[i].start, tokens[i].size );
      }
      if(
         (
            /* If parent is array, then string key isn't relevant. */
            JSMN_ARRAY == parent->type ||
            /* If contents is NULL, just go by parent. */
            (NULL == contents ?
            /* Finally, go by string key comparison. */
            1 : (
               0 == memory_strncmp_ptr(
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
               debug_printf( 1, "(%u) no tentative child found",  
                  tree_depth_id );
               return i;
            } else {
               /* It's a key, return child. */
               debug_printf( 1, "(%u) redirected to %d",
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
   jsmntok_t* tokens, uint16_t tokens_sz, const char* buf
) {
   int16_t i = 0,
      path_cur_tok_start = 0,
      path_cur_tok_sz = 0;

   debug_printf( 1, "path: %s", path );

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
   jsmntok_t* tokens, uint16_t tokens_sz, const char* buf
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
   jsmntok_t* tokens, uint16_t tokens_sz, const char* buf
) {
   int16_t out = 0,
      id = 0,
      excerpt_sz = 0;
   char* offset_buf = NULL;

   debug_printf( 1, "fetching JSON path %s...", path );

   id = json_token_id_from_path( path, path_sz, tokens, tokens_sz, buf );
   if( 0 > id ) {
      return id;
   }
   
   assert( id < tokens_sz );
   assert( 0 <= id );

   offset_buf = &(buf[tokens[id].start]);
   excerpt_sz = tokens[id].end - tokens[id].start;
   if( buffer_sz <= excerpt_sz ) {
      error_printf( "insufficient buffer length (need %d)", excerpt_sz );
      return 0;
   }
   memory_copy_ptr( buffer, &(buf[tokens[id].start]), excerpt_sz );
   return excerpt_sz;
}

