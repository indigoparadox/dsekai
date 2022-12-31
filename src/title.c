
#include "dsekai.h"

#ifdef RESOURCE_FILE
/* TODO: Detect graphics format, not platform. */
#ifdef PLATFORM_DOS
#  if defined( DEPTH_CGA )
#     define s_world ASSETS_PATH DEPTH_SPEC "/s_world.cga"
#  elif defined( DEPTH_VGA )
#     define s_world ASSETS_PATH DEPTH_SPEC "/s_world.vga"
#  else
#     error "not implemented"
#  endif /* DEPTH_CGA || DEPTH_VGA */
#else
#  define s_world ASSETS_PATH DEPTH_SPEC "/s_world.bmp"
#endif /* PLATFORM_DOS */
#endif /* RESOURCE_FILE */

#ifndef NO_ENGINE_POV
#  ifndef NO_QUIT
#     define TITLE_OPTIONS_COUNT 3
#  else
#     define TITLE_OPTIONS_COUNT 2
#  endif /* !NO_QUIT */
#else
#  ifndef NO_QUIT
#     define TITLE_OPTIONS_COUNT 2
#  else
#     define TITLE_OPTIONS_COUNT 1
#  endif /* !NO_QUIT */
#endif /* !NO_ENGINE_POV */

RES_CONST char* gc_title_options[] = {
   "topdown"
#ifndef NO_ENGINE_POV
   , "pov"
#endif /* !NO_ENGINE_POV */
#ifndef NO_QUIT
   , "quit"
#endif /* NO_QUIT */
};

#ifndef NO_TITLE

static void title_draw_menu( struct DSEKAI_STATE* state ) {
   int16_t i = 0;
   struct TITLE_STATE* gstate = NULL;
   GRAPHICS_COLOR color;
   uint8_t flags = 0;

   gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );

   window_push(
      WINDOW_ID_TITLE_MENU, 0, WINDOW_TYPE_WINDOW, 0,
      WINDOW_PLACEMENT_CENTER, SCREEN_H - (TILE_H * 5), /* x, y */
      WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO, /* w, h */
      WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(), 0,
      0, NULL );

   for( i = 0 ; TITLE_OPTIONS_COUNT > i ; i++ ) {
      if( gstate->option_high == i ) {
         debug_printf( 1, "title option selected for draw: %d", i );
         color = WINDOW_PREFAB_DEFAULT_HL();
         flags = GRAPHICS_STRING_FLAG_ALL_CAPS | GRAPHICS_STRING_FLAG_OUTLINE;
      } else {
         color = WINDOW_PREFAB_DEFAULT_FG();
         flags = GRAPHICS_STRING_FLAG_ALL_CAPS;
      }

      window_push(
         WINDOW_ID_TITLE_MENU + 1 + i, WINDOW_ID_TITLE_MENU, WINDOW_TYPE_LABEL,
         WINDOW_FLAG_TEXT_PTR,
         WINDOW_PLACEMENT_GRID, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO,
         color, WINDOW_PREFAB_DEFAULT_BG(), flags,
         0, gc_title_options[i] );
   }

   gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );
}

#endif /* !NO_TITLE */

int16_t title_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

#ifndef NO_TITLE

   debug_printf( 2, "allocating engine-specific state" );
   assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle =
      memory_alloc( sizeof( struct TITLE_STATE ), 1 );

   /* Create the environmental animations. */
   animate_create(
      ANIMATE_TYPE_FIRE, ANIMATE_FLAG_CLEANUP | ANIMATE_FLAG_BG,
      0, SCREEN_H - ANIMATE_TILE_H, SCREEN_W, ANIMATE_TILE_H );

   animate_create(
      ANIMATE_TYPE_SNOW, ANIMATE_FLAG_CLEANUP | ANIMATE_FLAG_BG,
      0, 0, SCREEN_W, SCREEN_H - ANIMATE_TILE_H );

   title_draw_menu( state );

   /* Create the spinning globe animation. */
   /* (It's actually just four mobiles.) */
   state->mobiles[0].coords.x = 4;
   state->mobiles[0].coords.y = 2;
   state->mobiles[0].coords_prev.x = 3;
   state->mobiles[0].coords_prev.y = 2;
   state->mobiles[0].script_id = -1;
   state->mobiles[0].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[0]), 2 );
   state->mobiles[0].mp_hp = 100;
   state->mobiles[0].sprite_id = graphics_cache_load_bitmap(
      s_world, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   state->mobiles[0].ascii = '/';

   state->mobiles[1].coords.x = 5;
   state->mobiles[1].coords.y = 2;
   state->mobiles[1].coords_prev.x = 3;
   state->mobiles[1].coords_prev.y = 2;
   state->mobiles[1].script_id = -1;
   state->mobiles[1].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[1]), 0 );
   state->mobiles[1].mp_hp = 100;
   state->mobiles[1].sprite_id = graphics_cache_load_bitmap(
      s_world, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   state->mobiles[1].ascii = '\\';

   state->mobiles[2].coords.x = 4;
   state->mobiles[2].coords.y = 3;
   state->mobiles[2].coords_prev.x = 4;
   state->mobiles[2].coords_prev.y = 3;
   state->mobiles[2].script_id = -1;
   state->mobiles[2].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[2]), 3 );
   state->mobiles[2].mp_hp = 100;
   state->mobiles[2].sprite_id = graphics_cache_load_bitmap(
      s_world, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   state->mobiles[2].ascii = '\\';

   state->mobiles[3].coords.x = 5;
   state->mobiles[3].coords.y = 3;
   state->mobiles[3].coords_prev.x = 5;
   state->mobiles[3].coords_prev.y = 3;
   state->mobiles[3].script_id = -1;
   state->mobiles[3].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[3]), 1 );
   state->mobiles[3].mp_hp = 100;
   state->mobiles[3].sprite_id = graphics_cache_load_bitmap(
      s_world, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   state->mobiles[3].ascii = '/';

   graphics_lock();
   graphics_clear_screen();
   graphics_release();

#else
   
   /* Just start the first tilemap. */
   memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ),
      memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) );
   state->engine_type_change = 1 /* ENGINE_TYPE_TOPDOWN */;

#endif /* !NO_TITLE */

   state->engine_state = ENGINE_STATE_RUNNING;

   return retval;
}

void title_shutdown( struct DSEKAI_STATE* state ) {
}

void title_draw( struct DSEKAI_STATE* state ) {

#ifndef NO_TITLE

   int16_t i = 0;

   /* graphics_draw_block( 0, 0, SCREEN_W, SCREEN_H, GRAPHICS_COLOR_BLACK ); */

#ifdef DEPTH_VGA
   graphics_string_at( "dsekai", 6, 55, TILE_H, GRAPHICS_COLOR_DARKRED, 1 );
#else
   graphics_string_at( "dsekai", 6, 55, TILE_H, GRAPHICS_COLOR_MAGENTA, 1 );
#endif

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if(
         MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & state->mobiles[i].flags)
      ) {
         continue;
      }
      if(
         MOBILE_FLAG_NOT_LAST !=
            (MOBILE_FLAG_NOT_LAST & state->mobiles[i].flags)
      ) {
         break;
      }

      assert( 0 <= state->mobiles[i].sprite_id );

      /* Draw current mobile sprite/frame. */
#ifdef PLATFORM_CURSES
      graphics_cache_blit_at(
         state->mobiles[i].ascii, i,
         state->ani_sprite_x,
         mobile_get_dir( &(state->mobiles[i]) ) * SPRITE_H,
         (state->mobiles[i].coords.x * SPRITE_W),
         (state->mobiles[i].coords.y * SPRITE_H),
         SPRITE_W, SPRITE_H );
#else
      graphics_cache_blit_at(
         state->mobiles[i].sprite_id, i,
         state->ani_sprite_x,
         mobile_get_dir( &(state->mobiles[i]) ) * SPRITE_H,
         (state->mobiles[i].coords.x * SPRITE_W),
         (state->mobiles[i].coords.y * SPRITE_H),
         SPRITE_W, SPRITE_H );
#endif /* PLATFORM_CURSES */
   }

   window_refresh( WINDOW_ID_TITLE_MENU );

#endif /* !NO_TITLE */

}

void title_animate( struct DSEKAI_STATE* state ) {

}

int16_t title_input(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state
) {
   int16_t retval = 1;

#ifndef NO_TITLE

   struct TITLE_STATE* gstate = NULL;
   uint8_t redraw_menu = 0;

   gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );

   if( g_input_key_up == in_char ) {
      if( 0 < gstate->option_high ) {
         gstate->option_high--;
         window_pop( WINDOW_ID_TITLE_MENU );
         redraw_menu = 1;
      }

   } else if( g_input_key_down == in_char ) {
      if( TITLE_OPTIONS_COUNT > gstate->option_high + 1 ) {
         gstate->option_high++;
         window_pop( WINDOW_ID_TITLE_MENU );
         redraw_menu = 1;
      }

   } else if( g_input_key_ok == in_char ) {

      window_pop( WINDOW_ID_TITLE_MENU );

#ifdef NO_ENGINE_POV
      if( 1 == gstate->option_high ) {
         retval = 0;
#else
#  ifndef NO_QUIT
      if( 2 == gstate->option_high ) {
         retval = 0;

      } else
#  endif /* !NO_QUIT */
      if( 1 == gstate->option_high ) {
         memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ),
            memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) );
         state->engine_type_change = 2 /* ENGINE_TYPE_POV */;

#endif /* NO_ENGINE_POV */
      } else if( 0 == gstate->option_high ) {
         memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ),
            memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) );
         state->engine_type_change = 1 /* ENGINE_TYPE_TOPDOWN */;
      }

#ifndef NO_QUIT
   } else if( g_input_key_quit == in_char ) {
      retval = 0;
#endif /*! NO_QUIT */
   }
 
   gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );

   /* Redraw menu here after gstate freed. */
   if( redraw_menu ) {
      title_draw_menu( state );
   }

#endif /* !NO_TITLE */

   return retval;
}

