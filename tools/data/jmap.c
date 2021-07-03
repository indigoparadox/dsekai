
#include "jmap.h"

#include "../../src/memory.h"
#include "../../src/data/dio.h"
#include "../../src/data/json.h"

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
      tok_parsed = 0,
      tiles_count = 0,
      i = 0,
      id = 0;
   jsmn_parser parser;
   jsmntok_t tokens[JMAP_TOKENS_MAX];
   struct CONVERT_GRID* grid_out = NULL;
   char iter_path[255];

   grid_out = memory_alloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid_out );

   jsmn_init( &parser );
   tok_parsed = jsmn_parse(
      &parser, buffer, buffer_sz, tokens, JMAP_TOKENS_MAX );

   debug_printf( 2, "%d tokens parsed", tok_parsed );

   /* Load map properties. */
   grid_out->sz_x =
      json_int_from_path( "/width", tokens, tok_parsed, buffer );
   grid_out->sz_y =
      json_int_from_path( "/height", tokens, tok_parsed, buffer );
   printf( "new grid: %d x %d\n", grid_out->sz_x, grid_out->sz_y );

   tiles_count = grid_out->sz_x * grid_out->sz_y;
   grid_out->data = memory_alloc( tiles_count, 1 );
   for( i = 0 ; tiles_count > i ; i++ ) {
      /* Load tile data into the grid. */
      dio_snprintf( iter_path, 255, "/layers/0/data/%d", i );
      grid_out->data[i] = 
         json_int_from_path( iter_path, &(tokens[0]), tok_parsed, buffer );
      assert( id >= 0 );
   }
   printf( "\n" );

   return grid_out;
}

