
#include "bmp.h"

#include "dio.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define BMP_FMT_FILE_HEADER_SZ   14
#define BMP_FMT_BMP_HEADER_SZ    40

#define bmp_int( type, buf, offset ) *((type*)&(buf[offset]))

static const uint32_t gc_bmp_header_sz = 40;
static const uint16_t gc_bmp_planes = 1;
static const uint32_t gc_bmp_compress_none = 0;
static const int32_t gc_bmp_res = 72;
static const uint32_t gc_bmp_colors_imp = 0;

int bmp_write_file(
   const char* path, const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   uint32_t bmp_buffer_sz = 0,
      bmp_row_sz = 0;
   uint8_t* bmp_buffer = NULL;
   FILE* file_out = NULL;
   int retval = 0;

   /* Add rows padded out to 4 bytes. */
   bmp_row_sz = (grid->sz_x * o->bpp) / 8;
   if( 0 != bmp_row_sz % 4 ) {
      bmp_row_sz += (4 - (bmp_row_sz % 4));
   }
   o->bmp_data_sz = (grid->sz_y * bmp_row_sz);

   bmp_buffer_sz = 
      BMP_FMT_FILE_HEADER_SZ +
      BMP_FMT_BMP_HEADER_SZ +
      (4 * (1 == o->bpp ? 2 : 4)) + /* Palette entries. */
      o->bmp_data_sz;

   bmp_buffer = calloc( 1, bmp_buffer_sz );
   assert( NULL != bmp_buffer );

   retval = bmp_write( bmp_buffer, bmp_buffer_sz, grid, o );
   if( retval ) {
      free( bmp_buffer );
      return retval;
   }

   file_out = fopen( path, "wb" );
   assert( NULL != file_out );

   fwrite( bmp_buffer, 1, bmp_buffer_sz, file_out );

   fclose( file_out );
   free( bmp_buffer );

   return retval;
}

int bmp_write(
   uint8_t* buf_ptr, uint32_t buf_sz,
   const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   FILE* bmp_file = NULL;
   int retval = 0;
   ssize_t x = 0,
      y = 0,
      bit_idx = 0;
   size_t bmp_data_offset = 0,
      bmp_file_sz = 0,
      i = 0,
      row_bytes = 0,
      bmp_data_byte_idx = 0;
   uint8_t byte_buffer = 0,
      bit_mask_in = 0,
      bit_mask_out = 0;
   uint32_t bmp_colors = 0;

   buf_ptr[0] = 'B';
   buf_ptr[1] = 'M';
   bmp_int( uint32_t, buf_ptr, 2 ) = buf_sz;

   bmp_colors = 1 == o->bpp ? 2 : 4;

   bmp_int( uint32_t, buf_ptr, 14 ) = 40;  /* Header Size */
   bmp_int( uint32_t, buf_ptr, 18 ) = grid->sz_x;
   bmp_int( uint32_t, buf_ptr, 22 ) = grid->sz_y;
   bmp_int( uint16_t, buf_ptr, 26 ) = 1; /* Planes */
   bmp_int( uint16_t, buf_ptr, 28 ) = o->bpp;
   bmp_int( uint32_t, buf_ptr, 30 ) = 0; /* Compression. */
   bmp_int( uint32_t, buf_ptr, 34 ) = 0;
   bmp_int( int32_t, buf_ptr, 38 ) = 72; /* HRes */
   bmp_int( int32_t, buf_ptr, 42 ) = 72; /* VRes */
   bmp_int( uint32_t, buf_ptr, 46 ) = bmp_colors;
   bmp_int( uint32_t, buf_ptr, 50 ) = 0; /* Important Colors */

   bmp_int( uint32_t, buf_ptr, 54 ) = 0x00000000; /* Palette: Black */
   if( 1 < o->bpp ) {
      bmp_int( uint32_t, buf_ptr, 58 ) = 0x0000ffff; /* Palette: Cyan */
      bmp_int( uint32_t, buf_ptr, 62 ) = 0x00ff00ff; /* Palette: Magenta */
      bmp_int( uint32_t, buf_ptr, 66 ) = 0x00ffffff; /* Palette: White */
      bmp_data_offset = 70;
   } else {
      bmp_int( uint32_t, buf_ptr, 58 ) = 0x00ffffff; /* Palette: White */
      bmp_data_offset = 62;
   }

   bmp_int( uint32_t, buf_ptr, 10 ) = bmp_data_offset;

   /* Calculate bit masks. */
   for( i = 0 ; o->bpp > i ; i++ ) {
      bit_mask_out <<= 1;
      bit_mask_out |= 0x01;
   }
   for( i = 0 ; grid->bpp > i ; i++ ) {
      bit_mask_in <<= 1;
      bit_mask_in |= 0x01;
   }

   dio_printf( "using write mask: 0x%x\n", bit_mask_out );

   for( y = grid->sz_y - 1 ; y >= 0 ; y-- ) {
      row_bytes = 0;
      for( x = 0 ; x < grid->sz_x ; x++ ) {
         assert( y >= 0 );
         assert( x >= 0 );
         assert( (y * grid->sz_x) + x < grid->data_sz );

         /* Format grid data into byte. */
         byte_buffer <<= o->bpp;
         if( 1 == o->bpp ) {
            if( o->reverse ) {
               /* In reverse, 0s become 1s. */
               if( 0 == grid->data[(y * grid->sz_x) + x] ) {
                  byte_buffer |= 0x01;
               } else {
                  byte_buffer |= 0x00;
               }
            } else {
               if( 0 != grid->data[(y * grid->sz_x) + x] ) {
                  byte_buffer |= 0x01;
               } else {
                  byte_buffer |= 0x00;
               }
            }
         } else {
            byte_buffer |= grid->data[(y * grid->sz_x) + x] & bit_mask_out;
         }
         dio_print_binary( byte_buffer );
         bit_idx += o->bpp;

         /* Write finished byte. */
         if( 0 != bit_idx && 0 == bit_idx % 8 ) {
            dio_printf( "writing one byte (row %ld, col %ld)\n", y, x );
            buf_ptr[bmp_data_offset + bmp_data_byte_idx] = byte_buffer;
            byte_buffer = 0;
            assert( bmp_data_byte_idx < buf_sz );
            bmp_data_byte_idx++;
            row_bytes++;
            bit_idx = 0;
         }
      }
      while( 0 != (row_bytes % 4) ) {
         dio_printf( "adding row padding byte\n" );
         buf_ptr[bmp_data_offset + bmp_data_byte_idx] = '\0';
         bmp_data_byte_idx++;
         row_bytes++;
      }
   }

   return retval;
}

struct CONVERT_GRID* bmp_read_file(
   const char* path, struct CONVERT_OPTIONS* o
) {
   uint8_t* bmp_buffer = NULL;
   uint32_t bmp_buffer_sz = 0;
   struct CONVERT_GRID* grid_out = NULL;

   bmp_buffer_sz = dio_read_file( path, &bmp_buffer );

   grid_out = bmp_read( bmp_buffer, bmp_buffer_sz, o );

   free( bmp_buffer );

   return grid_out;
}

struct CONVERT_GRID* bmp_read(
   const uint8_t* buf, uint32_t buf_sz, struct CONVERT_OPTIONS* o
) {
   size_t offset = 0,
      read = 0,
      x = 0,
      y = 0,
      i = 0,
      byte_idx = 0,
      bit_idx = 0;
   uint32_t bmp_data_offset = 0,
      bmp_data_size = 0;
   int32_t sz_x = 0,
      sz_y = 0;
   uint16_t bpp = 0;
   FILE* bmp_file = NULL;
   struct CONVERT_GRID* grid = NULL;
   char byte_buffer = 0;

   /* Read the bitmap file header. */
   assert( 'B' == buf[0] );
   assert( 'M' == buf[1] );
   assert( buf_sz == bmp_int( uint32_t, buf, 2 ) );
   bmp_data_offset = bmp_int( uint32_t, buf, 10 );
   dio_printf( "bitmap data starts at %u bytes\n", bmp_data_offset );

   /* Read the bitmap image header. */
   dio_printf( "bitmap header is %u bytes\n",
      bmp_int( uint32_t, buf, 14 ) );
   assert( 40 == bmp_int( uint32_t, buf, 14 ) ); /* Windows BMP. */
   sz_x = bmp_int( int32_t, buf, 18 );
   sz_y = bmp_int( int32_t, buf, 22 );
   bpp = bmp_int( uint16_t, buf, 28 );
   dio_printf( "bitmap is %d x %d, %u colors (palette has %u)\n",
      sz_x, sz_y, bpp, bmp_int( uint32_t, buf, 46 ) );
   assert( bmp_int( uint16_t, buf, 30 ) == 0 ); /* No compression. */

   /* Read the bitmap data. */
   bmp_data_size = buf_sz - bmp_data_offset;
   dio_printf( "bitmap data is %u bytes\n", bmp_data_size );
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
      dio_printf( "bmp: byte_idx %lu, bit_idx %lu, row %lu, col %lu (%lu)\n",
         byte_idx, bit_idx, y, x, (y * sz_x) + x );
      if( 0 == bit_idx % 8 ) {
         byte_buffer = buf[bmp_data_offset + byte_idx];
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

