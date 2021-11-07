
#include "dsekai.h"

int title_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args ) {
   struct DSEKAI_STATE* state = NULL;
   int retval = 1;
   uint8_t in_char = 0;
   struct TITLE_STATE* gstate = NULL;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   if( ENGINE_STATE_OPENING == state->engine_state ) {

      assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
      state->engine_state_handle =
         memory_alloc( sizeof( struct TITLE_STATE ), 1 );

      gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );

      gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );

      state->engine_state = ENGINE_STATE_RUNNING;
   }

   graphics_loop_start();

   graphics_blit_at(
#ifdef RESOURCE_FILE
      "assets/" DEPTH_SPEC "/title.bmp",
#else
      title,
#endif
      0, 0, 0, 0, SCREEN_W, SCREEN_H );

   if( state->input_blocked_countdown ) {
      state->input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   switch( in_char ) {
   case INPUT_KEY_OK:
      memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ),
         memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) );
      graphics_loop_end();
      goto cleanup;

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
      gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );
   }

   if( NULL != state ) {
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
   }

   graphics_flip( args );
   return retval;
}

