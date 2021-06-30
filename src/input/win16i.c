
#include "../input.h"

uint32_t is_pressed( uint32_t key ) {
   uint32_t other_case = 0;
   if( key >= 97 /* a */ && key <= 122 /* z */ ) {
      other_case = key - 32;
   } else if( key >= 65 /* A */ && key <= 90 ) {
      other_case = key + 32;
   }
   return (0x80000000 & GetAsyncKeyState( key )) ||
      (0x80000000 & GetAsyncKeyState( other_case ));
}

int input_init() {
   return 1;
}

int input_poll() {
   if( is_pressed( 'w' ) ) {
      return INPUT_KEY_UP;
   } else if( is_pressed( 'a' ) ) {
      return INPUT_KEY_RIGHT;
   } else if( is_pressed( 's' ) ) {
      return INPUT_KEY_DOWN;
   } else if( is_pressed( 'd' ) ) {
      return INPUT_KEY_LEFT;
   } else if( is_pressed( 'z' ) ) {
      return INPUT_KEY_OK;
   } else if( is_pressed( 'q' ) ) {
      return INPUT_KEY_QUIT;
   }
   return 0;
}

