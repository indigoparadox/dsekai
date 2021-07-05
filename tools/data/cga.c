
#include "cga.h"

#include "../../src/memory.h"
#include "../../src/dio.h"

#include "../convert.h"

const char gc_null = '\0';

/* #define PX_PER_BYTE 8 */
#define PX_PER_BYTE 4

int cga_write(
   struct DIO_STREAM* stream, MEMORY_HANDLE grid_handle,
   struct CONVERT_OPTIONS* o
) {
   int retval = 0;
   struct CGA_HEADER header;
   uint32_t plane1_start = 0,
      plane2_start = 0;
   struct CONVERT_GRID* grid = NULL;
   uint8_t byte_buffer = 0;
   uint8_t* grid_data = NULL;
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

   grid = memory_lock( grid_handle );
   if( NULL == grid ) { goto cleanup; }
   grid_data = memory_lock( grid->data );
   if( NULL == grid_data ) { goto cleanup; }

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

   if( NULL != grid_data ) {
      grid_data = memory_unlock( grid->data );
   }

   if( NULL != grid ) {
      grid = memory_unlock( grid_handle );
   }

   return retval;
}

MEMORY_HANDLE cga_read( struct DIO_STREAM* stream, struct CONVERT_OPTIONS* o ) {
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
   MEMORY_HANDLE grid_handle = NULL;
   uint8_t byte_buffer = 0;
   struct CONVERT_GRID* grid = NULL;
   struct CGA_HEADER header;
   uint8_t* grid_data = NULL;

   if( o->cga_use_header ) {
      dio_read_stream( &header, sizeof( struct CGA_HEADER ), stream );
   }

   /* Allocate new grid. */
   grid_handle = memory_alloc( 1, sizeof( struct CONVERT_GRID ) );
   if( NULL == grid_handle ) { goto cleanup; }
   grid = memory_lock( grid_handle );
   if( NULL == grid ) { goto cleanup; }

   assert( NULL != grid );
   if( o->cga_use_header ) {
      grid->sz_x = header.width;
      grid->sz_y = header.height;
      grid->data_sz = header.width * header.height;
      plane1_offset = header.plane1_offset;
      plane2_offset = header.plane2_offset;
   } else {
      grid->sz_x = o->w;
      grid->sz_y = o->h;
      grid->data_sz = o->w * o->h;
      plane2_offset = o->plane_padding;
   }
   assert( 0 < grid->sz_x );
   assert( 0 < grid->sz_y );

   grid->data = memory_alloc( 1, grid->data_sz );
   if( NULL == grid->data ) { goto cleanup; }
   grid_data = memory_lock( grid->data );
   if( NULL == grid_data ) { goto cleanup; }

   grid->bpp = 2; /* CGA is 2bpp or we don't understand it. */

   /* Image size is w * h * bpp, / 4 px per byte. Planes break / 2. */
   /* 8 makes castle.4 work... why? */
   o->plane_padding = ((o->w * o->h * o->bpp) / PX_PER_BYTE) / 2;

   /* Read pixels into grid. */
   for( y = 0 ; grid->sz_y > y ; y += 2 /* Every other scanline. */ ) {
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         if( 0 == bit_idx % 8 ) {
            dio_write_stream( &byte_buffer, 1, stream );
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

