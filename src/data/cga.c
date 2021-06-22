
#include "cga.h"

int cga_write( const char* path, const struct CONVERT_GRID* grid, int lp ) {
   int retval = 0;

   return retval;
}

#define CGA_BPP 2

struct CONVERT_GRID* cga_read( const char* path, int sz_x, int sz_y, int lp ) {
   FILE* cga_file = NULL;
   size_t read = 0,
      cga_file_sz = 0,
      byte_idx = 0,
      bit_idx = 0,
      grid_idx_odd = 0,
      grid_idx_even = 0,
      i = 0,
      y = 0,
      x = 0;
   uint8_t* raw_cga_data = NULL;
   uint8_t byte_buffer_odd = 0,
      byte_buffer_even = 0;
   struct CONVERT_GRID* grid = NULL;

   cga_file = fopen( path, "rb" );
   assert( NULL != cga_file );

   fseek( cga_file, 0, SEEK_END );
   cga_file_sz = ftell( cga_file );

   fseek( cga_file, 0, SEEK_SET );

   raw_cga_data = calloc( 1, cga_file_sz + 1 );
   assert( NULL != raw_cga_data );

   read = fread( raw_cga_data, 1, cga_file_sz, cga_file );
   assert( read == cga_file_sz );

   convert_printf( "read CGA file: %lu bytes\n", cga_file_sz );

   grid = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid );
   grid->data_sz = sz_x * sz_y;
   grid->data = calloc( 1, grid->data_sz );
   assert( NULL != grid->data );

   grid->sz_x = sz_x;
   grid->sz_y = sz_y;
   grid->bpp = 2;

   printf( "%d\n", lp );

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

         /* Read the even scanline. */
         grid->data[grid_idx_even] |= /* Little endian, so reverse bit_idx. */
            raw_cga_data[byte_idx] & (0x03 << (6 - bit_idx));
         grid->data[grid_idx_even] >>= (6 - bit_idx);

         /* Read the odd scanline. */
         grid->data[grid_idx_odd] |= /* Little endian, so reverse bit_idx. */
            raw_cga_data[byte_idx + lp] & (0x03 << (6 - bit_idx));
         grid->data[grid_idx_odd] >>= (6 - bit_idx);

         /* Advance the bit index by one pixel. */
         bit_idx += grid->bpp;
      }
   }

   fclose( cga_file );

   free( raw_cga_data );

   return grid;
}

