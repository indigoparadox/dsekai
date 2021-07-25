
#include "sdli.h"

int input_init() {
   return 1;
}

int input_poll() {
   SDL_Event event;

   memset( &event, 0, sizeof( SDL_Event ) );

   SDL_PollEvent( &event );

   if( SDL_KEYDOWN == event.type ) {
      return event.key.keysym.sym;
   } else if( SDL_QUIT == event.type ) {
      return INPUT_KEY_QUIT;
   }
   return 0;
}

