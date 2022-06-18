
#define ENGASN_C
#include "dsekai.h"

int32_t engines_asn_save( const char* save_name, struct DSEKAI_STATE* state ) {
   MEMORY_HANDLE h_buffer = (MEMORY_HANDLE)NULL;
   int32_t idx = 0;
   uint8_t* buffer = NULL;
   struct TILEMAP* t = NULL;
   int32_t mark_seq_main = 0;

   /* Allocate save buffer. */
   h_buffer = memory_alloc( TILEMAP_ASN_SAVE_BUFFER_INITIAL_SZ, 1 );
   if( NULL == h_buffer ) {
      idx = DSEKAI_ERROR_ALLOCATE;
      error_printf( "could not allocate h_buffer!" );
      goto cleanup;
   }

   /* main seq */
   idx = asn_write_seq_start( &h_buffer, idx, &mark_seq_main );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

   /* version */
   debug_printf( 3, "(offset 0x%02x) writing engine version", idx );
   idx = asn_write_int( &h_buffer, idx, 1 );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }
   
   /* map */
   t = (struct TILEMAP*)memory_lock( state->map_handle );
   assert( NULL != t );
   idx = tilemap_asn_save( h_buffer, idx, t );

   /* End the main sequence. */
   idx = asn_write_seq_end( &h_buffer, idx, &mark_seq_main );
   if( 0 > idx ) {
      error_printf( "error" );
      idx = -1;
      goto cleanup;
   }

   /* Write the buffer to disk. */
   buffer = memory_lock( h_buffer );
   assert( NULL != buffer );
   save_write( save_name, buffer, idx );
   buffer = memory_unlock( h_buffer );

cleanup:

   if( NULL != t ) {
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   if( (MEMORY_HANDLE)NULL != h_buffer ) {
      memory_free( h_buffer );
   }

   return idx;
}

