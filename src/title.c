
#include "dsekai.h"

#define DSEKAI_TITLE_TEXT "dsekai"

#define DSEKAI_TITLE_TEXT_SZ 6

#ifndef NO_TITLE

static void title_draw_menu(
   struct DSEKAI_STATE* state, int16_t opt_start, RES_CONST char* tokens[],
   title_option_cb* callbacks
) {
   int16_t i = 0;
   struct TITLE_STATE* gstate = NULL;
   GRAPHICS_COLOR color;
   uint8_t flags = 0;

   gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );
   assert( NULL != gstate );

   /* If no new tokens provided, use the old ones. */
   if( NULL != tokens ) {
      gstate->option_min = opt_start;
      gstate->option_idx = opt_start;
      gstate->option_tokens = tokens;
      gstate->option_callbacks = callbacks;
   }
   gstate->option_max = gstate->option_min;
   i = gstate->option_min;

   window_push(
      WINDOW_ID_TITLE_MENU, 0, WINDOW_TYPE_WINDOW, 0,
      WINDOW_PLACEMENT_CENTER, TILE_H * 5, /* x, y */
      WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO, /* w, h */
      WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(), 0,
      0, NULL );

   assert( NULL != gstate->option_tokens );

   /* Iterate through the string list until the NULL string. */
   while( NULL != gstate->option_tokens[i] ) {
      if( gstate->option_idx == i ) {
         debug_printf( 1, "title option selected for draw: %d", i );
         color = WINDOW_PREFAB_DEFAULT_HL();
         flags = GRAPHICS_STRING_FLAG_ALL_CAPS | GRAPHICS_STRING_FLAG_OUTLINE;
      } else {
         debug_printf( 1, "title option not selected: %d", i );
         color = WINDOW_PREFAB_DEFAULT_FG();
         flags = GRAPHICS_STRING_FLAG_ALL_CAPS;
      }

      /* Add the string to the window. */
      window_push(
         WINDOW_ID_TITLE_MENU + 1 + i, WINDOW_ID_TITLE_MENU, WINDOW_TYPE_LABEL,
         WINDOW_FLAG_TEXT_PTR,
         WINDOW_PLACEMENT_GRID, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO,
         color, WINDOW_PREFAB_DEFAULT_BG(), flags,
         0, gstate->option_tokens[i] );

      (gstate->option_max)++;
      i++;
   }

   gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );
}

#define TITLE_ENGINES_LIST_CALLBACKS( idx, eng, prefix ) \
   void title_callback_eng_ ## prefix ( struct DSEKAI_STATE* state ) { \
      memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ), \
         memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) ); \
         state->engine_type_change = idx; \
   }

ENGINE_TABLE( TITLE_ENGINES_LIST_CALLBACKS )
      
#define TITLE_ENGINES_LIST_CALLBACKS_LIST( idx, eng, prefix ) \
   title_callback_eng_ ## prefix,

static title_option_cb gc_title_menu_new_callbacks[] = {
   ENGINE_TABLE( TITLE_ENGINES_LIST_CALLBACKS_LIST )
   NULL
};

/* Menu callback: List of available engines. */
void title_menu_new( struct DSEKAI_STATE* state ) {
   title_draw_menu( state, 1, gc_engines_tokens,
      gc_title_menu_new_callbacks );
}

/* First menu callbacks defined here as they call static functions. */
#define TITLE_MENU_FIRST( f ) f( "new", title_menu_new )

#define TITLE_MENU_FIRST_STR( str, cb ) str,
static RES_CONST char* gc_title_menu_first_tokens[] = {
   TITLE_MENU_FIRST( TITLE_MENU_FIRST_STR )
   NULL
};

#define TITLE_MENU_FIRST_CBS( str, cb ) cb,
static title_option_cb gc_title_menu_first_callbacks[] = {
   TITLE_MENU_FIRST( TITLE_MENU_FIRST_CBS )
   NULL
};

/* Menu callback: First menu that opens (new/load/options/etc). */
static void title_menu_first( struct DSEKAI_STATE* state ) {
   title_draw_menu(
      state, 0, gc_title_menu_first_tokens, gc_title_menu_first_callbacks );
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

   title_menu_first( state );

   /* Create the spinning globe animation. */
   /* (It's actually just four mobiles.) */
   state->mobiles[0].coords.x = 0;
   state->mobiles[0].coords.y = 0;
   state->mobiles[0].coords_prev.x = 3;
   state->mobiles[0].coords_prev.y = 2;
   state->mobiles[0].script_id = -1;
   state->mobiles[0].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[0]), 2 );
   state->mobiles[0].mp_hp = 100;
   state->mobiles[0].sprite_id = graphics_cache_load_bitmap(
      TITLE_STATIC_SPRITE_WORLD, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   state->mobiles[0].ascii = '/';

   state->mobiles[1].coords.x = 1;
   state->mobiles[1].coords.y = 0;
   state->mobiles[1].coords_prev.x = 3;
   state->mobiles[1].coords_prev.y = 2;
   state->mobiles[1].script_id = -1;
   state->mobiles[1].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[1]), 0 );
   state->mobiles[1].mp_hp = 100;
   state->mobiles[1].sprite_id = graphics_cache_load_bitmap(
      TITLE_STATIC_SPRITE_WORLD, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   state->mobiles[1].ascii = '\\';

   state->mobiles[2].coords.x = 0;
   state->mobiles[2].coords.y = 1;
   state->mobiles[2].coords_prev.x = 4;
   state->mobiles[2].coords_prev.y = 3;
   state->mobiles[2].script_id = -1;
   state->mobiles[2].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[2]), 3 );
   state->mobiles[2].mp_hp = 100;
   state->mobiles[2].sprite_id = graphics_cache_load_bitmap(
      TITLE_STATIC_SPRITE_WORLD, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
   state->mobiles[2].ascii = '\\';

   state->mobiles[3].coords.x = 1;
   state->mobiles[3].coords.y = 1;
   state->mobiles[3].coords_prev.x = 5;
   state->mobiles[3].coords_prev.y = 3;
   state->mobiles[3].script_id = -1;
   state->mobiles[3].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[3]), 1 );
   state->mobiles[3].mp_hp = 100;
   state->mobiles[3].sprite_id = graphics_cache_load_bitmap(
      TITLE_STATIC_SPRITE_WORLD, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
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
   struct GRAPHICS_RECT title_str_sz;

#ifndef NO_TITLE

   int16_t i = 0;

   graphics_string_sz(
      DSEKAI_TITLE_TEXT, DSEKAI_TITLE_TEXT_SZ, 0, &title_str_sz );
   graphics_string_at(
      DSEKAI_TITLE_TEXT,
      DSEKAI_TITLE_TEXT_SZ,
      (SCREEN_W / 2) - (title_str_sz.w / 2), /* Center horizontally. */
      TILE_H,
      DSEKAI_TITLE_TEXT_COLOR, /* Varies by depth, set in engines.h. */
      1 );

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
      graphics_cache_blit_at(
         mobile_get_sprite( &(state->mobiles[i]) ), i,
         state->ani_sprite_x,
         mobile_get_dir( &(state->mobiles[i]) ) * SPRITE_H,
         ((SCREEN_W / 2) - SPRITE_W) + (state->mobiles[i].coords.x * SPRITE_W),
         (2 * SPRITE_H) + (state->mobiles[i].coords.y * SPRITE_H),
         SPRITE_W, SPRITE_H );
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
   title_option_cb cb = NULL;

   gstate = (struct TITLE_STATE*)memory_lock( state->engine_state_handle );

   if( g_input_key_up == in_char ) {
      if( gstate->option_min < gstate->option_idx ) {
         gstate->option_idx--;
         window_pop( WINDOW_ID_TITLE_MENU );
         redraw_menu = 1;
      }

   } else if( g_input_key_down == in_char ) {
      if( gstate->option_max - 1 > gstate->option_idx ) {
         gstate->option_idx++;
         window_pop( WINDOW_ID_TITLE_MENU );
         redraw_menu = 1;
      }

   } else if( g_input_key_ok == in_char ) {

      cb = gstate->option_callbacks[gstate->option_idx];

      /* Pop the window and unlock gstate for the callback! */
      window_pop( WINDOW_ID_TITLE_MENU );
      gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );

      cb( state );

      /* Don't set redraw, as the callback might redraw on its own. */

#ifndef NO_QUIT
   } else if( g_input_key_quit == in_char ) {
      retval = 0;
#endif /*! NO_QUIT */
   }
 
   if( NULL != gstate ) {
      gstate = (struct TITLE_STATE*)memory_unlock( state->engine_state_handle );
   }

   /* Redraw menu here after gstate freed. */
   if( redraw_menu ) {
      title_draw_menu( state, -1, NULL, NULL );
   }

#endif /* !NO_TITLE */

   return retval;
}

