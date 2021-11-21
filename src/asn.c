
#include "dsekai.h"

static int8_t asn_get_int_sz( int32_t value ) {
   int8_t val_sz = -1;

   if( value <= 0x7f ) {
      /* TODO: 0x7f only for int types? */
      val_sz = 1;
      
   } else if( value == (value & 0xffff) ) {
      val_sz = 2;

   } else if( value == (value & 0xffffff) ) {
      val_sz = 3;

   } else if( value == (value & 0xffffffff) ) {
      val_sz = 4;
   }

   return val_sz;
}

static int32_t asn_raw_write_int(
   uint8_t* buffer, int32_t idx, int32_t value, int32_t value_sz
) {
   int32_t i = 0;

   assert( 0 < value_sz );

   for( i = value_sz - 1 ; 0 <= i ; i-- ) {
      /* Grab the i'th byte of the value from left and place it in the buffer.
       */
      buffer[idx++] = ((value >> (i * 8)) & 0xff);
      debug_printf( 1, "wrote byte #%d to buffer (0x%02x)",
         i, buffer[idx - 1] );
   }

   return idx;
}

static int32_t asn_ensure_buffer_sz(
   uint8_t** p_buffer, int32_t* p_buffer_sz, int32_t idx, int32_t val_sz
) {
   int32_t sz_of_sz = 1;
   uint8_t* buffer_new = NULL;

   if( 127 < val_sz ) {
      sz_of_sz = asn_get_int_sz( val_sz ) + 1; /* +1 for 0x80 | sz_of_sz */
   }

   /* Grow the buffer if we need to. */
   while( idx + val_sz + sz_of_sz >= *p_buffer_sz ) {
      debug_printf( 2, "growing buffer from %d to %d bytes",
         *p_buffer_sz, *p_buffer_sz * 2 );
      (*p_buffer_sz) *= 2;
      buffer_new = realloc( *p_buffer, *p_buffer_sz );
      if( NULL == buffer_new ) {
         error_printf( "unable to expand buffer" );

         /* We haven't written to the buffer yet, so caller can use same
          * index if there's an error.
          */
         return ASN_ERROR_UNABLE_TO_ALLOCATE;
      }
      *p_buffer = buffer_new;
   }

   return *p_buffer_sz;
}

int32_t asn_write_int(
   uint8_t** p_buffer, int32_t* p_buffer_sz, int32_t idx, int32_t value
) {
   int32_t i = 0,
      val_sz = 0;
   
   val_sz = asn_get_int_sz( value );
   if( 0 >= val_sz ) {
      error_printf( "invalid value size" );
      return ASN_ERROR_INVALID_VALUE_SZ;
   }
   debug_printf( 1, "(offset 0x%02x) value %d (0x%02x) is %d byte(s)",
      idx, value, value, val_sz );

   /* Grow the buffer if we need to. */
   if( 0 >= asn_ensure_buffer_sz( p_buffer, p_buffer_sz, idx, val_sz ) ) {
      error_printf( "unable to grow buffer" );
      return ASN_ERROR_UNABLE_TO_ALLOCATE;
   }

   /* >32-bit ints not supported. */
   assert( 0 < val_sz );

   /* Start writing to the buffer! */

   /* Write the int type to the buffer. */
   if( 0 <= value ) {
      (*p_buffer)[idx++] = ASN_INT;
   } else {
      debug_printf( 1, "value %d (0x%02x) is negative", value, value );
      (*p_buffer)[idx++] = ASN_INT | 0x40;
   }

   /* Write the size of the int to the buffer. */
   (*p_buffer)[idx++] = val_sz;

   /* Write the actual value to the buffer. */
   idx = asn_raw_write_int( *p_buffer, idx, value, val_sz );

   return idx;
}

int32_t asn_write_string(
   uint8_t** p_buffer, int32_t* p_buffer_sz, int32_t idx,
   uint8_t* source, int32_t source_sz
) {
   int i = 0;
   int8_t sz_of_sz = 1;
   int32_t source_len = 0;

   source_len = memory_strnlen_ptr( source, source_sz );

   /* Get the size of the size. */
   sz_of_sz = asn_get_int_sz( source_len );
   if( 0 >= sz_of_sz ) {
      return ASN_ERROR_INVALID_VALUE_SZ;
   }
   debug_printf( 1, "string length %d (0x%02x) takes %d bytes",
      source_len, source_len, sz_of_sz );

   /* Grow the buffer if we need to. */
   if( 0 >= asn_ensure_buffer_sz( p_buffer, p_buffer_sz, idx, source_len ) ) {
      error_printf( "unable to grow buffer" );
      return ASN_ERROR_UNABLE_TO_ALLOCATE;
   }

   /* Start writing to the buffer! */

   (*p_buffer)[idx++] = ASN_STRING;

   if( 127 < source_len ) {
      /* 0x80 | size of size, followed by size. */
      (*p_buffer)[idx++] = 0x80 | sz_of_sz;
      idx = asn_raw_write_int( *p_buffer, idx, source_len, sz_of_sz );
   } else {
      (*p_buffer)[idx++] = source_len;
   }

   for( i = idx ; idx + source_len > i ; i++ ) {
      (*p_buffer)[i] = 0;
   }
   if( 0 < source_len ) {
      memory_copy_ptr( &((*p_buffer)[idx]), source, source_len );
      idx += source_len;
   }

   return idx;
}

