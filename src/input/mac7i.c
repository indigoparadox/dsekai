
#include "../input.h"

#include <Quickdraw.h>
#include <Windows.h>
#include <Events.h>

EventRecord g_event;

int input_poll() {
   SystemTask();

   if( WaitNextEvent( everyEvent, &g_event, 5L, NULL ) ) {
      if( mouseDown == g_event.what ) {
         return INPUT_KEY_QUIT;
      }
   }

   return 0;
}

