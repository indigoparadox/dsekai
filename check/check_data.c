
#include <check.h>

#include "../tools/convert.h"
#include "../tools/data/cga.h"
#include "../tools/data/bmp.h"
#include "../tools/data/ico.h"
#include "../tools/data/icns.h"

#include "testdata.h"

void buffer_printf( uint8_t* buffer, int start, int end, int col_break ) {
   int column_idx = 1,
      i = 0;

   for( i = start ; end > i ; i++ ) {
      if( 0 != buffer[i] ) {
         printf( "0x%02x, ", buffer[i] );
      } else {
         printf( "    , " );
      }

      if( 0 == column_idx % col_break ) {
         printf( "\n" );
      }
      column_idx++;
   }

   printf( "\n\n" );
}

START_TEST( check_data_cga_read ) {
   struct CONVERT_GRID* grid = NULL;
   struct CONVERT_OPTIONS options;

   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.cga_use_header = 1;

   grid = cga_read(
      (const uint8_t*)&gc_test_cga_16_16_4, TEST_CGA_16_16_4_SZ, &options );

   ck_assert_int_eq( gc_test_grid_16_16_4.sz_x, grid->sz_x );
   ck_assert_int_eq( gc_test_grid_16_16_4.sz_y, grid->sz_y );
   ck_assert_int_eq( gc_test_grid_16_16_4.bpp, grid->bpp );
   ck_assert_int_eq( gc_test_grid_16_16_4.data_sz, grid->data_sz );
   ck_assert_int_eq( gc_test_grid_16_16_4_data[_i], grid->data[_i] );

   /* dio_print_grid( grid ); */

   free( grid->data );
   free( grid );
}
END_TEST

START_TEST( check_data_cga_write ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_CGA_16_16_4_SZ];
   struct CGA_HEADER* cga_header = (struct CGA_HEADER*)buffer,
      * test_header = (struct CGA_HEADER*)gc_test_cga_16_16_4;

   memset( &buffer, '\0', TEST_CGA_16_16_4_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.cga_use_header = 1;
   options.bpp = 2;

   cga_write( 
      (uint8_t*)&buffer, TEST_CGA_16_16_4_SZ, &gc_test_grid_16_16_4, &options );

   ck_assert_int_eq( test_header->version, cga_header->version );
   ck_assert_int_eq( test_header->width, cga_header->width );
   ck_assert_int_eq( test_header->height, cga_header->height );
   ck_assert_int_eq( test_header->bpp, cga_header->bpp );
   ck_assert_int_eq( test_header->plane1_offset, cga_header->plane1_offset );
   ck_assert_int_eq( CGA_HEADER_SZ, cga_header->plane1_offset );
   ck_assert_int_eq( gc_test_cga_16_16_4[_i], buffer[_i] );
}
END_TEST

START_TEST( check_data_icns_read ) {
   struct CONVERT_GRID* grid = NULL;
   struct CONVERT_OPTIONS options;

   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );

   grid = icns_read(
      (const uint8_t*)&gc_test_icns_16_16_2, TEST_CGA_16_16_4_SZ, &options );

   ck_assert_int_eq( gc_test_grid_16_16_4.sz_x, grid->sz_x );
   ck_assert_int_eq( gc_test_grid_16_16_4.sz_y, grid->sz_y );
   ck_assert_int_eq( gc_test_grid_16_16_4.bpp, 2 );
   ck_assert_int_eq( gc_test_grid_16_16_4.data_sz, grid->data_sz );
   ck_assert_int_eq( (gc_test_grid_16_16_4_data[_i] & 0x01), grid->data[_i] );

   /* dio_print_grid( grid ); */

   free( grid->data );
   free( grid );
}
END_TEST

START_TEST( check_data_icns_write_data ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_ICNS_16_16_2_SZ];
   struct ICNS_FILE_HEADER* file_header = (struct ICNS_FILE_HEADER*)buffer,
      * test_header = (struct ICNS_FILE_HEADER*)gc_test_icns_16_16_2;

   memset( &buffer, '\0', TEST_ICNS_16_16_2_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 1;
   options.reverse = 0;

   icns_write( 
      (uint8_t*)&buffer, TEST_ICNS_16_16_2_SZ,
      &gc_test_grid_16_16_4, &options );

   ck_assert_int_eq(
      dio_reverse_endian_32( test_header->file_sz ),
      dio_reverse_endian_32( file_header->file_sz ) );
   ck_assert_int_eq(
      gc_test_icns_16_16_2[ICNS_FILE_HEADER_SZ + ICNS_DATA_HEADER_SZ + _i],
      buffer[ICNS_FILE_HEADER_SZ + ICNS_DATA_HEADER_SZ +_i] );
}
END_TEST

START_TEST( check_data_bmp_write_header ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_BMP_16_16_16_SZ];
   struct BITMAP_FILE_HEADER* file_header = (struct BITMAP_FILE_HEADER*)buffer,
      * test_f_header = (struct BITMAP_FILE_HEADER*)gc_test_bmp_16_16_16;
   struct BITMAP_DATA_HEADER* data_header = (struct BITMAP_DATA_HEADER*)buffer,
      * test_d_header = (struct BITMAP_DATA_HEADER*)&(gc_test_bmp_16_16_16[
         sizeof( struct BITMAP_FILE_HEADER )]);

   memset( &buffer, '\0', TEST_BMP_16_16_16_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 4;

   bmp_write( 
      (uint8_t*)&buffer, TEST_BMP_16_16_16_SZ,
      &gc_test_grid_16_16_4, &options );

}
END_TEST

START_TEST( check_data_bmp_write_data ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_BMP_16_16_16_SZ];
   struct BITMAP_FILE_HEADER* file_header = (struct BITMAP_FILE_HEADER*)buffer,
      * test_f_header = (struct BITMAP_FILE_HEADER*)gc_test_bmp_16_16_16;
   struct BITMAP_DATA_HEADER* data_header = (struct BITMAP_DATA_HEADER*)buffer,
      * test_d_header = (struct BITMAP_DATA_HEADER*)&(gc_test_bmp_16_16_16[
         sizeof( struct BITMAP_FILE_HEADER )]);

   memset( &buffer, '\0', TEST_BMP_16_16_16_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 4;

   bmp_write( 
      (uint8_t*)&buffer, TEST_BMP_16_16_16_SZ,
      &gc_test_grid_16_16_4, &options );

   /* ck_assert_int_eq( test_f_header->file_sz, file_header->file_sz ); */
   ck_assert_int_eq(
      gc_test_bmp_16_16_16[
         sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ) + _i],
      buffer[
         sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ) + _i] );
}
END_TEST

START_TEST( check_data_ico_write_header ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_ICO_16_16_16_SZ];
   struct ICO_DIR_HEADER* file_header = (struct ICO_DIR_HEADER*)buffer,
      * test_f_header = (struct ICO_DIR_HEADER*)gc_test_ico_16_16_16;
   struct ICO_DIR_ENTRY_HEADER* data_header =
      (struct ICO_DIR_ENTRY_HEADER*)&(buffer[
         sizeof( struct ICO_DIR_HEADER )]),
      * test_d_header = (struct ICO_DIR_ENTRY_HEADER*)&(gc_test_ico_16_16_16[
         sizeof( struct ICO_DIR_HEADER )]);
   struct BITMAP_DATA_HEADER* bmp_header =
      (struct BITMAP_DATA_HEADER*)&(buffer[
         sizeof( struct ICO_DIR_HEADER ) + sizeof( struct ICO_DIR_ENTRY_HEADER )
      ]),
      * test_b_header = (struct BITMAP_DATA_HEADER*)&(gc_test_ico_16_16_16[
         sizeof( struct ICO_DIR_HEADER ) + sizeof( struct ICO_DIR_ENTRY_HEADER )
      ]);

   memset( &buffer, '\0', TEST_ICO_16_16_16_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 4;

   ico_write( 
      (uint8_t*)&buffer, TEST_ICO_16_16_16_SZ,
      &gc_test_grid_16_16_4, &options );

   ck_assert_int_eq( test_f_header->type, file_header->type );
   ck_assert_int_eq( test_f_header->count, file_header->count );

   ck_assert_int_eq( test_d_header->ico_w, data_header->ico_w );
   ck_assert_int_eq( test_d_header->ico_h, data_header->ico_h );
   ck_assert_int_eq( test_d_header->colors, data_header->colors );
   ck_assert_int_eq(
      test_d_header->ico_planes_cur_hotspot_h,
      data_header->ico_planes_cur_hotspot_h );
   ck_assert_int_eq(
      test_d_header->ico_bpp_cur_hotspot_v,
      data_header->ico_bpp_cur_hotspot_v );
   ck_assert_int_eq( test_d_header->data_sz, data_header->data_sz );
   ck_assert_int_eq( test_d_header->bmp_offset, data_header->bmp_offset );

   ck_assert_int_eq( test_b_header->bitmap_w,
      bmp_header->bitmap_w );
   ck_assert_int_eq( test_b_header->bitmap_h,
      bmp_header->bitmap_h );
   ck_assert_int_eq( test_b_header->bpp,
      bmp_header->bpp );
   ck_assert_int_eq( test_b_header->image_sz,
      bmp_header->image_sz );
   ck_assert_int_eq( test_b_header->colors,
      bmp_header->colors );
}
END_TEST

START_TEST( check_data_ico_write_data ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_ICO_16_16_16_SZ];
   struct ICO_DIR_HEADER* file_header = (struct ICO_DIR_HEADER*)buffer,
      * test_f_header = (struct ICO_DIR_HEADER*)gc_test_ico_16_16_16;
   struct ICO_DIR_ENTRY_HEADER* data_header =
         (struct ICO_DIR_ENTRY_HEADER*)buffer,
      * test_d_header = (struct ICO_DIR_ENTRY_HEADER*)&(gc_test_ico_16_16_16[
         sizeof( struct ICO_DIR_HEADER )]);

   memset( &buffer, '\0', TEST_ICO_16_16_16_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 4;

   ico_write( 
      (uint8_t*)&buffer, TEST_ICO_16_16_16_SZ,
      &gc_test_grid_16_16_4, &options );

   /* ck_assert_int_eq( test_f_header->file_sz, file_header->file_sz ); */
   ck_assert_int_eq(
      gc_test_ico_16_16_16[
         sizeof( struct ICO_DIR_HEADER ) +
         sizeof( struct ICO_DIR_ENTRY_HEADER ) + _i],
      buffer[
         sizeof( struct ICO_DIR_HEADER ) +
         sizeof( struct ICO_DIR_ENTRY_HEADER ) + _i] );
}
END_TEST

#if 0
START_TEST( check_data_ico_write ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_ICO_16_16_16_SZ];
   struct ICNS_FILE_HEADER* file_header = (struct ICNS_FILE_HEADER*)buffer,
      * test_header = (struct ICNS_FILE_HEADER*)gc_test_icns_16_16_2;

   memset( &buffer, '\0', TEST_ICNS_16_16_2_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 1;

   icns_write( 
      (uint8_t*)&buffer, TEST_ICNS_16_16_2_SZ,
      &gc_test_grid_16_16_4, &options );

   ck_assert_int_eq(
      dio_reverse_endian_32( test_header->file_sz ),
      dio_reverse_endian_32( file_header->file_sz ) );
   ck_assert_int_eq(
      gc_test_icns_16_16_2[ICNS_FILE_HEADER_SZ + ICNS_DATA_HEADER_SZ + _i],
      buffer[ICNS_FILE_HEADER_SZ + ICNS_DATA_HEADER_SZ +_i] );
}
END_TEST
#endif

#if 0
START_TEST( check_data_json_parse ) {
   json_parse_buffer( &gc_test_json, 0 );
}
END_TEST
#endif

Suite* data_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "data" );

   /* Core test case */
   tc_core = tcase_create( "Core" );

   tcase_add_loop_test( tc_core, check_data_cga_read, 0, 256 );
   tcase_add_loop_test( tc_core, check_data_cga_write, 0, TEST_CGA_16_16_4_SZ );
   tcase_add_loop_test( tc_core, check_data_icns_read, 0, 256 );
   tcase_add_loop_test(
      tc_core, check_data_icns_write_data, 0, TEST_ICNS_16_16_2_DATA_SZ );

   tcase_add_test( tc_core, check_data_bmp_write_header );
   tcase_add_loop_test(
      tc_core, check_data_bmp_write_data, 
      sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ),
      sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ) + ((16 * 16) / 2) );

   tcase_add_test( tc_core, check_data_ico_write_header );
   tcase_add_loop_test(
      tc_core, check_data_ico_write_data, 
      sizeof( struct ICO_DIR_HEADER ) +
         sizeof( struct ICO_DIR_ENTRY_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ),
      sizeof( struct ICO_DIR_HEADER ) +
         sizeof( struct ICO_DIR_HEADER ) + 
         sizeof( struct BITMAP_DATA_HEADER ) +
         ((16 * 16) / 2) );
#if 0
   tcase_add_test( tc_core, check_data_json_parse );
#endif

   suite_add_tcase( s, tc_core );

   return s;
}

