
#include "sdli.h"

int input_poll() {
   SDL_Event event;

   SDL_PollEvent( &event );

   if( SDL_KEYDOWN == event.type ) {
      return event.key.keysym.sym;
   }
}

