
#include "dsekai.h"

#ifdef RESOURCE_FILE
/* TODO: Detect graphics format, not platform. */
#ifdef PLATFORM_DOS
#  define s_world ASSETS_PATH DEPTH_SPEC "/s_world.cga"
#else
#  define s_world ASSETS_PATH DEPTH_SPEC "/s_world.bmp"
#endif /* PLATFORM_DOS */
#endif /* RESOURCE_FILE */

int16_t title_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct TITLE_STATE* gstate = NULL;

   assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle =
      memory_alloc( sizeof( struct TITLE_STATE ), 1 );

   gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );

   /* Create the environmental animations. */
   animate_create(
      ANIMATE_TYPE_FIRE, ANIMATE_FLAG_CLEANUP,
      0, SCREEN_H - ANIMATE_TILE_H, SCREEN_W, ANIMATE_TILE_H );

   animate_create(
      ANIMATE_TYPE_SNOW, ANIMATE_FLAG_CLEANUP,
      0, 0, SCREEN_W, SCREEN_H - ANIMATE_TILE_H );

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

   graphics_draw_block( 0, 0, SCREEN_W, SCREEN_H, GRAPHICS_COLOR_BLACK );

   state->engine_state = ENGINE_STATE_RUNNING;

   return retval;
}

void title_draw( struct DSEKAI_STATE* state ) {
   int8_t i = 0;

   /* graphics_draw_block( 0, 0, SCREEN_W, SCREEN_H, GRAPHICS_COLOR_BLACK ); */

#ifdef DEPTH_VGA
   graphics_string_at( "dsekai", 6, 55, 30, GRAPHICS_COLOR_DARKRED, 1 );
#else
   graphics_string_at( "dsekai", 6, 55, 30, GRAPHICS_COLOR_MAGENTA, 1 );
#endif

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
}

void title_animate( struct DSEKAI_STATE* state ) {

}

int16_t title_input( char in_char, struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

   switch( in_char ) {
   case INPUT_KEY_OK:
      memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ),
         memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) );
      break;

   case INPUT_KEY_QUIT:
      window_pop( WINDOW_ID_STATUS, state );
      retval = 0;
      break;
   }

   return retval;
}

