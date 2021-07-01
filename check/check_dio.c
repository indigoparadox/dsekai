
#include <check.h>

#include "../src/data/dio.h"

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

Suite* dio_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "dio" );

   /* Core test case */
   tc_core = tcase_create( "Core" );

   tcase_add_test( tc_core, check_dio_atoi_one ); 
   tcase_add_test( tc_core, check_dio_atoi_ten ); 
   tcase_add_test( tc_core, check_dio_atoi_hundred ); 
   tcase_add_test( tc_core, check_dio_atoi_hundred_fifty ); 
   tcase_add_test( tc_core, check_dio_atoi_thousand_fifty ); 
   tcase_add_test( tc_core, check_dio_atoi_junk ); 

   suite_add_tcase( s, tc_core );

   return s;
}

