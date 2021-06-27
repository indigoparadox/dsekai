
#include <stdio.h>

void print_table(
   FILE* hout, const char* type, const char* name, int my, int mx, int modulus,
   int offset
) {
   int y = 0,
      x = 0;

   fprintf( hout, "static const %s huge %s[%d][%d] = {\n",
      type, name, my, mx );
   for( y = 0 ; my > y ; y++ ) {
      fprintf( hout, "   {" );
      for( x = 0 ; mx > x ; x++ ) {
         fprintf( hout, "0x%x", 
            modulus ? 6 - (((((y / 2) * mx) + x) % 4) * 2) :
            (((y / 2) * mx) + x) / 4 ) + offset;
         if( x + 1 < mx ) {
            fprintf( hout, ", " );
         }
      }
      fprintf( hout, "}" );
      if( y + 1 < my ) {
         fprintf( hout, ",\n" );
      } else {
         fprintf( hout, "\n" );
      }
   }
   fprintf( hout, "};\n\n" );
}

int main() {
   int x = 0, y = 0;
   FILE* hout = NULL;
   hout = fopen( "hout.h", "w" );

   print_table( hout, "uint16_t", "gc_offsets_cga_bytes_p1", 200, 320, 0, 0 );
   print_table( hout, "uint16_t", "gc_offsets_cga_bytes_p2", 200, 320, 0, 0 );
#if 0
   fprintf( hout, "static const uint16_t huge gc_offsets_cga_bytes_p1[100][320] = {\n" );
   for( y = 0 ; 100 > y ; y++ ) {
      fprintf( hout, "   {" );
      for( x = 0 ; 320 > x ; x++ ) {
         fprintf( hout, "0x%04x", ((y * 320) + x) / 4 );
         if( x + 1 < 320 ) {
            fprintf( hout, ", " );
         }
      }
      fprintf( hout, "}" );
      if( y + 1 < 200 ) {
         fprintf( hout, ",\n" );
      } else {
         fprintf( hout, "\n" );
      }
   }
   fprintf( hout, "};\n\n" );
#endif

   print_table( hout, "uint8_t", "gc_offsets_cga_bits_p1", 200, 320, 1, 0 );

#if 0
   fprintf( hout, "static const uint8_t huge gc_offsets_cga_bits_p1[100][320] = {\n" );
   for( y = 0 ; 100 > y ; y++ ) {
      fprintf( hout, "   {" );
      for( x = 0 ; 320 > x ; x++ ) {
         fprintf( hout, "0x%02x", (8 - (((y * 320) + x) % 4 )) * 2);
         if( x + 1 < 320 ) {
            fprintf( hout, ", " );
         }
      }
      fprintf( hout, "}" );
      if( y + 1 < 200 ) {
         fprintf( hout, ",\n" );
      } else {
         fprintf( hout, "\n" );
      }
   }
   fprintf( hout, "};\n" );
#endif

   fclose( hout );

   return 0;
}

