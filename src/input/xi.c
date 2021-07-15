
#include "../dstypes.h"

extern Display* g_display;
extern Window g_window;

int input_init() {
   XSelectInput( g_display, g_window, ExposureMask | KeyPressMask );
   return 1;
}

int input_poll() {
   XEvent event;

   XNextEvent( g_display, &event );

   if( KeyPress == event.type ) {
      return INPUT_KEY_QUIT;
   }
   return 0;
}

