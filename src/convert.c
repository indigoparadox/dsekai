
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
#define STATE_INPP      5
#define STATE_OUTPP     6
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
   printf( "bin: %d%d%d%d%d%d%d%d\n",
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
      bpp_in = 2, /* Default to 2-bit CGA. */
      bpp_out = 0,
      in_w = 0,
      in_h = 0,
      lp_in = 0,
      lp_out = 0,
      pp_in = 0,
      pp_out = 0;
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
         in_w = atoi( argv[i] );
         state = 0;
         break;

      case STATE_INH:
         in_h = atoi( argv[i] );
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

      case STATE_INPP:
         pp_in = atoi( argv[i] );
         state = 0;
         break;

      case STATE_OUTPP:
         pp_out = atoi( argv[i] );
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
         } else if( 0 == strncmp( argv[i], "-iw", 3 ) ) {
            state = STATE_INW;
         } else if( 0 == strncmp( argv[i], "-ih", 3 ) ) {
            state = STATE_INH;
         } else if( 0 == strncmp( argv[i], "-ip", 3 ) ) {
            state = STATE_INPP;
         } else if( 0 == strncmp( argv[i], "-op", 3 ) ) {
            state = STATE_OUTPP;
         } else if( 0 == strncmp( argv[i], "-il", 3 ) ) {
            state = STATE_INLP;
         } else if( 0 == strncmp( argv[i], "-ol", 3 ) ) {
            state = STATE_OUTLP;
         }
      }
   }

   printf( "%s (fmt %d) to %s (fmt %d)\n", namebuf_in, fmt_in,
      namebuf_out, fmt_out );

   if(
      0 == strlen( namebuf_in ) ||
      0 == strlen( namebuf_out ) ||
      0 == fmt_in || 0 == fmt_out ||
      (FMT_CGA == fmt_in && (0 == in_w || 0 == in_h || 0 == pp_out))
   ) {
      fprintf( stderr, "usage:\n\n" );
      fprintf( stderr, "%s [options] -ic <in_fmt> -oc <out_fmt> -if <in_file> -of <out_file>\n", argv[0] );
      fprintf( stderr, "\noptions:\n\n" );
      fprintf( stderr, "-ic [in format]\n" );
      fprintf( stderr, "-oc [out format]\n" );
      fprintf( stderr, "\nCGA options:\n" );
      fprintf( stderr, "\nthese options only apply to raw CGA files:\n\n" );
      fprintf( stderr, "-ib [in bpp] (defaults to 2)\n" );
      fprintf( stderr, "-ob [out bpp] (defaults to input bpp)\n" );
      fprintf( stderr, "-iw [in width] (requried for CGA in)\n" );
      fprintf( stderr, "-ih [in height] (required for CGA in)\n" );
      fprintf( stderr, "-il [in line padding] (full-screen uses 192)\n" );
      fprintf( stderr, "-ol [out line padding]\n" );
      fprintf( stderr, "-ip [in plane padding] (full-screen uses 8000)\n" );
      fprintf( stderr, "-op [out plane padding]\n" );
      return 1;
   }

   if( 0 == bpp_in && FMT_CGA == fmt_in ) {
      bpp_out = 2;
   }

   switch( fmt_in ) {
   case FMT_BITMAP:
      grid = bmp_read( namebuf_in );
      break;

   case FMT_CGA:
      grid = cga_read( namebuf_in, in_w, in_h, bpp_in, pp_in, lp_in );
      break;
   }

   if( NULL == grid ) {
      fprintf( stderr, "unable to open %s\n", namebuf_in );
      return 1;
   }

   if( 0 == bpp_out ) {
      /* Default to grid BPP. */
      bpp_out = grid->bpp;
   }

   //convert_print_grid( grid );

   switch( fmt_out ) {
   case FMT_BITMAP:
      retval = bmp_write( namebuf_out, grid, bpp_out );
      break;
   case FMT_CGA:
      retval = cga_write( namebuf_out, grid, bpp_out, pp_out, lp_out );
      break;
   }

   free( grid->data );
   free( grid );

   return retval;
}

