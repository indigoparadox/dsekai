
#include "dsekai.h"

int pov_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args ) {
   struct DSEKAI_STATE* state = NULL;
   int retval = 1;
   uint8_t in_char = 0;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   if( state->input_blocked_countdown ) {
      state->input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   switch( in_char ) {
   case INPUT_KEY_QUIT:
      window_pop( WINDOW_ID_STATUS, state );
      retval = 0;
      graphics_loop_end();
      goto cleanup;
   }

   graphics_loop_end();

cleanup:

   if( NULL != state ) {
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
   }

   graphics_flip( args );
   return retval;
}

