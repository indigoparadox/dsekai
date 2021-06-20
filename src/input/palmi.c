
#include "../input.h"
#include "palmi.h"

#include <PalmOS.h>

int input_poll() {
   EventType event;
   UInt32 key_state;

   /* Doze until an event arrives. */
   EvtGetEvent( &event, 100 );

   if( event.eType == keyDownEvent ) {
      key_state = KeyCurrentState();
      if( key_state & keyBitHard2 ) {
         return INPUT_KEY_LEFT;
      } else if( key_state & keyBitPageDown ) {
         return INPUT_KEY_DOWN;
      } else if( key_state & keyBitPageUp ) {
         return INPUT_KEY_UP;
      } else if( key_state & keyBitHard3 ) {
         return INPUT_KEY_RIGHT;
      }
   }

   /* System gets a chance to handle the event. */
   SysHandleEvent( &event );

   if( event.eType == appStopEvent ) {
      return INPUT_KEY_QUIT;
   }

   return 0;
}

