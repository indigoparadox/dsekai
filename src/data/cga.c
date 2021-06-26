
#include "cga.h"

#include "../memory.h"
#include "dio.h"

const char gc_null = '\0';

/* #define PX_PER_BYTE 8 */
#define PX_PER_BYTE 4

int cga_write_file(
   const char* path, const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   FILE* cga_file = NULL;
   int retval = 0;
   uint8_t* cga_buffer = NULL;
   uint32_t cga_buffer_sz = 0,
      cga_buffer_sz_raw = 0;

   /* Output BPP is always 2 for now. */
   o->bpp = 2;

   /* Determine the buffer size. */
   /* x * y size for total pixels / 4 (since 8 / 2 = 4) */
   cga_buffer_sz_raw = (grid->sz_x * grid->sz_y) / PX_PER_BYTE;
   cga_buffer_sz = cga_buffer_sz_raw + (2 * o->line_padding);
   /* cga_buffer_sz += 58; */ /* ??? */
   o->plane_padding = cga_buffer_sz / 2; /* Plane pads halfway in. */
   if( o->cga_use_header ) {
      printf( "use header\n" );
      cga_buffer_sz += CGA_HEADER_SZ + 1;
   }
   printf( "CGA buffer size: %u\n", cga_buffer_sz );

   cga_buffer = memory_alloc( 1, cga_buffer_sz );
   assert( NULL != cga_buffer );

   /* Perform the conversion and write the result to file. */
   cga_file = fopen( path, "wb" );
   assert( NULL != cga_file );
   retval = cga_write( cga_buffer, cga_buffer_sz, grid, o );
   fwrite( cga_buffer, 1, cga_buffer_sz, cga_file );
   dio_printf( "wrote CGA file: %lu bytes\n", ftell( cga_file ) );

   fclose( cga_file );
   memory_free( &cga_buffer );

   return retval;
}

int cga_write(
   uint8_t* buffer, uint32_t buffer_sz,
   const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   int retval = 0;
   struct CGA_HEADER* header = (struct CGA_HEADER*)buffer;
   uint8_t byte_buffer_even = 0,
      byte_buffer_odd;
   uint32_t plane1_start = 0,
      plane2_start = 0;
   int32_t
      x = 0,
      y = 0,
      i = 0,
      bit_idx = 0,
      grid_idx_even = 0,
      grid_idx_odd = 0,
      byte_idx_even = 0,
      byte_idx_odd = 0,
      last_byte_idx = 0,
      plane_sz = 0;

   plane_sz = ((grid->sz_y / 2) * grid->sz_x) / PX_PER_BYTE;
   plane2_start = plane1_start + plane_sz;

   if( o->cga_use_header ) {
      plane1_start += CGA_HEADER_SZ;
      plane2_start = plane1_start + plane_sz;

      dio_printf( "using CGA header...\n" );

      buffer[0] = 'C';
      buffer[1] = 'G';
      header->version = 2;
      header->width = grid->sz_x;
      header->height = grid->sz_y;
      header->bpp = o->bpp;
      assert( 2 == header->bpp );
      header->plane1_offset = plane1_start;
      header->plane1_sz = plane_sz;
      header->plane2_offset = plane2_start;
      header->plane2_sz = plane_sz;
      header->palette = 1;
      header->endian = o->little_endian;
   }

   /* Write pixels from grid. */
   for( y = 0 ; grid->sz_y - 1 > y ; y += 2 ) {
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         if( o->little_endian ) {
            if( x >= (grid->sz_x / 2) ) {
               grid_idx_even = (y * grid->sz_x) + 
                  (x - (grid->sz_x / 2));
               grid_idx_odd = grid_idx_even + grid->sz_x +
                  (x - (grid->sz_x / 2));
            } else {
               grid_idx_even = (y * grid->sz_x) + 
                  (x + (grid->sz_x / 2));
               grid_idx_odd = grid_idx_even + grid->sz_x +
                  (x + (grid->sz_x / 2));
            }
         } else {
            grid_idx_even = (y * grid->sz_x) + x;
            grid_idx_odd = grid_idx_even + grid->sz_x;
         }

         /* Write the even scanline. */
         byte_idx_even = (((y / 2) * grid->sz_x) + x) / 4;
         /* assert( byte_idx_even < buf_sz ); */
         bit_idx = (6 - (((((y / 2) * grid->sz_x) + x) % 4) * o->bpp));
         assert( bit_idx < 8 );
         assert( bit_idx >= 0 );
         assert( 0 == (bit_idx % 2) );
          assert( byte_idx_even < (plane1_start + plane_sz ) );
         assert( byte_idx_even < buffer_sz );
         buffer[plane1_start + byte_idx_even] |= 
            (grid->data[grid_idx_even] << bit_idx);
         dio_printf(
            "cga x%02d y%02d new byte %02d, bit %02d (byte %d has %02x)\n",
            x, y, byte_idx_even, bit_idx,
            plane1_start + byte_idx_even,
            buffer[plane1_start + byte_idx_even] );

         /* Write the odd scanline. */
         byte_idx_odd = (((((grid->sz_y + y) / 2) * grid->sz_x) + x) / 4) +
            o->line_padding;
         assert( byte_idx_odd < buffer_sz );
         buffer[plane1_start + byte_idx_odd] |=
            (grid->data[grid_idx_odd] << bit_idx);
      }
   }

   return retval;
}

struct CONVERT_GRID* cga_read_file(
   const char* path, struct CONVERT_OPTIONS* o
) {
   uint8_t* cga_buffer = NULL;
   uint32_t cga_buffer_sz = 0;
   struct CONVERT_GRID* grid_out = NULL;

   cga_buffer_sz = dio_read_file( path, &cga_buffer );

   grid_out = cga_read( cga_buffer, cga_buffer_sz, o );

   memory_free( &cga_buffer );

   return grid_out;
}

struct CONVERT_GRID* cga_read(
   const uint8_t* buf, uint32_t buf_sz, struct CONVERT_OPTIONS* o
) {
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
   struct CONVERT_GRID* grid = NULL;
   struct CGA_HEADER header;

   if( o->cga_use_header ) {
      memcpy( &header, buf, sizeof( struct CGA_HEADER ) );
   }

   /* Allocate new grid. */
   grid = memory_alloc( 1, sizeof( struct CONVERT_GRID ) );
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
   assert( NULL != grid->data );
   grid->bpp = 2; /* CGA is 2bpp or we don't understand it. */

   /* Image size is w * h * bpp, / 4 px per byte. Planes break / 2. */
   /* 8 makes castle.4 work... why? */
   o->plane_padding = ((o->w * o->h * o->bpp) / PX_PER_BYTE) / 2;

   /* Read pixels into grid. */
   for( y = 0 ; grid->sz_y > y ; y += 2 /* Every other scanline. */ ) {
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         /* Calculate linear grid indexes. */
         grid_idx_even = ((y * grid->sz_x) + x);
         grid_idx_odd = grid_idx_even + grid->sz_x; /* Next line. */

         assert( grid_idx_even < grid->data_sz );
         assert( grid_idx_odd < grid->data_sz );

         /* Read the even scanline. */
         /* Divide y by 2 since both planes are SCREEN_H / 2 high. */
         /* Divide result by 4 since it's 2 bits per pixel. */
         byte_idx_even = (((y / 2) * grid->sz_x) + x) / PX_PER_BYTE;
         assert( byte_idx_even < buf_sz );
         bit_idx =
            (6 - (((((y / 2) * grid->sz_x) + x) % PX_PER_BYTE) * grid->bpp));
         assert( bit_idx < 8 );
         assert( bit_idx >= 0 );
         assert( 0 == (bit_idx % 2) );
         grid->data[grid_idx_even] |=
            ((buf[plane1_offset + byte_idx_even] >> bit_idx) & 0x03);
         dio_printf(
            "cga x%02d y%02d new byte %02d, bit %02d (byte %d has %02x)\n",
            x, y, byte_idx_even, bit_idx,
            plane1_offset + byte_idx_even,
            buf[plane1_offset + byte_idx_even] );

         /* Read the odd scanline. */
         byte_idx_odd = (((((grid->sz_y + y) / 2) * grid->sz_x) + x) / 4) +
            o->line_padding;
         assert( byte_idx_odd < buf_sz );
         grid->data[grid_idx_odd] |=
            ((buf[plane1_offset + byte_idx_odd] >> bit_idx) & 0x03);
         dio_printf(
            "cga x%02d y%02d new byte %02d, bit %02d (byte %d has %02x)\n",
            x, y, byte_idx_even, bit_idx,
            plane1_offset + byte_idx_even,
            buf[plane1_offset + byte_idx_even] );

         assert(
            (grid->data[grid_idx_even] & 0xff) ==
            (grid->data[grid_idx_even] & 0x03) );
      }
      dio_printf( "---\n" );
   }

   if( o->cga_use_header ) {
      assert( header.plane1_sz == byte_idx_even + 1 );
   }

   return grid;
}

