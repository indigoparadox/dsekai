
#include "../input.h"

#include <Quickdraw.h>
#include <Windows.h>
#include <Events.h>

EventRecord g_event;

extern Rect g_window_rect;

int input_poll() {
   SystemTask();

   TextSize( 48 );
   MoveTo( g_window_rect.left, g_window_rect.top );
   DrawString( "\pXXX" );

   if( WaitNextEvent( everyEvent, &g_event, 5L, NULL ) ) {
      if( mouseDown == g_event.what ) {
         return INPUT_KEY_QUIT;
      }
   }

   return 0;
}

