
#include "dsekai.h"

static int16_t tilemap_asn_parse_string(
   char* buffer, int16_t buffer_sz, const char* asn_buffer
) {
   int16_t field_sz = 0;

   if( 0x16 != asn_buffer[0] ) {
      error_printf( "invalid string type byte: %d", asn_buffer[0] );
      goto cleanup;
   }
   
   field_sz = asn_buffer[1];
   if( buffer_sz <= field_sz ) {
      error_printf( "string too long to fit in buffer: %d", field_sz );
      goto cleanup;
   }

   memory_copy_ptr( buffer, &(asn_buffer[2]), field_sz );
   debug_printf( 1, "parsed string: %s (%d)", buffer, field_sz );

cleanup:

   return field_sz;
}

int16_t tilemap_asn_load( RESOURCE_ID id, struct TILEMAP* t ) {
   char* asn_buffer = NULL;
   int16_t retval = 1,
      idx = 0;
   uint16_t read_sz = 0;
   RESOURCE_JSON_HANDLE asn_handle = (RESOURCE_JSON_HANDLE)0;
   uint32_t asn_buffer_sz = 0;

   memory_zero_ptr( t, sizeof( struct TILEMAP ) );

   /* Allocate buffers for parsing JSON. */
   asn_handle = resource_get_json_handle( id );
   if( (RESOURCE_JSON_HANDLE)0 == asn_handle ) {
      error_printf( "could not get tilemap resource handle" );
      retval = 0;
      goto cleanup;
   }

   asn_buffer = resource_lock_handle( asn_handle );
   if( NULL == asn_buffer ) {
      error_printf( "could not get tilemap resource pointer" );
      retval = 0;
      goto cleanup;
   }

   memory_zero_ptr( t, sizeof( struct TILEMAP ) );

   if( 0x30 == asn_buffer[0] ) {
      debug_printf( 3, "tilemap sequence found in resource" );
   } else {
      error_printf( "no tilemap sequence found! (found %d instead)",
         asn_buffer[0] );
      goto cleanup;
   }
   idx++;

   idx += 3; /* Skip size bytes. */

   idx += 2; /* Skip version tag. */
   debug_printf( 3, "tilemap version %d", asn_buffer[idx++] );

   /* name */
   read_sz =
      tilemap_asn_parse_string( t->name, TILEMAP_NAME_MAX, &(asn_buffer[idx]) );
   if( 0 == read_sz ) {
      goto cleanup;
   }
   debug_printf( 3, "tilemap name: %s (%d)", t->name, read_sz );

cleanup:

   if( NULL != asn_buffer ) {
      resource_unlock_handle( asn_handle );
   }

   if( NULL != asn_handle ) {
      resource_free_handle( asn_handle );
   }

   return retval;
}

