
#include "psprintf.h"

#include <string.h>

int psitoa( char* buffer, int buffer_len, int d, int d_base ) {
   int idx = 0,
      tmp_idx = 0,
      d_rem = 0,
      d_negative = 0;
   char buffer_tmp[PSITOA_BUF_LEN + 1];

   memset( buffer_tmp, '\0', PSITOA_BUF_LEN + 1 );

   if( 0 == d && tmp_idx + 1 ) {
      buffer[idx++] = '0';
      return idx;
   }

   if( 10 == d_base && 0 > d ) {
      buffer[idx++] = '-';
      d = -d;
      d_negative = 1;
   }

   while( 0 != d && idx + 1 < buffer_len ) {
      d_rem = d % d_base;
      buffer_tmp[tmp_idx++] = 9 < d_rem ? 'a' + (d_rem - 10) : '0' + d_rem;
      d /= d_base;
   }

   if( d_negative && tmp_idx < PSITOA_BUF_LEN ) {
      buffer_tmp[tmp_idx++] = '-';
   }

   if( 0 < tmp_idx ) {
      tmp_idx--;
   }
   
   while(
      0 <= tmp_idx &&
      idx + 1 < buffer_len &&
      '\0' != buffer_tmp[tmp_idx]
   ) {
      buffer[idx++] = buffer_tmp[tmp_idx--];
   }

   return idx;
}

int psprintf( char* buffer, int buffer_len, const char* fmt, ... ) {
   va_list args;
   unsigned char c = '\0',
      last = '\0';
   int d = 0,
      idx_in = 0,
      idx_out = 0;

   /* Quick and dirty debug string formatting function. */

   va_start( args, fmt );

   for( idx_in = 0 ; '\0' != fmt[idx_in] && idx_out < buffer_len ; idx_in++ ) {
      c = fmt[idx_in];

      if( '%' == last ) {
         switch( c ) {
         case 'd':
            d = va_arg( args, int );
            idx_out +=
               psitoa( &(buffer[idx_out]), buffer_len - idx_out, d, 10 );
            break;

         case 'x':
            d = va_arg( args, int );
            idx_out +=
               psitoa( &(buffer[idx_out]), buffer_len - idx_out, d, 16 );
            break;
         }
      } else if( '%' != c ) {
         buffer[idx_out++] = c;
      }

      last = c;
   }

   buffer[idx_out++] = '\0';

   va_end( args );

   return idx_out;
}


