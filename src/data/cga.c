
#include "cga.h"

const char gc_null = '\0';

uint32_t cga_write_plane(
   const struct CONVERT_GRID* grid, FILE* cga_file, int y_offset
) {
   size_t
      x = 0,
      y = 0,
      bit_idx = 0,
      grid_idx = 0,
      bytes_out = 0;
   uint8_t byte_buffer = 0;

   /* Write even pixels from grid. */
   for( y = y_offset ; grid->sz_y > y ; y += 2 /* Every other scanline. */ ) {
      printf( "%d\n", y );
      for( x = 0 ; grid->sz_x > x ; x++ ) {
         grid_idx = (y * grid->sz_x) + x;

         if( 0 == bit_idx % 8 && 0 != bit_idx ) {
            /* Write current byte and start a new one. */
            bit_idx = 0;
            fwrite( &byte_buffer, 1, 1, cga_file );
            bytes_out++;
            byte_buffer = 0;
         }

         assert( grid_idx < grid->data_sz );

         /* Write the even scanline. */
         byte_buffer |= ((grid->data[grid_idx] & (0x03)) << (6 - bit_idx));

         /* Advance the bit index by one pixel. */
         bit_idx += grid->bpp;
      }
   }
   fwrite( &byte_buffer, 1, 1, cga_file );

   return bytes_out;
}

int cga_write(
   const char* path, const struct CONVERT_GRID* grid, int bpp, int pp, int lp
) {
   int retval = 0;
   FILE* cga_file = NULL;
   size_t padding_end = 0;

   cga_file = fopen( path, "wb" );
   assert( NULL != cga_file );

   cga_write_plane( grid, cga_file, 0 );

   while( ftell( cga_file ) < (lp + pp) ) {
      fwrite( &gc_null, 1, 1, cga_file );
   }

   cga_write_plane( grid, cga_file, 1 );

   padding_end = 0;
   while( padding_end < lp ) {
      fwrite( &gc_null, 1, 1, cga_file );
      padding_end++;
   }

   convert_printf( "wrote CGA file: %lu bytes\n", ftell( cga_file ) );

   fclose( cga_file );

   return retval;
}

struct CONVERT_GRID* cga_read(
   const char* path, int sz_x, int sz_y, int bpp, int pp, int lp
) {
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

   /* Allocate new grid. */
   grid = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid );
   grid->data_sz = sz_x * sz_y;
   grid->data = calloc( 1, grid->data_sz );
   assert( NULL != grid->data );
   grid->sz_x = sz_x;
   grid->sz_y = sz_y;
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
         assert( byte_idx < cga_file_sz );

         /* Read the even scanline. */
         grid->data[grid_idx_even] |= /* Little endian, so reverse bit_idx. */
            raw_cga_data[byte_idx] & (0x03 << (6 - bit_idx));
         grid->data[grid_idx_even] >>= (6 - bit_idx);

         /* Read the odd scanline. */
         grid->data[grid_idx_odd] |= /* Little endian, so reverse bit_idx. */
            raw_cga_data[byte_idx + pp + lp] & (0x03 << (6 - bit_idx));
         grid->data[grid_idx_odd] >>= (6 - bit_idx);

         /* Advance the bit index by one pixel. */
         bit_idx += grid->bpp;
      }
   }

   fclose( cga_file );

   free( raw_cga_data );

   return grid;
}

