
#include "bmp.h"

#include "../../src/dio.h"
#include "../../src/memory.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../convert.h"

#define BMP_FMT_FILE_HEADER_SZ   14
#define BMP_FMT_BMP_HEADER_SZ    40

#define bmp_int( type, buf, offset ) *((type*)&(buf[offset]))

/**
 * \return The number of bytes successfully written.
 */
int32_t bmp_write_file(
   const char* path, const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   int32_t bmp_buffer_sz = 0,
      bmp_row_sz = 0,
      palette_entries = 0,
      bmp_file_sz = 0,
      written = 0;
   uint8_t* bmp_buffer = NULL;
   FILE* file_out = NULL;

   /* Add rows padded out to 4 bytes. */
   bmp_row_sz = (grid->sz_x * o->bpp) / 8;
   if( 0 != bmp_row_sz % 4 ) {
      bmp_row_sz += (4 - (bmp_row_sz % 4));
   }
   o->bmp_data_sz = (grid->sz_y * bmp_row_sz);

   palette_entries = bmp_colors_count( o->bpp );

   bmp_buffer_sz = 
      BMP_FMT_FILE_HEADER_SZ +
      BMP_FMT_BMP_HEADER_SZ +
      (4 * palette_entries) + /* Palette entries are 32-bit (4 bytes). */
      o->bmp_data_sz;

   /* TODO: Use memory architecture. */
   bmp_buffer = calloc( 1, bmp_buffer_sz );
   assert( NULL != bmp_buffer );

   bmp_file_sz = bmp_write( bmp_buffer, bmp_buffer_sz, grid, o );
   assert( bmp_file_sz == bmp_buffer_sz );
   if( 0 >= bmp_file_sz ) {
      free( bmp_buffer );
      return bmp_file_sz;
   }

   file_out = fopen( path, "wb" );
   assert( NULL != file_out );

   written = fwrite( bmp_buffer, 1, bmp_buffer_sz, file_out );

   fclose( file_out );
   free( bmp_buffer );

   assert( written == bmp_buffer_sz );
   if( written != bmp_buffer_sz ) {
      return CONVERT_ERROR_FILE_WRITE;
   }

   return bmp_buffer_sz;
}

static uint8_t bmp_palette_cga_4_to_16( int color_in ) {
   switch( color_in ) {
   case 0:
      return 0;

   case 1:
      /* Cyan */
      return 11;

   case 2:
      /* Magenta */
      return 13;

   case 3:
      /* White */
      return 15;
   }

   return 0;
}

static uint8_t bmp_palette_any_to_mono( uint8_t color_in, uint8_t reverse ) {
   if( reverse ) {
      /* In reverse, 0s become 1s. */
      return 0 == color_in ? 1 : 0;
   } else {
      return 0 == color_in ? 0 : 1;
   }
   return 0;
}

uint8_t bmp_colors_count( uint8_t bpp ) {
   switch( bpp ) {
   case 1:
      return 2;

   case 2:
      return 4;

   case 4:
   default:
      return 16;
   }
}

/**
 * \return The number of bytes successfully written.
 */
int32_t bmp_write(
   uint8_t* buf_ptr, uint32_t buf_sz,
   const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   int32_t x = 0,
      y = 0,
      bit_idx = 0;
   uint32_t i = 0,
      row_bytes = 0,
      bmp_file_byte_idx = 0;
   uint8_t byte_buffer = 0,
      bit_mask_in = 0,
      bit_mask_out = 0;
   uint32_t bmp_colors = 0,
      * bmp_palette = 0;
   struct BITMAP_FILE_HEADER* file_header = NULL;
   struct BITMAP_DATA_HEADER* data_header = NULL;

   if( o->bmp_no_file_header ) {
      file_header = NULL;
      data_header = (struct BITMAP_DATA_HEADER*)buf_ptr;

      bmp_file_byte_idx +=
         sizeof( struct BITMAP_DATA_HEADER );

   } else {
      file_header = (struct BITMAP_FILE_HEADER*)buf_ptr;
      data_header = (struct BITMAP_DATA_HEADER*)&(buf_ptr[
         sizeof( struct BITMAP_FILE_HEADER )]);

      file_header->id[0] = 'B';
      file_header->id[1] = 'M';
         
      bmp_file_byte_idx +=
         sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER );
   }

   bmp_colors = bmp_colors_count( o->bpp );

   assert( sizeof( struct BITMAP_DATA_HEADER ) == 40 );
   
   data_header->header_sz = 40;  /* Header Size */
   data_header->bitmap_w = grid->sz_x;
   data_header->bitmap_h = grid->sz_y;
   data_header->planes = 1; /* Planes */
   data_header->bpp = o->bpp;
   data_header->compression = 0; /* Compression. */
   data_header->image_sz = 
      (data_header->bitmap_w * data_header->bitmap_h) / (8 / o->bpp);
   data_header->hres = 72; /* HRes */
   data_header->vres = 72; /* VRes */
   data_header->colors = bmp_colors;
   data_header->colors_important = 0; /* Important Colors */

   bmp_palette = (uint32_t*)&(buf_ptr[bmp_file_byte_idx]);
   *bmp_palette = 0x00000000; /* Palette: Black */
   bmp_palette++;
   bmp_file_byte_idx += 4;
   
   switch( o->bpp ) {
   case 1:
      *bmp_palette = 0x00ffffff; /* Palette: White */
      bmp_palette++;
      bmp_file_byte_idx += 4;

      assert( bmp_file_byte_idx ==
         (o->bmp_no_file_header ? 0 : sizeof( struct BITMAP_FILE_HEADER )) +
         sizeof( struct BITMAP_DATA_HEADER ) +
         (2 * 4) );
      break;

   case 2:
      *bmp_palette = 0x0000ffff; /* Palette: Cyan */
      bmp_palette++;
      *bmp_palette = 0x00ff00ff; /* Palette: Magenta */
      bmp_palette++;
      *bmp_palette = 0x00ffffff; /* Palette: White */
      bmp_palette++;
      bmp_file_byte_idx += (4 * 3);

      assert( bmp_file_byte_idx ==
         (o->bmp_no_file_header ? 0 : sizeof( struct BITMAP_FILE_HEADER )) +
         sizeof( struct BITMAP_DATA_HEADER ) +
         (4 * 4) );
      break;

   case 4:
   default:
      *bmp_palette = dio_reverse_endian_32( 0xaa000000 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0x00aa0000 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0xaaaa0000 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0x0000aa00 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0xaa00aa00 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0x0055aa00 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0xaaaaaa00 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0x55555500 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0xff555500 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0x55ff5500 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0xffff5500 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0x5555ff00 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0xff55ff00 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0x55ffff00 );
      bmp_palette++;
      *bmp_palette = dio_reverse_endian_32( 0xffffff00 );
      bmp_palette++;
      bmp_file_byte_idx += (4 * 15);

      assert( bmp_file_byte_idx ==
         (o->bmp_no_file_header ? 0 : sizeof( struct BITMAP_FILE_HEADER )) +
         sizeof( struct BITMAP_DATA_HEADER ) +
         (16 * 4) );

      break;
   }

   /* Store the offset of the start of the bitmap data. */
   if( !o->bmp_no_file_header ) {
      file_header->bmp_offset = bmp_file_byte_idx;
   }
   o->bmp_data_offset_out = bmp_file_byte_idx;

   /* Calculate bit masks. */
   for( i = 0 ; o->bpp > i ; i++ ) {
      bit_mask_out <<= 1;
      bit_mask_out |= 0x01;
   }
   for( i = 0 ; grid->bpp > i ; i++ ) {
      bit_mask_in <<= 1;
      bit_mask_in |= 0x01;
   }

   debug_printf( 1, "using write mask: 0x%x\n", bit_mask_out );

   for( y = grid->sz_y - 1 ; y >= 0 ; y-- ) {
      row_bytes = 0;
      for( x = 0 ; x < grid->sz_x ; x++ ) {
         assert( y >= 0 );
         assert( x >= 0 );
         assert( (y * grid->sz_x) + x < grid->data_sz );

         /* Format grid data into byte. */
         byte_buffer <<= o->bpp;
         switch( o->bpp ) {
         case 1:
            byte_buffer |= 
               bmp_palette_any_to_mono( grid->data[(y * grid->sz_x) + x],
               o->reverse);
            break;

         case 4:
         default:
            byte_buffer |= 
               bmp_palette_cga_4_to_16( grid->data[(y * grid->sz_x) + x] )
               & bit_mask_out;
            break;
         }
         /* dio_print_binary( byte_buffer ); */
         bit_idx += o->bpp;

         /* Write finished byte. */
         if( 0 != bit_idx && 0 == bit_idx % 8 ) {
            debug_printf( 
               1, "bmp: writing byte %d out of %d (row %d, col %d)\n",
                  bmp_file_byte_idx, buf_sz, y, x );
            assert( bmp_file_byte_idx < buf_sz );
            buf_ptr[bmp_file_byte_idx] = byte_buffer;
            byte_buffer = 0;
            bmp_file_byte_idx++;
            row_bytes++;
            bit_idx = 0;
         }
      }
      while( 0 != (row_bytes % 4) ) {
         debug_printf( 1, "bmp: adding row padding byte\n" );
         buf_ptr[bmp_file_byte_idx] = '\0';
         bmp_file_byte_idx++;
         row_bytes++;
      }
   }

   if( !o->bmp_no_file_header ) {
      file_header->file_sz = bmp_file_byte_idx;
   }

   return bmp_file_byte_idx;
}

struct CONVERT_GRID* bmp_read_file(
   const char* path, struct CONVERT_OPTIONS* o
) {
   uint8_t* bmp_buffer = NULL;
   MEMORY_HANDLE buffer_handle = NULL;
   uint32_t bmp_buffer_sz = 0;
   struct CONVERT_GRID* grid_out = NULL;

   bmp_buffer_sz = dio_read_file( path, &buffer_handle );

   bmp_buffer = memory_lock( buffer_handle );

   grid_out = bmp_read( bmp_buffer, bmp_buffer_sz, o );

   bmp_buffer = memory_unlock( buffer_handle );

   memory_free( buffer_handle );

   return grid_out;
}

struct CONVERT_GRID* bmp_read(
   const uint8_t* buf, uint32_t buf_sz, struct CONVERT_OPTIONS* o
) {
   uint32_t x = 0,
      y = 0,
      i = 0,
      byte_idx = 0,
      bit_idx = 0,
      bmp_data_offset = 0,
      bmp_data_size = 0;
   int32_t sz_x = 0,
      sz_y = 0;
   uint16_t bpp = 0;
   struct CONVERT_GRID* grid = NULL;
   char byte_buffer = 0;

   /* Read the bitmap file header. */
   assert( 'B' == buf[0] );
   assert( 'M' == buf[1] );
   assert( buf_sz == bmp_int( uint32_t, buf, 2 ) );
   bmp_data_offset = bmp_int( uint32_t, buf, 10 );
   debug_printf( 2, "bitmap data starts at %u bytes\n", bmp_data_offset );

   /* Read the bitmap image header. */
   debug_printf( 2, "bitmap header is %u bytes\n",
      bmp_int( uint32_t, buf, 14 ) );
   assert( 40 == bmp_int( uint32_t, buf, 14 ) ); /* Windows BMP. */
   sz_x = bmp_int( int32_t, buf, 18 );
   sz_y = bmp_int( int32_t, buf, 22 );
   bpp = bmp_int( uint16_t, buf, 28 );
   debug_printf( 2, "bitmap is %d x %d, %u colors (palette has %u)\n",
      sz_x, sz_y, bpp, bmp_int( uint32_t, buf, 46 ) );
   assert( bmp_int( uint16_t, buf, 30 ) == 0 ); /* No compression. */

   /* Read the bitmap data. */
   bmp_data_size = buf_sz - bmp_data_offset;
   debug_printf( 2, "bitmap data is %u bytes\n", bmp_data_size );
   /* TODO: Use memory architecture. */
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
      debug_printf( 1, "bmp: byte_idx %u, bit_idx %u, row %u, col %u (%u)\n",
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

