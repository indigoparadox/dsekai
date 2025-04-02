
#include "dsekai.h"

#define DSEKAI_TITLE_TEXT "dsekai"

#define DSEKAI_TITLE_TEXT_SZ 6

#ifndef NO_TITLE

void title_menu_first( struct DSEKAI_STATE* state );

static void title_draw_menu(
   struct DSEKAI_STATE* state, int16_t opt_start, MAUG_CONST char* tokens[],
   title_option_cb* callbacks
) {
   int16_t i = 0;
   struct TITLE_STATE* gstate = NULL;
   RETROFLAT_COLOR color;
   uint8_t flags = 0;

   maug_mlock( state->engine_state_handle, gstate );
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

/* TODO: Reimplement in maug. */
#if 0
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
#endif

   maug_munlock( state->engine_state_handle, gstate );
}

#define TITLE_ENGINES_LIST_CALLBACKS( idx, eng, prefix ) \
   void title_callback_eng_ ## prefix ( struct DSEKAI_STATE* state ) { \
      strcpy( state->warp_to, ENTRY_MAP ); \
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

void title_menu_load( struct DSEKAI_STATE* state ) {
   int32_t load_res = 0;

   /* TODO: Pick filename. */
   load_res = serial_load( "save.asn", state );

   if( 0 > load_res ) {
      /* TODO: Show error. */
      title_menu_first( state );
   }
   debug_printf( 3, "save loaded successfully! (%d bytes)", load_res );
}

/* First menu callbacks defined here as they call static functions. */
#define TITLE_MENU_FIRST( f ) f( "new", title_menu_new ) f( "load", title_menu_load )

#define TITLE_MENU_FIRST_STR( str, cb ) str,
static MAUG_CONST char* gc_title_menu_first_tokens[] = {
   TITLE_MENU_FIRST( TITLE_MENU_FIRST_STR )
   NULL
};

#define TITLE_MENU_FIRST_CBS( str, cb ) cb,
static title_option_cb gc_title_menu_first_callbacks[] = {
   TITLE_MENU_FIRST( TITLE_MENU_FIRST_CBS )
   NULL
};

/* Menu callback: First menu that opens (new/load/options/etc). */
void title_menu_first( struct DSEKAI_STATE* state ) {
   title_draw_menu(
      state, 0, gc_title_menu_first_tokens, gc_title_menu_first_callbacks );
}

#endif /* !NO_TITLE */

int16_t title_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

#ifndef NO_TITLE
   retroflat_asset_path mobile_sprite_id = "world.bmp"; /* TODO: Correct sprite. */

   debug_printf( 2, "allocating engine-specific state" );
   assert( (MAUG_MHANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle =
      maug_malloc( sizeof( struct TITLE_STATE ), 1 );

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Create the environmental animations. */
   retroani_create(
      &(state->animations[0]), DSEKAI_ANIMATIONS_MAX,
      RETROANI_TYPE_FIRE, RETROANI_FLAG_CLEANUP | ANIMATE_FLAG_BG,
      0, retroflat_screen_h() - RETROANI_TILE_H, retroflat_screen_w(), RETROANI_TILE_H );

   retroani_create(
      &(state->animations[0]), DSEKAI_ANIMATIONS_MAX,
      RETROANI_TYPE_SNOW, RETROANI_FLAG_CLEANUP | ANIMATE_FLAG_BG,
      0, 0, retroflat_screen_w(), retroflat_screen_h() - RETROANI_TILE_H );

   title_menu_first( state );

   assert( 4 <= state->mobiles_sz );

   /* Create the spinning globe animation. */
   /* (It's actually just four mobiles.) */
   state->mobiles[0].coords[1].x = 0;
   state->mobiles[0].coords[1].y = 0;
   state->mobiles[0].coords[0].x = 3;
   state->mobiles[0].coords[0].y = 2;
   state->mobiles[0].script_id = -1;
   state->mobiles[0].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[0]), 2 );
   state->mobiles[0].mp_hp = 100;
   state->mobiles[0].sprite_cache_id = retrogxc_load_bitmap(
      mobile_sprite_id, 0 );
   maug_cleanup_if_lt( state->mobiles[0].sprite_cache_id, 0, "%d", -1 );
   state->mobiles[0].ascii = '/';

   state->mobiles[1].coords[1].x = 1;
   state->mobiles[1].coords[1].y = 0;
   state->mobiles[1].coords[0].x = 3;
   state->mobiles[1].coords[0].y = 2;
   state->mobiles[1].script_id = -1;
   state->mobiles[1].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[1]), 0 );
   state->mobiles[1].mp_hp = 100;
   state->mobiles[1].sprite_cache_id = retrogxc_load_bitmap(
      mobile_sprite_id, 0 );
   maug_cleanup_if_lt( state->mobiles[1].sprite_cache_id, 0, "%d", -1 );
   state->mobiles[1].ascii = '\\';

   state->mobiles[2].coords[1].x = 0;
   state->mobiles[2].coords[1].y = 1;
   state->mobiles[2].coords[0].x = 4;
   state->mobiles[2].coords[0].y = 3;
   state->mobiles[2].script_id = -1;
   state->mobiles[2].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[2]), 3 );
   state->mobiles[2].mp_hp = 100;
   state->mobiles[2].sprite_cache_id = retrogxc_load_bitmap(
      mobile_sprite_id, 0 );
   maug_cleanup_if_lt( state->mobiles[2].sprite_cache_id, 0, "%d", -1 );
   state->mobiles[2].ascii = '\\';

   state->mobiles[3].coords[1].x = 1;
   state->mobiles[3].coords[1].y = 1;
   state->mobiles[3].coords[0].x = 5;
   state->mobiles[3].coords[0].y = 3;
   state->mobiles[3].script_id = -1;
   state->mobiles[3].flags = MOBILE_FLAG_ACTIVE | MOBILE_FLAG_NOT_LAST;
   mobile_set_dir( &(state->mobiles[3]), 1 );
   state->mobiles[3].mp_hp = 100;
   state->mobiles[3].sprite_cache_id = retrogxc_load_bitmap(
      mobile_sprite_id, 0 );
   maug_cleanup_if_lt( state->mobiles[3].sprite_cache_id, 0, "%d", -1 );
   state->mobiles[3].ascii = '/';

   /*
   graphics_lock();
   graphics_clear_screen();
   graphics_release();
   */

#else
   
   /* Just start the first tilemap. */
   memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ),
      memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) );
   state->engine_type_change = 1 /* ENGINE_TYPE_TOPDOWN */;

#endif /* !NO_TITLE */

   state->engine_state = ENGINE_STATE_RUNNING;

cleanup:

   return retval;
}

void title_shutdown( struct DSEKAI_STATE* state ) {
}

void title_draw( struct DSEKAI_STATE* state ) {
   struct GRAPHICS_RECT title_str_sz;

#ifndef NO_TITLE

   int16_t i = 0;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

#if 0
   /* TODO: Title text. */
   graphics_string_sz(
      DSEKAI_TITLE_TEXT, DSEKAI_TITLE_TEXT_SZ, 0, &title_str_sz );
   graphics_string_at(
      DSEKAI_TITLE_TEXT,
      DSEKAI_TITLE_TEXT_SZ,
      (retroflat_screen_w() / 2) - (title_str_sz.w / 2), /* Center horizontally. */
      TILE_H,
      DSEKAI_TITLE_TEXT_COLOR, /* Varies by depth, set in engines.h. */
      1 );
#endif

   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      if( !mobile_is_active( &(state->mobiles[i]) ) ) {
         continue;
      }
      /*
      if(
         MOBILE_FLAG_NOT_LAST !=
            (MOBILE_FLAG_NOT_LAST & mobiles[i].flags)
      ) {
         break;
      }
      */

      assert( 0 <= state->mobiles[i].sprite_cache_id );

      /* Draw current mobile sprite/frame. */
      retrogxc_blit_bitmap(
         NULL, state->mobiles[i].sprite_cache_id,
         state->ani_sprite_x,
         mobile_get_dir( &(state->mobiles[i]) ) * SPRITE_H,
         ((retroflat_screen_w() / 2) - SPRITE_W) + (state->mobiles[i].coords[1].x * SPRITE_W),
         (2 * SPRITE_H) + (state->mobiles[i].coords[1].y * SPRITE_H),
         SPRITE_W, SPRITE_H, i );
   }

#if 0
   window_refresh( WINDOW_ID_TITLE_MENU );
#endif

cleanup:

   return;

#endif /* !NO_TITLE */

}

void title_animate( struct DSEKAI_STATE* state ) {

}

int16_t title_input(
   struct RETROFLAT_INPUT* in_evt, RETROFLAT_IN_KEY in_char,
   struct DSEKAI_STATE* state
) {
   int16_t retval = 1;

#ifndef NO_TITLE

   struct TITLE_STATE* gstate = NULL;
   uint8_t redraw_menu = 0;
   title_option_cb cb = NULL;

   maug_mlock( state->engine_state_handle, gstate );

   if( RETROFLAT_KEY_UP == in_char ) {
      if( gstate->option_min < gstate->option_idx ) {
         gstate->option_idx--;
         /* window_pop( WINDOW_ID_TITLE_MENU ); */
         redraw_menu = 1;
      }

   } else if( RETROFLAT_KEY_DOWN == in_char ) {
      if( gstate->option_max - 1 > gstate->option_idx ) {
         gstate->option_idx++;
         /* window_pop( WINDOW_ID_TITLE_MENU ); */
         redraw_menu = 1;
      }

   } else if( RETROFLAT_KEY_ENTER == in_char ) {

      cb = gstate->option_callbacks[gstate->option_idx];

      /* Pop the window and unlock gstate for the callback! */
      window_pop( WINDOW_ID_TITLE_MENU );
      maug_munlock( state->engine_state_handle, gstate );

      cb( state );

      /* Don't set redraw, as the callback might redraw on its own. */

#ifndef NO_QUIT
   } else if( RETROFLAT_KEY_ESC == in_char ) {
      retval = 0;
#endif /*! NO_QUIT */
   }
 
   if( NULL != gstate ) {
      maug_munlock( state->engine_state_handle, gstate );
   }

   /* Redraw menu here after gstate freed. */
   if( redraw_menu ) {
      title_draw_menu( state, -1, NULL, NULL );
   }

#endif /* !NO_TITLE */

   return retval;
}

