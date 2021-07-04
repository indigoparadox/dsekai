
#include <check.h>

#include "../src/dio.h"

START_TEST( check_dio_atoi_one ) {
   char test_a[] = "1";
   int16_t test_i = 0;

   test_i = dio_atoi( test_a, 10 );

   ck_assert_int_eq( test_i, 1 );
}
END_TEST

START_TEST( check_dio_atoi_ten ) {
   char test_a[] = "10";
   int16_t test_i = 0;

   test_i = dio_atoi( test_a, 10 );

   ck_assert_int_eq( test_i, 10 );
}
END_TEST

START_TEST( check_dio_atoi_hundred ) {
   char test_a[] = "100";
   int16_t test_i = 0;

   test_i = dio_atoi( test_a, 10 );

   ck_assert_int_eq( test_i, 100 );
}
END_TEST

START_TEST( check_dio_atoi_hundred_fifty ) {
   char test_a[] = "150";
   int16_t test_i = 0;

   test_i = dio_atoi( test_a, 10 );

   ck_assert_int_eq( test_i, 150 );
}
END_TEST

START_TEST( check_dio_atoi_thousand_fifty ) {
   char test_a[] = "1050";
   int16_t test_i = 0;

   test_i = dio_atoi( test_a, 10 );

   ck_assert_int_eq( test_i, 1050 );
}
END_TEST

START_TEST( check_dio_atoi_junk ) {
   char test_a[] = "10dr3";
   int16_t test_i = 0;

   test_i = dio_atoi( test_a, 10 );

   ck_assert_int_eq( test_i, 10 );
}
END_TEST

START_TEST( check_dio_stream_bytes_openclose ) {
   struct DIO_STREAM test_stream;
   uint8_t test_buffer[1024];

/* memset( &g_buffer_w, '\0', DRC_CKBUFFER_SZ ); */
   memset( &test_stream, '\0', sizeof( struct DIO_STREAM ) );

   dio_open_stream_buffer( test_buffer, 1024, &test_stream );
   ck_assert_int_eq( 1, dio_type_stream( &test_stream ) );
   ck_assert_int_eq( 1024, dio_sz_stream( &test_stream ) );
}
END_TEST

START_TEST( check_dio_stream_bytes_rw ) {
   struct DIO_STREAM test_stream;
   const char* test_string = "Test String";
   uint8_t test_buffer[1024],
      test_reader[24];

   memset( test_buffer, '\0', 1024 );
   memset( &test_stream, '\0', sizeof( struct DIO_STREAM ) );
   memset( test_reader, '\0', 24 );

   dio_open_stream_buffer( test_buffer, 1024, &test_stream );
   dio_write_stream( test_string, 11 /* strlen( test_str ) */, &test_stream );

   ck_assert_int_eq( 11, strlen( test_string ) );
   ck_assert_int_eq( 11, strlen( test_buffer ) );
   ck_assert_int_eq( 11, dio_position_stream( &test_stream ) );
   ck_assert_int_eq( 1024, dio_sz_stream( &test_stream ) );

   dio_seek_stream( &test_stream, 5, SEEK_SET );
   dio_read_stream( test_reader, 6, &test_stream );

   ck_assert_str_eq( test_reader, "String" );
}
END_TEST

START_TEST( check_dio_stream_bytes_seek ) {
   struct DIO_STREAM test_stream;
   uint8_t test_buffer[1024];
   char test_string[] = "This is a test string.",
      test_string_2[] = "XYZ";
   
   dio_open_stream_buffer( test_buffer, 1024, &test_stream );
   dio_write_stream( &test_string, 22, &test_stream );

   dio_seek_stream( &test_stream, 0, SEEK_SET );

   dio_write_stream( test_string_2, 3, &test_stream );

   ck_assert_int_eq( 'X', test_buffer[0] );
   ck_assert_int_eq( 'Y', test_buffer[1] );
   ck_assert_int_eq( 'Z', test_buffer[2] );

   dio_seek_stream( &test_stream, 10, SEEK_SET );
   
   ck_assert_int_eq( 1024, dio_sz_stream( &test_stream ) );
   ck_assert_int_eq( 10, dio_position_stream( &test_stream ) );

   dio_seek_stream( &test_stream, 5, SEEK_CUR );

   dio_write_stream( test_string_2, 3, &test_stream );

   ck_assert_int_eq( 18, dio_position_stream( &test_stream ) );

   ck_assert_int_eq( 'X', test_buffer[15] );
   ck_assert_int_eq( 'Y', test_buffer[16] );
   ck_assert_int_eq( 'Z', test_buffer[17] );

   dio_seek_stream( &test_stream, 0, SEEK_END );

   ck_assert_int_eq( 1023, dio_position_stream( &test_stream ) );

   dio_close_stream( &test_stream );
}
END_TEST

#define TEST_STREAM_FILE_PATH "/tmp/check_dio.tmp"

struct DIO_STREAM g_test_stream_file;

static void dio_stream_file_setup() {
   dio_remove_file( TEST_STREAM_FILE_PATH );
}

static void dio_stream_file_teardown() {
   dio_close_stream( &g_test_stream_file );
   dio_remove_file( TEST_STREAM_FILE_PATH );
}

START_TEST( check_dio_stream_file_openclose ) {
   dio_open_stream_file( TEST_STREAM_FILE_PATH, "wb", &g_test_stream_file );
   ck_assert_int_eq( 2, dio_type_stream( &g_test_stream_file ) );
   ck_assert_int_eq( 0, dio_sz_stream( &g_test_stream_file ) );
}
END_TEST

START_TEST( check_dio_stream_file_rw ) {
   uint8_t test_buffer_in[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };

   dio_open_stream_file( TEST_STREAM_FILE_PATH, "wb", &g_test_stream_file );
   
   dio_write_stream( &test_buffer_in, 4, &g_test_stream_file );
   
   ck_assert_int_eq( 2, dio_type_stream( &g_test_stream_file ) );
   ck_assert_int_eq( 4, dio_sz_stream( &g_test_stream_file ) );
   ck_assert_int_eq( 4, dio_position_stream( &g_test_stream_file ) );

   dio_write_stream( &test_buffer_in, 2, &g_test_stream_file );
   
   ck_assert_int_eq( 6, dio_sz_stream( &g_test_stream_file ) );
   ck_assert_int_eq( 6, dio_position_stream( &g_test_stream_file ) );
}
END_TEST

START_TEST( check_dio_stream_file_seek ) {
   char test_string[] = "This is a test string.";
   
   dio_open_stream_file( TEST_STREAM_FILE_PATH, "wb", &g_test_stream_file );
   dio_write_stream( &test_string, 22, &g_test_stream_file );
   dio_close_stream( &g_test_stream_file );

   dio_open_stream_file( TEST_STREAM_FILE_PATH, "rb", &g_test_stream_file );

   dio_seek_stream( &g_test_stream_file, 10, SEEK_SET );
   
   ck_assert_int_eq( 22, dio_sz_stream( &g_test_stream_file ) );
   ck_assert_int_eq( 10, dio_position_stream( &g_test_stream_file ) );

   dio_seek_stream( &g_test_stream_file, 5, SEEK_CUR );

   ck_assert_int_eq( 15, dio_position_stream( &g_test_stream_file ) );

   dio_seek_stream( &g_test_stream_file, 0, SEEK_END );

   ck_assert_int_eq( 21, dio_position_stream( &g_test_stream_file ) );
}
END_TEST

Suite* dio_suite( void ) {
   Suite* s;
   TCase* tc_atoi,
      * tc_stream_bytes,
      * tc_stream_file;

   s = suite_create( "dio" );

   /* Core test case */
   tc_atoi = tcase_create( "atoi" );
   tc_stream_bytes = tcase_create( "stream_bytes" );
   tc_stream_file = tcase_create( "stream_file" );

   tcase_add_test( tc_atoi, check_dio_atoi_one ); 
   tcase_add_test( tc_atoi, check_dio_atoi_ten ); 
   tcase_add_test( tc_atoi, check_dio_atoi_hundred ); 
   tcase_add_test( tc_atoi, check_dio_atoi_hundred_fifty ); 
   tcase_add_test( tc_atoi, check_dio_atoi_thousand_fifty ); 
   tcase_add_test( tc_atoi, check_dio_atoi_junk ); 

   suite_add_tcase( s, tc_atoi );

   tcase_add_test( tc_stream_bytes, check_dio_stream_bytes_openclose ); 
   tcase_add_test( tc_stream_bytes, check_dio_stream_bytes_rw ); 
   tcase_add_test( tc_stream_bytes, check_dio_stream_bytes_seek ); 

   suite_add_tcase( s, tc_stream_bytes );

   tcase_add_checked_fixture(
      tc_stream_file, dio_stream_file_setup, dio_stream_file_teardown );

   tcase_add_test( tc_stream_file, check_dio_stream_file_openclose ); 
   tcase_add_test( tc_stream_file, check_dio_stream_file_rw ); 
   tcase_add_test( tc_stream_file, check_dio_stream_file_seek ); 

   suite_add_tcase( s, tc_stream_file );

   return s;
}

