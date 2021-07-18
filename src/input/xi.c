
#include "../dstypes.h"

extern Display* g_display;
extern Window g_window;

int input_init() {
   XSelectInput( g_display, g_window, ExposureMask | KeyPressMask );
   return 1;
}

int input_poll() {
   XEvent event;
   int mask = 0;

   mask = ExposureMask | KeyPressMask | KeyReleaseMask;

   /* XNextEvent( g_display, &event ); */
   if( (
      XCheckWindowEvent( g_display, g_window, mask, &event ) ||
      XCheckTypedWindowEvent( g_display, g_window, KeyPress, &event )
   ) && KeyPress == event.type ) {
      /* printf( "%lu\n", event.xkey.keycode ); */
      return event.xkey.keycode;
   }
   return 0;
}

