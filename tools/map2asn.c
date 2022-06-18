
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define ENGINES_TOKENS_ONLY
#define ENGINES_C
#include "../src/dsekai.h"

#include "../src/tmjson.h"
#include "../src/tmasn.h"

void buffer_assign_short( uint8_t* buffer, uint16_t n ) {
   buffer[0] = ((n & 0xff00) >> 8);
   buffer[1] = (n & 0x00ff);
}

int main( int argc, char* argv[] ) {
   FILE* map_file = NULL,
      * asn_file = NULL;
   struct TILEMAP t;
   int retval = 0,
      idx = 0;
   MEMORY_HANDLE h_buffer = (MEMORY_HANDLE)NULL;
   uint8_t* buffer = NULL;

   assert( 1 < argc );

   retval = tilemap_json_load( argv[1], &t );
   if( !retval ) {
      retval = 1;
      goto cleanup;
   }
   retval = 0;

   h_buffer = memory_alloc( TILEMAP_ASN_SAVE_BUFFER_INITIAL_SZ, 1 );

   idx = tilemap_asn_save( h_buffer, 0, &t );
   if( 0 > idx ) {
      error_printf( "could not write tilemap!" );
      goto cleanup;
   }

   buffer = memory_lock( h_buffer );

   /* Write the ASN map file to disk. */
   save_write( argv[2], buffer, idx );

   buffer = memory_unlock( h_buffer );

   debug_printf( 3, "%d bytes written", idx );

cleanup:

   if( (MEMORY_HANDLE)NULL != h_buffer ) {
      memory_free( h_buffer );
   }

   return retval;
}

