
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

int main( int argc, char* argv[] ) {
   int retval = 0;
   int i = 0,
      state = 0,
      fmt_in = 0,
      fmt_out = 0,
      bpp_in = 0,
      bpp_out = 0;
#if 0
   char
      endian_in = 'b',
      endian_out = 'l'
#endif
   char namebuf_in[NAMEBUF_MAX + 1],
      namebuf_out[NAMEBUF_MAX + 1];
   unsigned char* grid = NULL;

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
      fprintf( stderr, "-ib [bpp]\n" );
      fprintf( stderr, "-ob [bpp]\n" );
      fprintf( stderr, "-ic [bpp]\n" );
      fprintf( stderr, "-oc [bpp]\n" );
      return 1;
   }

   switch( fmt_in ) {
   case FMT_BITMAP:
      grid = bmp_read( namebuf_in );
      break;
   }

   if( NULL == grid ) {
      fprintf( stderr, "unable to open %s\n", namebuf_in );
      return 1;
   }

   switch( fmt_out ) {
   case FMT_CGA:
      retval = cga_write( namebuf_out, grid, bpp_out );
      break;
   }

   free( grid );

   return retval;
}

