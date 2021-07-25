
#include "../input.h"
#include "palmi.h"

#include <PalmOS.h>

static UInt32 g_next_input = 0;

int input_init() {
   return 1;
}

int input_poll() {
   EventType event;
   UInt32 key_state;
   UInt16 allow_input = 0;

   /* Doze until an event arrives. */
   EvtGetEvent( &event, 100 );

   if( 0 == g_next_input || TimGetTicks() > g_next_input ) {
      g_next_input = TimGetTicks() + (SysTicksPerSecond());
      allow_input = 0xffff;
   }

   switch( event.eType ) {
   case keyDownEvent:
      if( event.data.keyDown.chr == hard2Chr ) {
         return allow_input & INPUT_KEY_LEFT;

      } else if( event.data.keyDown.chr == pageDownChr ) {
         return allow_input & INPUT_KEY_DOWN;

      } else if( event.data.keyDown.chr == pageUpChr ) {
         return allow_input & INPUT_KEY_UP;

      } else if( event.data.keyDown.chr == hard3Chr ) {
         return allow_input & INPUT_KEY_RIGHT;

      } else if( event.data.keyDown.chr == hard4Chr ) {
         return allow_input & INPUT_KEY_OK;

      } else {
         /* System gets a chance to handle the event. */
         SysHandleEvent( &event );
      }
      break;

   case appStopEvent:
      return INPUT_KEY_QUIT;

   default:
      /* System gets a chance to handle the event. */
      SysHandleEvent( &event );
      break;
   }

   return 0;
}

