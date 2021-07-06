
#include "cga.h"

#include "../../src/memory.h"
#include "../../src/dio.h"

#include "../convert.h"

const char gc_null = '\0';

/* #define PX_PER_BYTE 8 */
#define PX_PER_BYTE 4

DECLARE_FMT_WRITE( cga ) {
   uint8_t
      * grid_buffer = NULL,
      * grid_data = NULL;
   struct CONVERT_GRID_HEADER* grid = NULL;
   struct CGA_HEADER header;
   int retval = 0;
   uint32_t plane1_start = 0,
      plane2_start = 0;
   uint8_t byte_buffer = 0;
   int32_t
      x = 0,
      y = 0,
      bit_idx = 0,
      grid_idx_even = 0,
      grid_idx_odd = 0,
      byte_idx_even = 0,
      byte_idx_odd = 0,
      byte_idx = 0,
      plane_sz = 0;

   LOCK_CONVERT_GRID( grid_data, grid, grid_handle );

   plane_sz = ((grid->sz_y / 2) * grid->sz_x) / PX_PER_BYTE;
   plane2_start = plane1_start + plane_sz;

   if( o->cga_use_header ) {
      plane1_start += sizeof( struct CGA_HEADER );
      plane2_start = plane1_start + plane_sz;

      debug_printf( 1, "using CGA header...\n" );

      header.type[0] = 'C';
      header.type[1] = 'G';
      header.type[2] = 'A';
      header.type[3] = 'G';
      header.version = 2;
      header.width = grid->sz_x;
      header.height = grid->sz_y;
      header.bpp = o->bpp;
      assert( 2 == header.bpp );
      header.plane1_offset = plane1_start;
      header.plane1_sz = plane_sz;
      header.plane2_offset = plane2_start;
      header.plane2_sz = plane_sz;
      header.palette = 1;
      
      dio_write_stream( &header, sizeof( struct CGA_HEADER ), stream );
   }

   /* Write pixels from grid. */
   for( y = 0 ; grid->sz_y - 1 > y ; y += 2 ) {
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         grid_idx_even = (y * grid->sz_x) + x;
         grid_idx_odd = grid_idx_even + grid->sz_x;

         /* Write the even scanline. */
         byte_idx_even = (((y / 2) * grid->sz_x) + x) / PX_PER_BYTE;
         bit_idx =
            (6 - (((((y / 2) * grid->sz_x) + x) % PX_PER_BYTE) * o->bpp));
         assert( bit_idx < 8 );
         assert( bit_idx >= 0 );
         assert( 0 == (bit_idx % 2) );
         assert( byte_idx_even < (plane1_start + plane_sz ) );
         byte_buffer |= (grid_data[grid_idx_even] << bit_idx);

         if( 8 <= bit_idx ) {
            dio_write_stream( &byte_buffer, 1, stream );
            byte_idx++;
            bit_idx = 0;
            byte_buffer = 0;
         }
      }
   }

   #if 0
         /* Write the odd scanline. */
         byte_idx_odd = (((((grid->sz_y + y) / 2) * grid->sz_x) + x) / 4) +
            o->line_padding;
         assert( byte_idx_odd < buffer_sz );
         buffer[plane1_start + byte_idx_odd] |=
            (grid->data[grid_idx_odd] << bit_idx);
   #endif

cleanup:

   if( NULL != grid ) {
      grid = memory_unlock( grid_handle );
   }

   return retval;
}

DECLARE_FMT_READ( cga ) {
   uint8_t
      * grid_buffer = NULL,
      * grid_data = NULL;
   struct CONVERT_GRID_HEADER* grid = NULL;
   struct CGA_HEADER header;
   int32_t
      bit_idx = 0,
      grid_idx_odd = 0,
      grid_idx_even = 0,
      byte_idx_odd = 0,
      byte_idx_even = 0,
      plane1_offset = 0,
      plane2_offset = 0,
      y = 0,
      x = 0;
   uint8_t byte_buffer = 0;

   if( o->cga_use_header ) {
      dio_read_stream( &header, sizeof( struct CGA_HEADER ), stream );

      NEW_CONVERT_GRID(
         header.width, header.height, 2, grid_data, grid, *grid_handle );

      plane1_offset = header.plane1_offset;
      plane2_offset = header.plane2_offset;
   } else {
      NEW_CONVERT_GRID( o->w, o->h, 2, grid_data, grid, *grid_handle );
      plane2_offset = o->plane_padding;
   }

   /* Image size is w * h * bpp, / 4 px per byte. Planes break / 2. */
   /* 8 makes castle.4 work... why? */
   o->plane_padding = ((o->w * o->h * o->bpp) / PX_PER_BYTE) / 2;

   /* Read pixels into grid. */
   for( y = 0 ; grid->sz_y > y ; y += 2 /* Every other scanline. */ ) {
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         if( 0 == bit_idx % 8 ) {
            dio_read_stream( &byte_buffer, 1, stream );
         }

         /* Calculate linear grid indexes. */
         grid_idx_even = ((y * grid->sz_x) + x);
         grid_idx_odd = grid_idx_even + grid->sz_x; /* Next line. */

         assert( grid_idx_even < grid->data_sz );
         assert( grid_idx_odd < grid->data_sz );

         /* Read the even scanline. */
         /* Divide y by 2 since both planes are SCREEN_H / 2 high. */
         /* Divide result by 4 since it's 2 bits per pixel. */
         byte_idx_even = (((y / 2) * grid->sz_x) + x) / PX_PER_BYTE;
         bit_idx =
            (6 - (((((y / 2) * grid->sz_x) + x) % PX_PER_BYTE) * grid->bpp));
         assert( bit_idx < 8 );
         assert( bit_idx >= 0 );
         assert( 0 == (bit_idx % 2) );
         grid_data[grid_idx_even] |= ((byte_buffer >> bit_idx) & 0x03);

#if 0

         /* Read the odd scanline. */
         byte_idx_odd = (((((grid->sz_y + y) / 2) * grid->sz_x) + x) / 4) +
            o->line_padding;
         assert( byte_idx_odd < buf_sz );
         grid->data[grid_idx_odd] |=
            ((buf[plane1_offset + byte_idx_odd] >> bit_idx) & 0x03);
         debug_printf(
            1, "cga x%02d y%02d new byte %02d, bit %02d (byte %d has %02x)\n",
            x, y, byte_idx_odd, bit_idx,
            plane1_offset + byte_idx_odd,
            buf[plane1_offset + byte_idx_odd] );

         /*
         assert(
            (grid->data[grid_idx_even] & 0xff) ==
            (grid->data[grid_idx_even] & 0x03) );
         */
#endif
      }
   }

   if( o->cga_use_header ) {
      assert( header.plane1_sz == byte_idx_even + 1 );
   }

cleanup:

   if( NULL != grid ) {
      grid = memory_unlock( *grid_handle );
   }

   /* XXX */
   return byte_idx_even;
}

