
#include <check.h>

#include "../src/data/drc.h"
#include "../tools/data/drcwrite.h"

#define DRC_CKBUFFER_SZ 10240

uint8_t g_buffer_r[DRC_CKBUFFER_SZ];
uint8_t g_buffer_w[DRC_CKBUFFER_SZ];
struct DIO_STREAM g_drc_stream_r;
struct DIO_STREAM g_drc_stream_w;

static void drc_setup() {
   /* dio_remove_file( "/tmp/drc_check.drc" );
   dio_open_stream_file( "/tmp/drc_check.drc", ); */
   memset( &g_buffer_w, '\0', DRC_CKBUFFER_SZ );
   memset( &g_buffer_r, '\0', DRC_CKBUFFER_SZ );
   dio_open_stream_buffer( g_buffer_r, DRC_CKBUFFER_SZ, &g_drc_stream_r );
   dio_open_stream_buffer( g_buffer_w, DRC_CKBUFFER_SZ, &g_drc_stream_w );
   ck_assert_int_eq( 1, dio_type_stream( &g_drc_stream_r ) );
   ck_assert_int_eq( 1, dio_type_stream( &g_drc_stream_w ) );
}

static void drc_teardown() {
   dio_close_stream( &g_drc_stream_r );
   dio_close_stream( &g_drc_stream_w );
}

START_TEST( check_drc_create ) {
   struct DRC_HEADER* header = (struct DRC_HEADER*)&g_buffer_w;
   drc_create( &g_drc_stream_w );
   ck_assert_int_eq(
      dio_tell_stream( &g_drc_stream_w ), sizeof( struct DRC_HEADER ) );
   ck_assert_int_eq( header->version, 1 );
   ck_assert_int_eq( header->filesize, sizeof( struct DRC_HEADER ) );
}
END_TEST

START_TEST( check_drc_add ) {
   int i = 0;
   union DRC_TYPE type;
   char test_string[] = "This is a test string.";
   struct DRC_HEADER* header = (struct DRC_HEADER*)&g_buffer_w;
   struct DRC_TOC_E* toc_e =
      (struct DRC_HEADER*)&(g_buffer_w[sizeof( struct DRC_HEADER )]);

   type.str[0] = 'B';
   type.str[1] = 'M';
   type.str[2] = 'P';
   type.str[3] = '1';

   drc_create( &g_drc_stream_r );

   drc_add_resource(
      &g_drc_stream_r, &g_drc_stream_w, type, 5001, "test.bmp", 8,
      test_string, 22 );

   for( i = 0 ; 20 > i ; i++ ) {
      printf( "%02x ", g_buffer_r[i] );
   }
   printf( "\n" );

   for( i = 0 ; 20 > i ; i++ ) {
      printf( "%02x ", g_buffer_w[i] );
   }
   printf( "\n" );

   ck_assert_int_eq( toc_e->type.u32, type.u32 );
   
   ck_assert_int_eq(
      sizeof( struct DRC_HEADER ) + sizeof( struct DRC_TOC_E ) + 22,
      dio_tell_stream( &g_drc_stream_w ) );
}
END_TEST

Suite* drc_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "drc" );

   /* Core test case */
   tc_core = tcase_create( "Core" );
   tcase_add_checked_fixture( tc_core, drc_setup, drc_teardown );

   tcase_add_test( tc_core, check_drc_create ); 
   tcase_add_test( tc_core, check_drc_add ); 

   suite_add_tcase( s, tc_core );

   return s;
}

