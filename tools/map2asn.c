
#define ENGINES_TOKENS_ONLY
#define ENGINES_C
#define ITEM_C
#define CROP_C
#define MOBILE_C
#define TILEMAP_C
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
   struct TILEMAP* t = NULL;
   int retval = 0,
      idx = 0;
   MEMORY_HANDLE h_buffer = (MEMORY_HANDLE)NULL,
      h_t = (MEMORY_HANDLE)NULL;
   uint8_t* buffer = NULL;

   debug_printf( 3, "map2asn compiled " __DATE__ __TIME__ );
   debug_printf( 3, "git hash: " GIT_HASH );

   if( 3 > argc ) {
      error_printf( "usage:" );
      error_printf( "%s <in_json> <out_asn>", argv[0] );
      error_printf( "in_json - JSON tilemap file to convert to ASN.1." );
      error_printf( "out_asn - ASN.1 filename to create." );
      retval = 1;
      goto cleanup;
   }

   h_t = memory_alloc( sizeof( struct TILEMAP ), 1 );
   if( (MEMORY_HANDLE)NULL == h_t ) {
      error_printf( "could not allocate tilemap!" );
      retval = 1;
      goto cleanup;
   }

   t = memory_lock( h_t );
   assert( NULL != t );

   retval = tilemap_json_load( argv[1], t );
   if( !retval ) {
      retval = 1;
      goto cleanup;
   }
   retval = 0;

   h_buffer = memory_alloc( TILEMAP_ASN_SAVE_BUFFER_INITIAL_SZ, 1 );

   idx = tilemap_asn_save( h_buffer, 0, t );
   if( 0 > idx ) {
      error_printf( "could not write tilemap!" );
      goto cleanup;
   }

   /* Write the ASN map file to disk. */
   save_write( argv[2], h_buffer, idx );

   debug_printf( 3, "%d bytes written", idx );

cleanup:

   if( NULL != t ) {
      t = memory_unlock( h_t );
   }

   if( (MEMORY_HANDLE)NULL != h_t ) {
      memory_free( h_t );
   }

   if( NULL != buffer ) {
      buffer = memory_unlock( h_buffer );
   }

   if( (MEMORY_HANDLE)NULL != h_buffer ) {
      memory_free( h_buffer );
   }

   return retval;
}

