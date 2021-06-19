
#include "sdli.h"

int input_poll() {
   SDL_Event event;

   memset( &event, 0, sizeof( SDL_Event ) );

   SDL_PollEvent( &event );

   if( SDL_KEYDOWN == event.type ) {
      return event.key.keysym.sym;
   }
   return 0;
}

