
#include "bmp.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../convert.h"

#define BMP_FMT_FILE_HEADER_SZ   14
#define BMP_FMT_BMP_HEADER_SZ    40

#define bmp_int( type, buf, offset ) *((type*)&(buf[offset]))

const uint32_t gc_bmp_color_black =          0x00000000;
const uint32_t gc_bmp_color_blue =           0x000000aa;
const uint32_t gc_bmp_color_green =          0x0000aa00;
const uint32_t gc_bmp_color_cyan =           0x0000aaaa;
const uint32_t gc_bmp_color_red =            0x00aa0000;
const uint32_t gc_bmp_color_magenta =        0x00aa00aa;
const uint32_t gc_bmp_color_brown =          0x00aa5500;
const uint32_t gc_bmp_color_light_gray =     0x00aaaaaa;
const uint32_t gc_bmp_color_dark_gray =      0x00555555;
const uint32_t gc_bmp_color_light_blue =     0x005555ff;
const uint32_t gc_bmp_color_light_green =    0x0055ff55;
const uint32_t gc_bmp_color_light_cyan =     0x0055ffff;
const uint32_t gc_bmp_color_light_red =      0x00ff5555;
const uint32_t gc_bmp_color_light_magenta =  0x00ff55ff;
const uint32_t gc_bmp_color_yellow =         0x00ffff55;
const uint32_t gc_bmp_color_white =          0x00ffffff;

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
   struct DIO_STREAM* stream,
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
      bit_mask_out = 0,
      * grid_data = NULL;
   uint32_t bmp_colors = 0,
      * bmp_palette = 0;
   struct BITMAP_FILE_HEADER file_header;
   struct BITMAP_DATA_HEADER data_header;

   if( o->bmp_no_file_header ) {
      bmp_file_byte_idx +=
         sizeof( struct BITMAP_DATA_HEADER );

   } else {
      file_header.id[0] = 'B';
      file_header.id[1] = 'M';
         
      bmp_file_byte_idx +=
         sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER );

      dio_seek_stream( stream, sizeof( struct BITMAP_FILE_HEADER ), SEEK_SET );
   }

   bmp_colors = bmp_colors_count( o->bpp );

   assert( sizeof( struct BITMAP_DATA_HEADER ) == 40 );

   data_header.header_sz = 40;  /* Header Size */
   data_header.bitmap_w = grid->sz_x;
   data_header.bitmap_h = grid->sz_y;
   data_header.planes = 1; /* Planes */
   data_header.bpp = o->bpp;
   data_header.compression = 0; /* Compression. */
   data_header.image_sz = 
      (data_header.bitmap_w * data_header.bitmap_h) / (8 / o->bpp);
   data_header.hres = 72; /* HRes */
   data_header.vres = 72; /* VRes */
   data_header.colors = bmp_colors;
   data_header.colors_important = 0; /* Important Colors */

   dio_write_stream(
      &data_header, sizeof( struct BITMAP_DATA_HEADER ), stream );

   dio_write_stream( &gc_bmp_color_black, 4, stream );
   bmp_file_byte_idx += 4;
   
   switch( o->bpp ) {
   case 1:
      dio_write_stream( &gc_bmp_color_white, 4, stream );
      bmp_file_byte_idx += 4;

      assert( bmp_file_byte_idx ==
         (o->bmp_no_file_header ? 0 : sizeof( struct BITMAP_FILE_HEADER )) +
         sizeof( struct BITMAP_DATA_HEADER ) +
         (2 * 4) );
      break;

   case 2:
      dio_write_stream( &gc_bmp_color_cyan, 4, stream );
      dio_write_stream( &gc_bmp_color_magenta, 4, stream );
      dio_write_stream( &gc_bmp_color_white, 4, stream );
      bmp_file_byte_idx += (4 * 3);

      assert( bmp_file_byte_idx ==
         (o->bmp_no_file_header ? 0 : sizeof( struct BITMAP_FILE_HEADER )) +
         sizeof( struct BITMAP_DATA_HEADER ) +
         (4 * 4) );
      break;

   case 4:
   default:
      dio_write_stream( &gc_bmp_color_blue, 4, stream );
      dio_write_stream( &gc_bmp_color_green, 4, stream );
      dio_write_stream( &gc_bmp_color_cyan, 4, stream );
      dio_write_stream( &gc_bmp_color_red, 4, stream );
      dio_write_stream( &gc_bmp_color_magenta, 4, stream );
      dio_write_stream( &gc_bmp_color_brown, 4, stream );
      dio_write_stream( &gc_bmp_color_light_gray, 4, stream );
      dio_write_stream( &gc_bmp_color_dark_gray, 4, stream );
      dio_write_stream( &gc_bmp_color_light_blue, 4, stream );
      dio_write_stream( &gc_bmp_color_light_green, 4, stream );
      dio_write_stream( &gc_bmp_color_light_cyan, 4, stream );
      dio_write_stream( &gc_bmp_color_light_red, 4, stream );
      dio_write_stream( &gc_bmp_color_light_magenta, 4, stream );
      dio_write_stream( &gc_bmp_color_yellow, 4, stream );
      dio_write_stream( &gc_bmp_color_white, 4, stream );
      bmp_file_byte_idx += (4 * 15);

      assert( bmp_file_byte_idx ==
         (o->bmp_no_file_header ? 0 : sizeof( struct BITMAP_FILE_HEADER )) +
         sizeof( struct BITMAP_DATA_HEADER ) +
         (16 * 4) );

      break;
   }

   /* Store the offset of the start of the bitmap data. */
   if( !o->bmp_no_file_header ) {
      file_header.bmp_offset = bmp_file_byte_idx;
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

   grid_data = memory_lock( grid->data );
   if( NULL == grid_data ) { goto cleanup; }

   for( y = grid->sz_y - 1 ; y >= 0 ; y-- ) {
      row_bytes = 0;
      for( x = 0 ; x < grid->sz_x ; x++ ) {
         assert( y >= 0 );
         assert( x >= 0 );
         assert( (y * grid->sz_x) + x < grid->data_sz );

         /* Format grid data into byte using a color conversion function. */
         byte_buffer <<= o->bpp;
         switch( o->bpp ) {
         case 1:
            byte_buffer |= 
               bmp_palette_any_to_mono( grid_data[(y * grid->sz_x) + x],
               o->reverse);
            break;

         case 4:
         default:
            byte_buffer |= 
               bmp_palette_cga_4_to_16( grid_data[(y * grid->sz_x) + x] )
               & bit_mask_out;
            break;
         }
         bit_idx += o->bpp;

         /* Write finished byte. */
         if( 0 != bit_idx && 0 == bit_idx % 8 ) {
            /* debug_printf( 
               1, "bmp: writing byte %d out of %d (row %d, col %d)\n",
                  bmp_file_byte_idx, buf_sz, y, x );
            assert( bmp_file_byte_idx < buf_sz ); */
            dio_write_stream( &byte_buffer, 1, stream );
            byte_buffer = 0;
            bmp_file_byte_idx++;
            row_bytes++;
            bit_idx = 0;
         }
      }
      while( 0 != (row_bytes % 4) ) {
         debug_printf( 1, "bmp: adding row padding byte\n" );
         dio_write_stream( &byte_buffer, 1, stream );
         bmp_file_byte_idx++;
         row_bytes++;
      }
   }

   if( !o->bmp_no_file_header ) {
      /* Go back and write the now-completed file header before returning to
       * the end. */
      file_header.file_sz = bmp_file_byte_idx;
      dio_seek_stream( stream, 0, SEEK_SET );
      dio_write_stream(
         &file_header, sizeof( struct BITMAP_FILE_HEADER ), stream );
      dio_seek_stream( stream, bmp_file_byte_idx, SEEK_SET );
   }

cleanup:

   if( NULL != grid_data ) {
      memory_unlock( grid->data );
   }

   return bmp_file_byte_idx;
}

MEMORY_HANDLE bmp_read( struct DIO_STREAM* stream, struct CONVERT_OPTIONS* o ) {
   uint32_t x = 0,
      y = 0,
      i = 0,
      byte_idx = 0,
      bit_idx = 0,
      bmp_data_size = 0;
   uint16_t bpp = 0;
   MEMORY_HANDLE grid_handle = NULL;
   struct CONVERT_GRID* grid = NULL;
   char byte_buffer = 0;
   uint8_t* grid_data = NULL;
   struct BITMAP_FILE_HEADER file_header;
   struct BITMAP_DATA_HEADER data_header;

   /* Read the bitmap file header. */
   dio_read_stream( &file_header, sizeof( struct BITMAP_FILE_HEADER ), stream );
   assert( 'B' == file_header.id[0] );
   assert( 'M' == file_header.id[1] );
   debug_printf(
      1, "bitmap data starts at %u bytes\n", file_header.bmp_offset );

   /* Read the bitmap image header. */
   dio_read_stream( &data_header, sizeof( struct BITMAP_DATA_HEADER ), stream );
   debug_printf( 1, "bitmap header is %u bytes\n", data_header.header_sz );
   assert( 40 == data_header.header_sz );
   debug_printf( 2, "bitmap is %d x %d, %u BPP (palette has %u colors)\n",
      data_header.bitmap_w, data_header.bitmap_h, data_header.bpp,
      data_header.colors );
   /*  assert( bmp_int( uint16_t, buf, 30 ) == 0 ); */ /* No compression. */

   /* Read the bitmap data. */
   bmp_data_size = file_header.file_sz - file_header.bmp_offset;
   debug_printf( 2, "bitmap data is %u bytes\n", bmp_data_size );
   /* Allocate new grid. */
   grid_handle = memory_alloc( 1, sizeof( struct CONVERT_GRID ) );
   if( NULL == grid_handle ) { goto cleanup; }
   grid = memory_lock( grid_handle );
   if( NULL == grid ) { goto cleanup; }

   grid->data_sz = data_header.bitmap_w * data_header.bitmap_h;
   grid->data = memory_alloc( 1, grid->data_sz );
   if( NULL == grid->data ) { goto cleanup; }
   grid_data = memory_lock( grid->data );
   if( NULL == grid_data ) { goto cleanup; }

   grid->sz_x = data_header.bitmap_w;
   grid->sz_y = data_header.bitmap_h;
   grid->bpp = data_header.bpp;

   dio_seek_stream( stream, file_header.bmp_offset, SEEK_SET );

   /* Grid starts from top, bitmap starts from bottom. */
   y = grid->sz_y - 1;
   while( bmp_data_size > byte_idx ) {
      /* debug_printf( 1, "bmp: byte_idx %u, bit_idx %u, row %u, col %u (%u)\n",
         byte_idx, bit_idx, y, x, (y * sz_x) + x ); */
      if( 0 == bit_idx % 8 ) {
         dio_read_stream( &byte_buffer, 1, stream );
         byte_idx++;
         bit_idx = 0;
      }

      /* assert( (y * sz_x) + x < (bmp_data_size * (8 / bpp)) ); */

      /* Read this pixel into the grid. */
      for( i = 0 ; bpp > i ; i++ ) {
         grid_data[(y * grid->sz_x) + x] |= byte_buffer & (0x01 << bit_idx);
         bit_idx++;
      }
      grid_data[(y * grid->sz_x) + x] >>= (bit_idx - bpp);

      /* Move to the next pixel. */
      x++;
      if( x >= grid->sz_x ) {
         /* Move to the next row. */
         y--;
         x = 0;
         while( byte_idx % 4 != 0 ) {
            byte_idx++;
         }
         /* Get past the padding. */
      }
   }

cleanup:

   if( NULL == grid_data && NULL != grid ) {
      error_printf( "failed to allocate grid data" );
      memory_unlock( grid_handle );
      memory_free( grid_handle );
      grid = NULL;

   } else if( NULL != grid_data ) {
      memory_unlock( grid->data );
      memory_unlock( grid_handle );
   }

   return grid_handle;
}

