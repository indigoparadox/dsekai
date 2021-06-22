
#include "data/pak.h"
#include "data/bmp.h"
#include "data/cga.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAMEBUF_MAX 255

#define FMT_BITMAP      1
#define FMT_CGA         2

#define ENDIAN_LITTLE   'l'
#define ENDIAN_BIG      'b'

#define STATE_INFILE    1
#define STATE_OUTFILE   2
#define STATE_INBITS    3
#define STATE_OUTBITS   4
#define STATE_INENDIAN  5
#define STATE_OUTENDIAN 6
#define STATE_INFMT     7
#define STATE_OUTFMT    8
#define STATE_INW       9
#define STATE_INH       10
#define STATE_INLP      11
#define STATE_OUTLP     12

uint32_t convert_reverse_endian_32( uint32_t int_in ) {
   int i = 0;
   uint32_t int_out = 0;

   for( i = 3 ; 0 <= i ; i-- ) {
      int_out <<= 8;
      int_out |= (int_in & 0xff);
      int_in >>= 8;
   }

   return int_out;
}

void convert_print_grid( struct CONVERT_GRID* grid ) {
   size_t x = 0,
      y = 0;
   /* Display the bitmap on the console. */
   convert_printf( "\npreview:\n" );
   for( y = 0 ; grid->sz_y > y ; y++ ) {
      printf( "\n" );
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         if( 0 == grid->data[(y * grid->sz_x) + x] ) {
            printf( " ," );
         } else {
            printf( "%x,", grid->data[(y * grid->sz_x) + x] );
         }
      }
   }
   printf( "\n" );
}

void convert_print_binary( uint8_t byte_in ) {
   printf( "%d%d%d%d%d%d%d%d\n",
      byte_in & 0x80 ? 1 : 0,
      byte_in & 0x40 ? 1 : 0,
      byte_in & 0x20 ? 1 : 0,
      byte_in & 0x10 ? 1 : 0,
      byte_in & 0x08 ? 1 : 0,
      byte_in & 0x04 ? 1 : 0,
      byte_in & 0x02 ? 1 : 0,
      byte_in & 0x01 ? 1 : 0 );
}

int main( int argc, char* argv[] ) {
   int retval = 0;
   int i = 0,
      state = 0,
      fmt_in = 0,
      fmt_out = 0,
      bpp_in = 0,
      bpp_out = 0,
      w_in = 0,
      h_in = 0,
      lp_in = 0,
      lp_out = 0;
#if 0
   char
      endian_in = 'b',
      endian_out = 'l'
#endif
   char namebuf_in[NAMEBUF_MAX + 1],
      namebuf_out[NAMEBUF_MAX + 1];
   struct CONVERT_GRID* grid = NULL;

   memset( namebuf_in, '\0', NAMEBUF_MAX + 1 );
   memset( namebuf_out, '\0', NAMEBUF_MAX + 1 );

   for( i = 1 ; argc > i ; i++ ) {
      switch( state ) {
      case STATE_INFILE:
         strncpy( namebuf_in, argv[i], NAMEBUF_MAX );
         state = 0;
         break;

      case STATE_OUTFILE:
         strncpy( namebuf_out, argv[i], NAMEBUF_MAX );
         state = 0;
         break;

      case STATE_INBITS:
         bpp_in = atoi( argv[i] );
         state = 0;
         break;

      case STATE_OUTBITS:
         bpp_out = atoi( argv[i] );
         state = 0;
         break;

#if 0
      case STATE_INENDIAN:
         if( 'l' == argv[i][0] ) {
            endian_in = ENDIAN_LITTLE;
         } else if( 'b' == argv[i][0] ) {
            endian_in = ENDIAN_BIG;
         }
         state = 0;
         break;

      case STATE_OUTENDIAN:
         if( 'l' == argv[i][0] ) {
            endian_out = ENDIAN_LITTLE;
         } else if( 'b' == argv[i][0] ) {
            endian_out = ENDIAN_BIG;
         }
         state = 0;
         break;
#endif

      case STATE_INFMT:
         if( 0 == strncmp( argv[i], "bitmap", 6 ) ) {
            fmt_in = FMT_BITMAP;
         } else if( 0 == strncmp( argv[i], "cga", 3 ) ) {
            fmt_in = FMT_CGA;
#if 0
         } else if( 0 == strncmp( argv[i], "header", 6 ) ) {
            fmt_in = FMT_HEADER;
#endif
         }
         state = 0;
         break;

      case STATE_OUTFMT:
         if( 0 == strncmp( argv[i], "bitmap", 6 ) ) {
            fmt_out = FMT_BITMAP;
         } else if( 0 == strncmp( argv[i], "cga", 3 ) ) {
            fmt_out = FMT_CGA;
#if 0
         } else if( 0 == strncmp( argv[i], "header", 6 ) ) {
            fmt_out = FMT_HEADER;
#endif
         }
         state = 0;
         break;

      case STATE_INW:
         w_in = atoi( argv[i] );
         state = 0;
         break;

      case STATE_INH:
         h_in = atoi( argv[i] );
         state = 0;
         break;

      case STATE_INLP:
         lp_in = atoi( argv[i] );
         state = 0;
         break;

      case STATE_OUTLP:
         lp_out = atoi( argv[i] );
         state = 0;
         break;

      default:
         if( 0 == strncmp( argv[i], "-if", 3 ) ) {
            state = STATE_INFILE;
         } else if( 0 == strncmp( argv[i], "-of", 3 ) ) {
            state = STATE_OUTFILE;
         } else if( 0 == strncmp( argv[i], "-ib", 3 ) ) {
            state = STATE_INBITS;
         } else if( 0 == strncmp( argv[i], "-ob", 3 ) ) {
            state = STATE_OUTBITS;
         } else if( 0 == strncmp( argv[i], "-ic", 3 ) ) {
            state = STATE_INFMT;
         } else if( 0 == strncmp( argv[i], "-oc", 3 ) ) {
            state = STATE_OUTFMT;
         } else if( 0 == strncmp( argv[i], "-ie", 3 ) ) {
            state = STATE_INENDIAN;
         } else if( 0 == strncmp( argv[i], "-oe", 3 ) ) {
            state = STATE_OUTENDIAN;
         } else if( 0 == strncmp( argv[i], "-iw", 3 ) ) {
            state = STATE_INW;
         } else if( 0 == strncmp( argv[i], "-ih", 3 ) ) {
            state = STATE_INH;
         } else if( 0 == strncmp( argv[i], "-ip", 3 ) ) {
            state = STATE_INLP;
         } else if( 0 == strncmp( argv[i], "-op", 3 ) ) {
            state = STATE_OUTLP;
         }
      }
   }

   printf( "%s (fmt %d) to %s (fmt %d)\n", namebuf_in, fmt_in,
      namebuf_out, fmt_out );

   if(
      0 == strlen( namebuf_in ) ||
      0 == strlen( namebuf_out ) ||
      0 == fmt_in || 0 == fmt_out
   ) {
      fprintf( stderr, "usage:\n\n" );
      fprintf( stderr, "%s [options] -ic <in_fmt> -oc <out_fmt> -if <in_file> -of <out_file>\n", argv[0] );
      fprintf( stderr, "\noptions:\n\n" );
      fprintf( stderr, "-ic [in format]\n" );
      fprintf( stderr, "-oc [out format]\n" );
      fprintf( stderr, "\nCGA options:\n" );
      fprintf( stderr, "\nthese options only apply to raw CGA files:\n\n" );
      fprintf( stderr, "-ib [in bpp]\n" );
      fprintf( stderr, "-ob [out bpp]\n" );
      fprintf( stderr, "-iw [in width]\n" );
      fprintf( stderr, "-ih [in height]\n" );
      fprintf( stderr, "-ip [in line padding] (full-screen uses 8192)\n" );
      fprintf( stderr, "-op [out line padding]\n" );
      return 1;
   }

   switch( fmt_in ) {
   case FMT_BITMAP:
      grid = bmp_read( namebuf_in );
      break;

   case FMT_CGA:
      grid = cga_read( namebuf_in, w_in, h_in, lp_in );
      break;
   }

   if( NULL == grid ) {
      fprintf( stderr, "unable to open %s\n", namebuf_in );
      return 1;
   }

   //convert_print_grid( grid );

   switch( fmt_out ) {
   case FMT_BITMAP:
      retval = bmp_write( namebuf_out, grid, bpp_out );
   case FMT_CGA:
      retval = cga_write( namebuf_out, grid, lp_out );
      break;
   }

   //free( grid->data );
   //free( grid );

   return retval;
}

