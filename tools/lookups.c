
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define GRAPHICS_90DEG_RADS 1.5708
#define GRAPHICS_RAY_ROTATE_INC (3 * GRAPHICS_90DEG_RADS)
#define GRAPHICS_RAY_FOV 0.66

void print_table_cga_bytes(
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

int main( int argc, char* argv[] ) {
   int x = 0, y = 0;
   double dir_x_right,
      dir_y_right,
      plane_x_right,
      plane_y_right,
      delta_dist_x,
      delta_dist_y,
      dir_x,
      dir_y,
      camera_x;
   FILE* hout = NULL;

   assert( argc == 3 );

   hout = fopen( argv[2], "w" );

   if( 0 == strncmp( "cga", argv[1], 3 ) ) {
      print_table_cga_bytes(
         hout, "uint16_t", "gc_offsets_cga_bytes_p1", 200, 320, 0, 0 );
      print_table_cga_bytes(
         hout, "uint16_t", "gc_offsets_cga_bytes_p2", 200, 320, 0, 0 );
      print_table_cga_bytes(
         hout, "uint8_t", "gc_offsets_cga_bits_p1", 200, 320, 1, 0 );

   } else if( 0 == strncmp( "pov", argv[1], 3 ) ) {
      /* left */
      fprintf( hout, "\n/* left */\n\n" );

      fprintf( hout, "/* facing_x */\n%f,\n",
         0 * cos( GRAPHICS_RAY_ROTATE_INC ) -
            -1 * sin( GRAPHICS_RAY_ROTATE_INC ) );
      fprintf( hout, "/* facing_y */\n%f,\n",
         0 * sin( GRAPHICS_RAY_ROTATE_INC ) +
            -1 * cos( GRAPHICS_RAY_ROTATE_INC ) );
      fprintf( hout, "/* plane_x */\n%f,\n",
         GRAPHICS_RAY_FOV * cos( GRAPHICS_RAY_ROTATE_INC ) -
            0 * sin( GRAPHICS_RAY_ROTATE_INC ) );
      fprintf( hout, "/* plane_y */\n%f,\n",
         GRAPHICS_RAY_FOV * sin( GRAPHICS_RAY_ROTATE_INC ) +
            0 * cos( GRAPHICS_RAY_ROTATE_INC ) );

      /* right */
      fprintf( hout, "\n/* right */\n\n" );

      dir_x_right = 0 * cos( -GRAPHICS_RAY_ROTATE_INC ) -
         -1 * sin( -GRAPHICS_RAY_ROTATE_INC );
      dir_y_right = 0 * sin( -GRAPHICS_RAY_ROTATE_INC ) +
         -1 * cos( -GRAPHICS_RAY_ROTATE_INC );
      plane_x_right = GRAPHICS_RAY_FOV * cos(-GRAPHICS_RAY_ROTATE_INC ) -
         0 * sin( -GRAPHICS_RAY_ROTATE_INC );
      plane_y_right = GRAPHICS_RAY_FOV * sin( -GRAPHICS_RAY_ROTATE_INC ) +
         0 * cos( -GRAPHICS_RAY_ROTATE_INC );

      fprintf( hout, "/* facing_x */\n%f,\n", dir_x_right );
      fprintf( hout, "/* facing_y */\n%f,\n", dir_y_right );
      fprintf( hout, "/* plane_x */\n%f,\n", plane_x_right );
      fprintf( hout, "/* plane_y */\n%f,\n", plane_y_right );

      fprintf( hout, "\n/* fabs */\n\n" );
      
      for( x = 0 ; 160 > x ; x++ ) {
         /* Setup ray direction and position. */
         camera_x = 2 * x / (double)160 - 1;
         dir_x = dir_x_right + plane_x_right * camera_x;
         dir_y = dir_y_right + plane_y_right * camera_x;

         /* Set ray distance to next tile side. */
         delta_dist_x = (0 == dir_x) ? 1e30 : fabs( 1 / dir_x );
         delta_dist_y = (0 == dir_y) ? 1e30 : fabs( 1 / dir_y );
         fprintf( hout, "/* dir_x */\n%f\n", dir_x_right );
         fprintf( hout, "/* dir_y */\n%f\n", dir_y_right );
         fprintf( hout, "/* delta_dist_x */\n%f\n", delta_dist_x );
         fprintf( hout, "/* delta_dist_y */\n%f\n", delta_dist_y );
         fprintf( hout, "\n" );
      }


   } else {
      printf( "usage: lookups [cga|pov] <header.h>" );
   }

   fclose( hout );

   return 0;
}

