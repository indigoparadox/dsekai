
#include "dsekai.h"

int16_t strpool_get_sz( const char* str_in ) {
   int16_t int_out = 0;

   int_out |= (str_in[1] & 0xff);
   int_out <<= 8;
   int_out |= (str_in[0] & 0xff);

   return int_out;
}

void strpool_init( char* strpool, int16_t strpool_sz ) {

   maug_mzero( strpool, strpool_sz );
   strpool[0] = ((strpool_sz - 2) & 0xff);
   strpool[1] = (((strpool_sz - 2) >> 8) & 0xff);
   debug_printf( 3, "strpool initialized as %d bytes", strpool_sz );

   assert( strpool_sz - 2 == strpool_get_sz( strpool ) );
}

int16_t strpool_add_string( char* strpool, char* string, int16_t string_sz ) {
   int16_t i = 0,
      str_len = 0,
      strpool_sz = 0;

   strpool_sz = strpool_get_sz( strpool );
   i += 2; /* Move past strpool size. */

   while( '\0' != strpool[i] ) {
      str_len = strpool_get_sz( &(strpool[i]) );
      i += 2; /* Move past length. */
      i += str_len; /* Move past string. */
   }

   if( string_sz + 1 + i + 2 >= strpool_sz ) {
      error_printf( "strpool size exceeded (%d out of %d)!",
         string_sz + 1 + i + 2, strpool_sz );
      return -1;
   }

   /* Store LSB first so zeros are easier to detect. */
   strpool[i] = ((string_sz + 1) & 0xff);
   strpool[i + 1] = (((string_sz + 1) >> 8) & 0xff);
   memcpy( &(strpool[i + 2]), string, string_sz );

   return i;
}

const char* strpool_get( const char* strpool, int16_t idx, int16_t* sz_out ) {
   int16_t i = 0,
      str_len = 0,
      idx_iter = 0,
      strpool_sz = 0;

   strpool_sz = strpool_get_sz( strpool );
   i += 2; /* Move past strpool size. */

   while( idx_iter < idx && i < strpool_sz ) {
      str_len = strpool_get_sz( &(strpool[i]) );
      i += 2; /* Move past length. */
      i += str_len; /* Move past string. */
      idx_iter++;
   }

   if( idx_iter != idx ) {
      return NULL;
   }

   if( NULL != sz_out ) {
      *sz_out = strpool_get_sz( &(strpool[i]) );
   }

   i += 2; /* Move past string size. */

   return &(strpool[i]);
}

