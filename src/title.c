
#include "dsekai.h"

#ifdef RESOURCE_FILE
/* TODO: Detect graphics format, not platform. */
#ifdef PLATFORM_DOS
#  define s_world "assets/" DEPTH_SPEC "/s_world.cga"
#else
#  define s_world "assets/" DEPTH_SPEC "/s_world.bmp"
#endif /* PLATFORM_DOS */
#endif /* RESOURCE_FILE */

int title_loop( MEMORY_HANDLE state_handle ) {
   struct DSEKAI_STATE* state = NULL;
   int retval = 1,
      i = 0;
   uint8_t in_char = 0;
   struct TITLE_STATE* gstate = NULL;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   if( ENGINE_STATE_OPENING == state->engine_state ) {

      assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
      state->engine_state_handle =
         memory_alloc( sizeof( struct TITLE_STATE ), 1 );

      gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );

      /* Create the environmental animations. */
      animate_create(
         ANIMATE_TYPE_FIRE,
         0, SCREEN_H - ANIMATE_TILE_H, SCREEN_W, ANIMATE_TILE_H );

      animate_create(
         ANIMATE_TYPE_SNOW, 0, 0, SCREEN_W, SCREEN_H - ANIMATE_TILE_H );

      gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );

      /* Create the spinning globe animation. */
      /* (It's actually just four mobiles.) */
      state->mobiles[0].coords.x = 4;
      state->mobiles[0].coords.y = 3;
      state->mobiles[0].coords_prev.x = 3;
      state->mobiles[0].coords_prev.y = 3;
      state->mobiles[0].script_id = -1;
      state->mobiles[0].flags = MOBILE_FLAG_ACTIVE;
      state->mobiles[0].dir = 2;
      state->mobiles[0].hp = 100;
      resource_assign_id( state->mobiles[0].sprite, s_world );

      state->mobiles[1].coords.x = 5;
      state->mobiles[1].coords.y = 3;
      state->mobiles[1].coords_prev.x = 3;
      state->mobiles[1].coords_prev.y = 3;
      state->mobiles[1].script_id = -1;
      state->mobiles[1].flags = MOBILE_FLAG_ACTIVE;
      state->mobiles[1].dir = 0;
      state->mobiles[1].hp = 100;
      resource_assign_id( state->mobiles[1].sprite, s_world );

      state->mobiles[2].coords.x = 4;
      state->mobiles[2].coords.y = 4;
      state->mobiles[2].coords_prev.x = 3;
      state->mobiles[2].coords_prev.y = 3;
      state->mobiles[2].script_id = -1;
      state->mobiles[2].flags = MOBILE_FLAG_ACTIVE;
      state->mobiles[2].dir = 3;
      state->mobiles[2].hp = 100;
      resource_assign_id( state->mobiles[2].sprite, s_world );

      state->mobiles[3].coords.x = 5;
      state->mobiles[3].coords.y = 4;
      state->mobiles[3].coords_prev.x = 3;
      state->mobiles[3].coords_prev.y = 3;
      state->mobiles[3].script_id = -1;
      state->mobiles[3].flags = MOBILE_FLAG_ACTIVE;
      state->mobiles[3].dir = 1;
      state->mobiles[3].hp = 100;
      resource_assign_id( state->mobiles[3].sprite, s_world );

      state->engine_state = ENGINE_STATE_RUNNING;
   }

   graphics_loop_start();

#if 0
   graphics_blit_at(
#ifdef RESOURCE_FILE
      "assets/" DEPTH_SPEC "/title.bmp",
#else
      title,
#endif
      0, 0, 0, 0, SCREEN_W, SCREEN_H );
#endif

   mobile_state_animate( state );

   graphics_draw_block( 0, 0, SCREEN_W, SCREEN_H, GRAPHICS_COLOR_BLACK );

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if(
         MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & state->mobiles[i].flags)
      ) {
         continue;
      }

      /* Draw current mobile sprite/frame. */
      graphics_blit_sprite_at(
         state->mobiles[i].sprite,
         state->ani_sprite_x,
         state->mobiles[i].dir * SPRITE_H,
         (state->mobiles[i].coords.x * SPRITE_W),
         (state->mobiles[i].coords.y * SPRITE_H),
         SPRITE_W, SPRITE_H );
   }


#ifdef DEPTH_VGA
   graphics_string_at( "dsekai", 6, 55, 30, GRAPHICS_COLOR_DARKRED, 1 );
#else
   graphics_string_at( "dsekai", 6, 55, 30, GRAPHICS_COLOR_MAGENTA, 1 );
#endif

   animate_frame();

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
      if( '\0' != state->warp_to[0] ) {
         /* There's a warp-in map, so unload the current map and load it. */
         state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
         engines_warp_loop( state_handle );
      } else {
         state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
      }
   }

   graphics_flip();
   return retval;
}

