
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* TODO: Work this compiler into src/script.c so it can be called by tmjson.c!
 */

#include "../src/dsekai.h"

uint8_t g_flags = 0;

#define CS_FLAG_VERBOSE 1

int main( int argc, char** argv ) {
   int retval = 0;
   FILE* in_file = NULL;
   size_t in_sz = 0,
      in_read = 0,
      i = 0;
   uint8_t* in_bytes = NULL;
   struct SCRIPT_COMPILE_STATE s;
   char* filename = NULL;

   /* TODO: Use unilayer functions! */

   for( i = 1 ; argc > i ; i++ ) {
      if( 0 == strncmp( "-v", argv[i], 2 ) ) {
         g_flags |= CS_FLAG_VERBOSE;
      } else {
         assert( NULL == filename );
         filename = argv[i];
      }
   }

   if( NULL == filename ) {
      debug_printf( 3, "usage:" );
      debug_printf( 3, "%s <in_script>", argv[0] );
      goto cleanup;
   }

   /* Open file. */
   in_file = fopen( filename, "r" );
   assert( NULL != in_file );

   /* Get size. */
   fseek( in_file, 0, SEEK_END );
   in_sz = ftell( in_file );
   fseek( in_file, 0, SEEK_SET );

   /* Allocate, read. */
   in_bytes = calloc( in_sz, 1 );
   assert( NULL != in_bytes );

   in_read = fread( in_bytes, 1, in_sz, in_file );
   assert( in_read == in_sz );

   /* Parse. */
   memset( &s, '\0', sizeof( struct SCRIPT_COMPILE_STATE ) );

   for( i = 0 ; in_sz > i ; i++ ) {
      script_parse_src( in_bytes[i], &s );
   }

   /* Display bytes. */
   printf( "\n" );
   for( i = 0 ; s.action_sz > i ; i++ ) {
      printf( "%c%d", gc_sc_bytes[s.action[i].action], s.action[i].arg );
   }
   printf( "\n" );

cleanup:

   if( NULL != in_file ) {
      fclose( in_file );
   }

   if( NULL != in_bytes ) {
      free( in_bytes );
   }

   return retval;
}

