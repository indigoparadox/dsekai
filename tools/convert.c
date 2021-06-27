
#include "convert.h"
#include "data/drcwrite.h"
#include "data/bmp.h"
#include "data/cga.h"
#include "../src/data/dio.h"
#include "data/header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FMT_BITMAP      1
#define FMT_CGA         2
#define FMT_HEADER      3
#define FMT_HEADER_IMG  4
#define FMT_HEADER_MAP  5
#define FMT_HEADER_WIN  6
#define FMT_JSON_MAP    7
#define FMT_JSON_WIN    8

#define ENDIAN_LITTLE   'l'
#define ENDIAN_BIG      'b'

#define STATE_INFILE       1
#define STATE_OUTFILE      2
#define STATE_INBITS       3
#define STATE_OUTBITS      4
#define STATE_INFMT        7
#define STATE_OUTFMT       8
#define STATE_INW          9
#define STATE_INH          10
#define STATE_INLP         11
#define STATE_OUTLP        12
#define STATE_ENDIAN_IN    13
#define STATE_ENDIAN_OUT   14

int main( int argc, char* argv[] ) {
   int retval = 0;
   int i = 0,
      state = 0,
      fmt_in = 0,
      fmt_out = 0,
      drc_out = 0;
   char namebuf_in[NAMEBUF_MAX + 1],
      namebuf_out[NAMEBUF_MAX + 1];
   struct CONVERT_GRID* grid = NULL;
   struct CONVERT_OPTIONS options_out,
      options_in;

   memset( namebuf_in, '\0', NAMEBUF_MAX + 1 );
   memset( namebuf_out, '\0', NAMEBUF_MAX + 1 );
   memset( &options_in, '\0', sizeof( struct CONVERT_OPTIONS ) );
   memset( &options_out, '\0', sizeof( struct CONVERT_OPTIONS ) );

   options_in.bpp = 2; /* Default to 2-bit CGA. */

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
         options_in.bpp = atoi( argv[i] );
         state = 0;
         break;

      case STATE_OUTBITS:
         options_out.bpp = atoi( argv[i] );
         state = 0;
         break;

#if 0
      case STATE_ENDIAN_IN:
         if( 'l' == argv[i][0] ) {
            options_in.little_endian = 1;
         }
         state = 0;
         break;

      case STATE_ENDIAN_OUT:
         if( 'l' == argv[i][0] ) {
            options_out.little_endian = 1;
         }
         state = 0;
         break;
#endif

      case STATE_INFMT:
         if( 0 == strncmp( argv[i], "bitmap", 6 ) ) {
            fmt_in = FMT_BITMAP;
         } else if( 0 == strncmp( argv[i], "cga", 3 ) ) {
            fmt_in = FMT_CGA;
         } else if( 0 == strncmp( argv[i], "jmap", 4 ) ) {
            fmt_in = FMT_JSON_MAP;
         } else if( 0 == strncmp( argv[i], "jwin", 4 ) ) {
            fmt_in = FMT_JSON_WIN;
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
         } else if( 0 == strncmp( argv[i], "header", 6 ) ) {
            fmt_out = FMT_HEADER;
         }
         state = 0;
         break;

      case STATE_INW:
         options_in.w = atoi( argv[i] );
         state = 0;
         break;

      case STATE_INH:
         options_in.h = atoi( argv[i] );
         state = 0;
         break;

      case STATE_INLP:
         options_in.line_padding = atoi( argv[i] );
         state = 0;
         break;

      case STATE_OUTLP:
         options_out.line_padding = atoi( argv[i] );
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
         } else if( 0 == strncmp( argv[i], "-il", 3 ) ) {
            state = STATE_INLP;
         } else if( 0 == strncmp( argv[i], "-ol", 3 ) ) {
            state = STATE_OUTLP;
         } else if( 0 == strncmp( argv[i], "-r", 3 ) ) {
            options_out.reverse = 1;
         } else if( 0 == strncmp( argv[i], "-ig", 3 ) ) {
            options_in.cga_use_header = 1;
         } else if( 0 == strncmp( argv[i], "-og", 3 ) ) {
            options_out.cga_use_header = 1;
#if 0
         } else if( 0 == strncmp( argv[i], "-ie", 3 ) ) {
            state = STATE_ENDIAN_IN;
         } else if( 0 == strncmp( argv[i], "-oe", 3 ) ) {
            state = STATE_ENDIAN_OUT;
#endif
         } else if( '-' == argv[i][0] ) {
            fprintf( stderr, "invalid command specified\n" );
            return 1;
         }
      }
   }

   printf( "%s (fmt %d) to %s (fmt %d)\n", namebuf_in, fmt_in,
      namebuf_out, fmt_out );

   assert( 0 != strlen( namebuf_in ) );
   assert( 0 != strlen( namebuf_out ) );
   assert( 0 != fmt_in );
   assert( 0 != fmt_out );
   //assert( FMT_CGA != fmt_in || 0 != options_in.w );
   //assert( FMT_CGA != fmt_in || 0 != options_in.h );

   if( FMT_HEADER == fmt_out ) {
      switch( fmt_in ) {
      case FMT_JSON_MAP:
         fmt_out = FMT_HEADER_MAP;
         break;

      case FMT_JSON_WIN:
         fmt_out = FMT_HEADER_WIN;
         break;

      default:
         fmt_out = FMT_HEADER_IMG;
         break;
      }
   }

   if(
      0 == strlen( namebuf_in ) ||
      0 == strlen( namebuf_out ) ||
      0 == fmt_in || 0 == fmt_out ||
      (FMT_CGA == fmt_in &&
         (0 == options_in.w || 0 == options_in.h) &&
            !options_in.cga_use_header )
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
#if 0
      fprintf( stderr, "-ip [in plane padding] (full-screen uses 8000)\n" );
      fprintf( stderr, "-op [out plane padding]\n" );
      fprintf( stderr, "-ie [in endian (b/l)]\n" );
      fprintf( stderr, "-oe [out endian (b/l)]\n" );
#endif
      return 1;
   }

   if( 0 == options_in.bpp && (FMT_CGA == fmt_in || FMT_CGA == fmt_out) ) {
      options_out.bpp = 2;
   }

   switch( fmt_in ) {
   case FMT_BITMAP:
      grid = bmp_read_file( namebuf_in, &options_in );
      break;

   case FMT_CGA:
      grid = cga_read_file( namebuf_in, &options_in );
      break;
   }

   if( NULL == grid ) {
      fprintf( stderr, "unable to open %s\n", namebuf_in );
      return 1;
   }

   if( 0 == options_out.bpp ) {
      /* Default to grid BPP. */
      options_out.bpp = grid->bpp;
   }

   /* dio_print_grid( grid ); */

   switch( fmt_out ) {
   case FMT_BITMAP:
      retval = bmp_write_file( namebuf_out, grid, &options_out );
      break;

   case FMT_CGA:
      retval = cga_write_file( namebuf_out, grid, &options_out );
      break;

   case FMT_HEADER_IMG:
      retval = header_img_write_file( namebuf_out, grid, &options_out );
      break;

   }

   free( grid->data );
   free( grid );

   return retval;
}

