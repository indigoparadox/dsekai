
#include "bmp.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define BMP_FMT_FILE_HEADER_SZ   14
#define BMP_FMT_BMP_HEADER_SZ    40

#define bmp_int( type, buf, offset ) *(type*)&(buf[offset])

static const uint32_t gc_bmp_header_sz = 40;
static const uint16_t gc_bmp_planes = 1;
static const uint32_t gc_bmp_compress_none = 0;
static const int32_t gc_bmp_res = 72;
static const uint32_t gc_bmp_colors_imp = 0;

int bmp_write(
   const char* path, const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   FILE* bmp_file = NULL;
   int retval = 0;
   ssize_t x = 0,
      y = 0,
      bit_idx = 0;
   size_t bmp_data_offset = 0,
      bmp_file_sz = 0,
      i = 0,
      row_bytes = 0;
   uint8_t byte_buffer = 0,
      bit_mask_in = 0,
      bit_mask_out = 0;
   uint32_t bmp_colors = 0;

   bmp_file = fopen( path, "wb" );
   assert( NULL != bmp_file );

   fputc( 'B', bmp_file );
   fputc( 'M', bmp_file );
   fwrite( "\0\0\0\0", 4, 1, bmp_file ); /* Placeholder: File Size */
   fwrite( "\0\0\0\0", 4, 1, bmp_file ); /* Reserved */
   fwrite( "\0\0\0\0", 4, 1, bmp_file ); /* Placeholder: Bitmap offset. */

   bmp_colors = 1 == o->bpp ? 2 : 4;

   fwrite( &gc_bmp_header_sz, 4, 1, bmp_file );  /* Header Size */
   fwrite( &(grid->sz_x), 4, 1, bmp_file );  /* X Size */
   fwrite( &(grid->sz_y), 4, 1, bmp_file );  /* Y Size */
   fwrite( &gc_bmp_planes, 2, 1, bmp_file );
   fwrite( &o->bpp, 2, 1, bmp_file );
   fwrite( &gc_bmp_compress_none, 4, 1, bmp_file );
   fwrite( "\0\0\0\0", 4, 1, bmp_file ); /* Placeholder: Bitmap Size */
   fwrite( &gc_bmp_res, 4, 1, bmp_file );
   fwrite( &gc_bmp_res, 4, 1, bmp_file );
   fwrite( &bmp_colors, 4, 1, bmp_file );
   fwrite( &gc_bmp_colors_imp, 4, 1, bmp_file );

   fwrite( "\0\0\0\0", 1, 4, bmp_file );
   if( 1 < o->bpp ) {
      fwrite( "\xff\xff\0\0", 1, 4, bmp_file );
      fwrite( "\xff\0\xff\0", 1, 4, bmp_file );
   }
   fwrite( "\xff\xff\xff\0", 1, 4, bmp_file );

   /* Calculate bit masks. */
   for( i = 0 ; o->bpp > i ; i++ ) {
      bit_mask_out <<= 1;
      bit_mask_out |= 0x01;
   }
   for( i = 0 ; grid->bpp > i ; i++ ) {
      bit_mask_in <<= 1;
      bit_mask_in |= 0x01;
   }

   convert_printf( "using write mask: 0x%x\n", bit_mask_out );

   bmp_data_offset = ftell( bmp_file );
   for( y = grid->sz_y - 1 ; y >= 0 ; y-- ) {
      row_bytes = 0;
      for( x = 0 ; x < grid->sz_x ; x++ ) {
         assert( y >= 0 );
         assert( x >= 0 );
         assert( (y * grid->sz_x) + x < grid->data_sz );

         /* Format grid data into byte. */
         byte_buffer <<= o->bpp;
         if( o->bpp < grid->bpp && 0 != grid->data[(y * grid->sz_x) + x] ) {
            byte_buffer |= 0x01;
         } else {
            byte_buffer |= grid->data[(y * grid->sz_x) + x] & bit_mask_out;
         }
         convert_print_binary( byte_buffer );
         bit_idx += o->bpp;

         /* Write finished byte. */
         if( 0 != bit_idx && 0 == bit_idx % 8 ) {
            convert_printf( "writing one byte (row %ld, col %ld)\n", y, x );
            fwrite( &byte_buffer, 1, 1, bmp_file );
            byte_buffer = 0;
            row_bytes++;
            bit_idx = 0;
         }
      }
      while( 0 != (row_bytes % 4) ) {
         convert_printf( "adding row padding byte\n" );
         fputc( '\0', bmp_file );
         row_bytes++;
      }
   }

   bmp_file_sz = ftell( bmp_file );

   fseek( bmp_file, 2, SEEK_SET );
   fwrite( &bmp_file_sz, 4, 1, bmp_file );

   fseek( bmp_file, 10, SEEK_SET );
   fwrite( &bmp_data_offset, 4, 1, bmp_file );

   fclose( bmp_file );

   return retval;
}

struct CONVERT_GRID* bmp_read( const char* path, struct CONVERT_OPTIONS* o ) {
   size_t offset = 0,
      read = 0,
      x = 0,
      y = 0,
      i = 0,
      byte_idx = 0,
      bit_idx = 0;
   uint32_t bmp_file_sz = 0,
      bmp_data_offset = 0,
      bmp_data_size = 0;
   int32_t sz_x = 0,
      sz_y = 0;
   uint16_t bpp = 0;
   FILE* bmp_file = NULL;
   uint8_t* raw_bmp_data = NULL;
   struct CONVERT_GRID* grid = NULL;
   char buffer[CONVERT_BUFFER_SZ + 1];
   char byte_buffer = 0;

   memset( buffer, '\0', CONVERT_BUFFER_SZ );

   bmp_file = fopen( path, "rb" );
   assert( NULL != bmp_file );

   /* Read the bitmap file header. */
   read = fread( buffer, 1, BMP_FMT_FILE_HEADER_SZ, bmp_file );
   assert( BMP_FMT_FILE_HEADER_SZ == read );
   assert( 'B' == buffer[0] );
   assert( 'M' == buffer[1] );
   bmp_file_sz = bmp_int( uint32_t, buffer, 2 );
   convert_printf( "bitmap file is %u bytes\n", bmp_file_sz );
   bmp_data_offset = bmp_int( uint32_t, buffer, 10 );
   convert_printf( "bitmap data starts at %u bytes\n", bmp_data_offset );

   memset( buffer, '\0', CONVERT_BUFFER_SZ );
   read = fread( buffer, 1, BMP_FMT_BMP_HEADER_SZ, bmp_file );
   assert( BMP_FMT_BMP_HEADER_SZ == read );

   /* Read the bitmap image header. */
   convert_printf( "bitmap header is %u bytes\n", bmp_int( uint32_t, buffer, 0 ) );
   assert( 40 == bmp_int( uint32_t, buffer, 0 ) ); /* Windows BMP. */
   sz_x = bmp_int( int32_t, buffer, 4 );
   sz_y = bmp_int( int32_t, buffer, 8 );
   bpp = bmp_int( uint16_t, buffer, 14 );
   convert_printf( "bitmap is %d x %d, %u colors (palette has %u)\n",
      sz_x, sz_y, bpp, bmp_int( uint32_t, buffer, 28 ) );
   assert( bmp_int( uint16_t, buffer, 16 ) == 0 ); /* No compression. */

   /* Read the bitmap data. */
   bmp_data_size = bmp_file_sz - bmp_data_offset;
   convert_printf( "bitmap data is %u bytes\n", bmp_data_size );
   raw_bmp_data = calloc( 1, bmp_data_size );
   assert( NULL != raw_bmp_data );
   fseek( bmp_file, bmp_data_offset, SEEK_SET );
   read = fread( raw_bmp_data, 1, bmp_data_size, bmp_file );
   assert( read == bmp_data_size );
   fclose( bmp_file );
   grid = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid );
   grid->data_sz = sz_x * sz_y;
   grid->data = calloc( 1, grid->data_sz );
   assert( NULL != grid->data );

   grid->sz_x = sz_x;
   grid->sz_y = sz_y;
   grid->bpp = bpp;

   /* Grid starts from top, bitmap starts from bottom. */
   y = sz_y - 1;
   while( bmp_data_size > byte_idx ) {
      convert_printf( "byte_idx %lu, bit_idx %lu, row %lu, col %lu (%lu)\n",
         byte_idx, bit_idx, y, x, (y * sz_x) + x );
      if( 0 == bit_idx % 8 ) {
         byte_buffer = raw_bmp_data[byte_idx];
         byte_idx++;
         bit_idx = 0;
      }

      assert( (y * sz_x) + x < (bmp_data_size * (8 / bpp)) );

      /* Read this pixel into the grid. */
      for( i = 0 ; bpp > i ; i++ ) {
         grid->data[(y * sz_x) + x] |= byte_buffer & (0x01 << bit_idx);
         bit_idx++;
      }
      grid->data[(y * sz_x) + x] >>= (bit_idx - bpp);

      /* Move to the next pixel. */
      x++;
      if( x >= sz_x ) {
         /* Move to the next row. */
         y--;
         x = 0;
         while( byte_idx % 4 != 0 ) {
            byte_idx++;
         }
         /* Get past the padding. */
      }
   }

   return grid;
}

