
#include <check.h>

Suite* graphics_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "graphics" );

   /* Core test case */
   tc_core = tcase_create( "Core" );

   suite_add_tcase( s, tc_core );

   return s;
}

