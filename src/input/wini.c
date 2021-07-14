
#include "../input.h"

/* This is set by the window message processor in win16g.c. */
uint32_t g_last_key = 0;

int input_init() {
   return 1;
}

int input_poll() {
   uint32_t last_key = g_last_key;
   g_last_key = 0;
   switch( last_key ) {
   case 0x57: /* W */
      return INPUT_KEY_UP;
   case 0x53: /* S */
      return INPUT_KEY_DOWN;
   case 0x44: /* D */
      return INPUT_KEY_RIGHT;
   case 0x41: /* A */
      return INPUT_KEY_LEFT;
   case 0x5a:
      return INPUT_KEY_OK;
   case 0x51:
      return INPUT_KEY_QUIT;
   }
   return 0;
}

