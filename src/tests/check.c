
#include <check.h>

#define main_add_test_proto( suite_name ) \
   Suite* suite_name ## _suite();

#define main_add_test( suite_name ) \
   Suite* s_ ## suite_name = suite_name ## _suite(); \
   SRunner* sr_ ## suite_name = srunner_create( s_ ## suite_name ); \
   /* srunner_set_fork_status( sr_ ## suite_name, CK_NOFORK ); */ \
   srunner_run_all( sr_ ## suite_name, CK_VERBOSE ); \
   number_failed += srunner_ntests_failed( sr_ ## suite_name ); \
   srunner_free( sr_ ## suite_name );

main_add_test_proto( mobile )
main_add_test_proto( tilemap )
main_add_test_proto( graphics )
main_add_test_proto( psprintf )
main_add_test_proto( item )
main_add_test_proto( engines )
main_add_test_proto( window )

int main( void ) {
   int number_failed = 0;

   main_add_test( mobile );
   main_add_test( tilemap );
   main_add_test( graphics );
   main_add_test( psprintf );
   main_add_test( item );
   main_add_test( engines );
   main_add_test( window );

   return( number_failed == 0 ) ? 0 : 1;
}

