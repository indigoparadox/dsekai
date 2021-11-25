
#include "dsekai.h"

int pov_loop( MEMORY_HANDLE state_handle ) {
   struct DSEKAI_STATE* state = NULL;
   int retval = 1;
   uint8_t in_char = 0;
   struct POV_STATE* gstate = NULL;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   if( ENGINE_STATE_OPENING == state->engine_state ) {

      assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
      state->engine_state_handle =
         memory_alloc( sizeof( struct POV_STATE ), 1 );

      gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

      gstate = (struct POV_STATE*)memory_unlock( state->engine_state_handle );

      state->engine_state = ENGINE_STATE_RUNNING;
   }

   graphics_loop_start();

   /* pov_draw( state, &(state->mobiles[state->player_idx]) ); */

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

   if( NULL != gstate ) {
      assert( NULL != state );
      gstate = (struct POV_STATE*)memory_unlock( state->engine_state_handle );
   }

   if( NULL != state ) {
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
   }

   graphics_flip();
   return retval;
}

