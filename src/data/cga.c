
#include "cga.h"

const char gc_null = '\0';

uint32_t cga_write_plane(
   uint8_t* buffer, uint32_t buffer_sz,
   const struct CONVERT_GRID* grid, int y_offset
) {
   size_t
      x = 0,
      y = 0,
      bit_idx = 0,
      grid_idx = 0;
   uint8_t byte_buffer = 0;
   uint32_t buffer_offset = 0;

   /* Write even pixels from grid. */
   for( y = y_offset ; grid->sz_y > y ; y += 2 /* Every other scanline. */ ) {
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         grid_idx = (y * grid->sz_x) + x;

         if( 0 == bit_idx % 8 && 0 != bit_idx ) {
            /* Write current byte and start a new one. */
            bit_idx = 0;
            buffer[buffer_offset] = byte_buffer;
            buffer_offset++;
            byte_buffer = 0;
         }

         assert( grid_idx < grid->data_sz );

         /* Write the scanline. */
         byte_buffer |= ((grid->data[grid_idx] & (0x03)) << (6 - bit_idx));

         /* Advance the bit index by one pixel. */
         bit_idx += grid->bpp;
      }
   }
   buffer[buffer_offset++] = byte_buffer;

   return buffer_offset;
}

int cga_write_file(
   const char* path, const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   FILE* cga_file = NULL;
   int retval = 0;
   uint8_t* cga_buffer = NULL;
   uint32_t cga_buffer_sz = 0;

   cga_buffer_sz = 
      (((grid->sz_y / 2) * (grid->sz_x * o->bpp)) / 8) / 2;
   cga_buffer_sz += (2 * o->line_padding);
   convert_printf( "CGA buffer size: %u\n", cga_buffer_sz );

   cga_buffer = calloc( 1, cga_buffer_sz );
   assert( NULL != cga_buffer );

   cga_file = fopen( path, "wb" );
   assert( NULL != cga_file );

   retval = cga_write( cga_buffer, cga_buffer_sz, grid, o );

   fwrite( cga_buffer, 1, cga_buffer_sz, cga_file );

   convert_printf( "wrote CGA file: %lu bytes\n", ftell( cga_file ) );

   free( cga_buffer );
   fclose( cga_file );

   return retval;
}

int cga_write(
   uint8_t* buffer, uint32_t buffer_sz,
   const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   int retval = 0;

   cga_write_plane( buffer, buffer_sz, grid, 0 );

   cga_write_plane( &(buffer[o->plane_padding + o->line_padding]),
      buffer_sz - o->plane_padding, grid, 1 ) ;

   return retval;
}

struct CONVERT_GRID* cga_read_file(
   const char* path, struct CONVERT_OPTIONS* o
) {
   uint8_t* cga_buffer = NULL;
   uint32_t cga_buffer_sz = 0;
   struct CONVERT_GRID* grid_out = NULL;

   cga_buffer_sz = convert_read_file( path, &cga_buffer );

   grid_out = cga_read( cga_buffer, cga_buffer_sz, o );

   free( cga_buffer );

   return grid_out;
}

struct CONVERT_GRID* cga_read(
   uint8_t* buf, uint32_t buf_sz, struct CONVERT_OPTIONS* o
) {
   size_t read = 0,
      byte_idx = 0,
      bit_idx = 0,
      grid_idx_odd = 0,
      grid_idx_even = 0,
      i = 0,
      y = 0,
      x = 0;
   uint8_t byte_buffer_odd = 0,
      byte_buffer_even = 0;
   struct CONVERT_GRID* grid = NULL;

   /* Allocate new grid. */
   grid = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid );
   grid->data_sz = o->w * o->h;
   grid->data = calloc( 1, grid->data_sz );
   assert( NULL != grid->data );
   grid->sz_x = o->w;
   grid->sz_y = o->h;
   grid->bpp = 2; /* CGA is 2bpp or we don't understand it. */

   /* Read pixels into grid. */
   for( y = 0 ; grid->sz_y > y ; y += 2 /* Every other scanline. */ ) {
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         grid_idx_even = (y * grid->sz_x) + x;
         grid_idx_odd = grid_idx_even + grid->sz_x; /* Next line. */

         if( 0 == bit_idx % 8 && 0 != bit_idx ) {
            bit_idx = 0;
            byte_idx++;
         }

         assert( grid_idx_even < grid->data_sz );
         assert( grid_idx_odd < grid->data_sz );
         assert( byte_idx < buf_sz );

         /* Read the even scanline. */
         grid->data[grid_idx_even] |= /* Little endian, so reverse bit_idx. */
            buf[byte_idx] & (0x03 << (6 - bit_idx));
         grid->data[grid_idx_even] >>= (6 - bit_idx);

         /* Read the odd scanline. */
         grid->data[grid_idx_odd] |= /* Little endian, so reverse bit_idx. */
            buf[byte_idx + o->plane_padding + o->line_padding] & 
               (0x03 << (6 - bit_idx));
         grid->data[grid_idx_odd] >>= (6 - bit_idx);

         /* Advance the bit index by one pixel. */
         bit_idx += grid->bpp;
      }
   }

   return grid;
}

