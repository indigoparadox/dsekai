
#include "dsekai.h"

#ifdef RESOURCE_FILE
/* TODO: Detect graphics format, not platform. */
#ifdef PLATFORM_DOS
#  define s_world ASSETS_PATH DEPTH_SPEC "/s_world.cga"
#else
#  define s_world ASSETS_PATH DEPTH_SPEC "/s_world.bmp"
#endif /* PLATFORM_DOS */
#endif /* RESOURCE_FILE */

#define TITLE_OPTIONS_COUNT 3

const char* gc_title_options[] = {
   "topdown",
   "pov",
   "quit"
};

static void title_draw_menu( struct DSEKAI_STATE* state ) {
   int16_t i = 0;
   struct TITLE_STATE* gstate = NULL;
   GRAPHICS_COLOR color;

   gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );

   window_push(
      WINDOW_ID_TITLE_MENU, 0, WINDOW_TYPE_WINDOW, 0,
      WINDOW_PLACEMENT_CENTER,
      SCREEN_H - (TILE_H * 5), TILE_H * 6, TILE_W * 3,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK, 0,
      0, 0, NULL, state );

   for( i = 0 ; TITLE_OPTIONS_COUNT > i ; i++ ) {
      if( gstate->option_high == i ) {
         color = GRAPHICS_COLOR_CYAN;
      } else {
         color = GRAPHICS_COLOR_WHITE;
      }

      window_push(
         WINDOW_ID_TITLE_MENU + 1 + i, WINDOW_ID_TITLE_MENU, WINDOW_TYPE_LABEL,
         WINDOW_FLAG_TEXT_PTR,
         WINDOW_PLACEMENT_GRID, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO,
         color, GRAPHICS_COLOR_BLACK,
         GRAPHICS_STRING_FLAGS_ALL_CAPS,
         0, 0, gc_title_options[i], state );
   }

   gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );
}

int16_t title_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

   assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle =
      memory_alloc( sizeof( struct TITLE_STATE ), 1 );

   /* Create the environmental animations. */
   animate_create(
      ANIMATE_TYPE_FIRE, ANIMATE_FLAG_CLEANUP,
      0, SCREEN_H - ANIMATE_TILE_H, SCREEN_W, ANIMATE_TILE_H );

   animate_create(
      ANIMATE_TYPE_SNOW, ANIMATE_FLAG_CLEANUP,
      0, 0, SCREEN_W, SCREEN_H - ANIMATE_TILE_H );

   title_draw_menu( state );

   /* Create the spinning globe animation. */
   /* (It's actually just four mobiles.) */
   state->mobiles[0].coords.x = 4;
   state->mobiles[0].coords.y = 2;
   state->mobiles[0].coords_prev.x = 3;
   state->mobiles[0].coords_prev.y = 2;
   state->mobiles[0].script_id = -1;
   state->mobiles[0].flags = MOBILE_FLAG_ACTIVE;
   state->mobiles[0].dir = 2;
   state->mobiles[0].hp = 100;
   resource_assign_id( state->mobiles[0].sprite, s_world );

   state->mobiles[1].coords.x = 5;
   state->mobiles[1].coords.y = 2;
   state->mobiles[1].coords_prev.x = 3;
   state->mobiles[1].coords_prev.y = 2;
   state->mobiles[1].script_id = -1;
   state->mobiles[1].flags = MOBILE_FLAG_ACTIVE;
   state->mobiles[1].dir = 0;
   state->mobiles[1].hp = 100;
   resource_assign_id( state->mobiles[1].sprite, s_world );

   state->mobiles[2].coords.x = 4;
   state->mobiles[2].coords.y = 3;
   state->mobiles[2].coords_prev.x = 4;
   state->mobiles[2].coords_prev.y = 3;
   state->mobiles[2].script_id = -1;
   state->mobiles[2].flags = MOBILE_FLAG_ACTIVE;
   state->mobiles[2].dir = 3;
   state->mobiles[2].hp = 100;
   resource_assign_id( state->mobiles[2].sprite, s_world );

   state->mobiles[3].coords.x = 5;
   state->mobiles[3].coords.y = 3;
   state->mobiles[3].coords_prev.x = 5;
   state->mobiles[3].coords_prev.y = 3;
   state->mobiles[3].script_id = -1;
   state->mobiles[3].flags = MOBILE_FLAG_ACTIVE;
   state->mobiles[3].dir = 1;
   state->mobiles[3].hp = 100;
   resource_assign_id( state->mobiles[3].sprite, s_world );

   graphics_draw_block( 0, 0, SCREEN_W, SCREEN_H, GRAPHICS_COLOR_BLACK );

   /* Put snow behind windows and globe. */
   state->flags |= DSEKAI_FLAG_ANIMATIONS_BG;

   state->engine_state = ENGINE_STATE_RUNNING;

   return retval;
}

void title_shutdown( struct DSEKAI_STATE* state ) {
   state->flags &= ~DSEKAI_FLAG_ANIMATIONS_BG;
}

void title_draw( struct DSEKAI_STATE* state ) {
   int8_t i = 0;

   /* graphics_draw_block( 0, 0, SCREEN_W, SCREEN_H, GRAPHICS_COLOR_BLACK ); */

#ifdef DEPTH_VGA
   graphics_string_at( "dsekai", 6, 55, 20, GRAPHICS_COLOR_DARKRED, 1 );
#else
   graphics_string_at( "dsekai", 6, 55, 20, GRAPHICS_COLOR_MAGENTA, 1 );
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

   window_refresh( WINDOW_ID_TITLE_MENU, state );
}

void title_animate( struct DSEKAI_STATE* state ) {

}

int16_t title_input( char in_char, struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct TITLE_STATE* gstate = NULL;

   gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );

   switch( in_char ) {
   case INPUT_KEY_UP:
      if( 0 < gstate->option_high ) {
         gstate->option_high--;
         window_pop( WINDOW_ID_TITLE_MENU, state );
         title_draw_menu( state );
      }
      break;

   case INPUT_KEY_DOWN:
      if( TITLE_OPTIONS_COUNT > gstate->option_high + 1 ) {
         gstate->option_high++;
         window_pop( WINDOW_ID_TITLE_MENU, state );
         title_draw_menu( state );
      }
      break;

   case INPUT_KEY_OK:
      if( 2 == gstate->option_high ) {
         retval = 0;
      } else if( 0 == gstate->option_high ) {
         memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ),
            memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) );
         state->engine_type_change = 1 /* ENGINE_TYPE_TOPDOWN */;
      }
      break;

   case INPUT_KEY_QUIT:
      retval = 0;
      break;
   }
 
   gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );

   return retval;
}

