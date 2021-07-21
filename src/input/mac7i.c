
#include "../input.h"

#include <Multiverse.h>

int input_init() {
   FlushEvents( everyEvent, 0 );
   return 1;
}

int input_poll() {
   EventRecord event;
   uint8_t key = 0;

   /* SystemTask(); */

   if( GetNextEvent( everyEvent, &event ) ) {
      SystemTask();
      if( keyDown == event.what ) {
         key = (uint8_t)((event.message & keyCodeMask) >> 8);
         switch( key ) {
         case 0x0c: /* Q */
            return INPUT_KEY_QUIT;

         case 0x06: /* Z */
            return INPUT_KEY_OK;

         default:
            break;
         }
      }
   }

   return 0;
}

