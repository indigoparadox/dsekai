
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

   assert( 1 < argc );

   retval = tilemap_json_load( argv[1], &t );
   if( !retval ) {
      retval = 1;
      goto cleanup;
   }

   retval = tilemap_asn_save( argv[2], &t );
   if( 0 > retval ) {
      error_printf( "could not write tilemap!" );
      goto cleanup;
   }

   debug_printf( 3, "%d bytes written", retval );
   retval = 0;

cleanup:

   return retval;
}

