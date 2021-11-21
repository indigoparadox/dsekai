
#include "dsekai.h"

int32_t asn_write_int(
   uint8_t** p_buffer, int32_t* p_buffer_sz, int32_t idx, int32_t value
) {
   int32_t i = 0,
      val_sz = 0;
   
   if( value <= 0x7f ) {
      val_sz = 1;
      
   } else if( value == (value & 0xffff) ) {
      val_sz = 2;

   } else if( value == (value & 0xffffff) ) {
      val_sz = 3;

   } else if( value == (value & 0xffffffff) ) {
      val_sz = 4;

   }
   debug_printf( 1, "(offset 0x%02x) value %d (0x%02x) is %d byte(s)",
      idx, value, value, val_sz );

   /* >32-bit ints not supported. */
   assert( 0 < val_sz );

   /* Write the int type to the buffer. */
   if( 0 <= value ) {
      (*p_buffer)[idx++] = ASN_INT;
   } else {
      debug_printf( 1, "value %d (0x%02x) is negative", value, value );
      (*p_buffer)[idx++] = ASN_INT | 0x40;
   }

   /* Write the size of the int to the buffer. */
   (*p_buffer)[idx++] = val_sz;

   /* Grow the buffer if we need to. */
   if( idx + val_sz >= *p_buffer_sz ) {
      debug_printf( 2, "growing buffer from %d to %d bytes",
         *p_buffer_sz, *p_buffer_sz * 2 );
      (*p_buffer_sz) *= 2;
      *p_buffer = realloc( *p_buffer, *p_buffer_sz );
      assert( NULL != *p_buffer );
   }

   /* Write the actual value to the buffer. */
   for( i = val_sz - 1 ; 0 <= i ; i-- ) {
      /* Grab the i'th byte of the value from left and place it in the buffer.
       */
      (*p_buffer)[idx++] = ((value >> (i * 8)) & 0xff);
      debug_printf( 0, "wrote byte #%d to buffer (0x%02x)",
         i, (*p_buffer)[idx - 1] );
   }

   return idx;
}


