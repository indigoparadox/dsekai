
#include "dsekai.h"

int16_t pov_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct POV_STATE* gstate = NULL;

   assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle =
      memory_alloc( sizeof( struct POV_STATE ), 1 );

   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

   gstate = (struct POV_STATE*)memory_unlock( state->engine_state_handle );

   state->engine_state = ENGINE_STATE_RUNNING;

   return retval;
}

void pov_shutdown( struct DSEKAI_STATE* state ) {

}

int16_t pov_input( char in_char, struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

   switch( in_char ) {
   case INPUT_KEY_QUIT:
      window_pop( WINDOW_ID_STATUS, state );
      retval = 0;
   }

   return retval;
}

void pov_animate( struct DSEKAI_STATE* state ) {

}

void pov_draw( struct DSEKAI_STATE* state ) {

}

