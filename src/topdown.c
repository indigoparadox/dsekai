
#include "dstypes.h"

#include "data/tiles.h"
#ifndef USE_JSON_MAPS
#if defined( PLATFORM_WIN16 ) || defined( PLATFORM_WINCE )
#include "../gen/win16/map_field.h"
#elif defined( PLATFORM_WIN32 )
#include "../gen/win32/map_field.h"
#elif defined( PLATFORM_MAC7 )
#include "../gen/mac7/map_field.h"
#elif defined( PLATFORM_NDS )
#include "../gen/nds/map_field.h"
#elif defined( PLATFORM_DOS )
#include "../gen/dos/map_field.h"
#elif defined( PLATFORM_SDL )
#include "../gen/sdl-nj/map_field.h"
#elif defined( PLATFORM_XLIB )
#include "../gen/xlib/map_field.h"
#endif
#endif /* USE_JSON_MAPS */

#define TOPDOWN_STATE_WELCOME 1

int topdown_draw(
   struct DSEKAI_STATE* state, struct TILEMAP* map, struct MOBILE* mobiles
) {
   int in_char = 0,
      i = 0;

   /*
   If the screen is scrolling, prioritize that before accepting more
   commands or drawing mobiles.
   */
   if(
      state->screen_scroll_x_tgt != state->screen_scroll_x ||
      state->screen_scroll_y_tgt != state->screen_scroll_y 
   ) {
      if( state->screen_scroll_x_tgt > state->screen_scroll_x ) {
         state->screen_scroll_x += TILE_W;
         state->screen_scroll_tx = state->screen_scroll_x / TILE_W;
      }
      if( state->screen_scroll_y_tgt > state->screen_scroll_y ) {
         state->screen_scroll_y += TILE_H;
         state->screen_scroll_ty = state->screen_scroll_y / TILE_H;
      }
      if( state->screen_scroll_x_tgt < state->screen_scroll_x ) {
         state->screen_scroll_x -= TILE_W;
         state->screen_scroll_tx = state->screen_scroll_x / TILE_W;
      }
      if( state->screen_scroll_y_tgt < state->screen_scroll_y ) {
         state->screen_scroll_y -= TILE_H;
         state->screen_scroll_ty = state->screen_scroll_y / TILE_H;
      }

      state->input_blocked_countdown = INPUT_BLOCK_DELAY;

      assert( 0 <= state->screen_scroll_x );
      assert( 0 <= state->screen_scroll_tx );

      tilemap_refresh_tiles( map );
#ifdef ANIMATE_SCREEN_MOVEMENT
#ifndef DISABLE_GRAPHICS
      tilemap_draw( map, state );
#endif /* !DISABLE_GRAPHICS */
#endif /* ANIMATE_SCREEN_MOVEMENT */

      /* Drain input. */
      in_char = input_poll();
      if( INPUT_KEY_QUIT == in_char ) {
         return 0;
      }

      if(
         state->screen_scroll_y == state->screen_scroll_y_tgt &&
         state->screen_scroll_x == state->screen_scroll_x_tgt
      ) {
         /* Screen scroll finished. */
         mobiles[state->player_idx].coords_prev.x =
            mobiles[state->player_idx].coords.x;
         mobiles[state->player_idx].coords_prev.y =
            mobiles[state->player_idx].coords.y;
         mobiles[state->player_idx].steps_x = 0;
         mobiles[state->player_idx].steps_y = 0;
      }

      /* Keep running. */
      return 1;
   }

#ifndef DISABLE_GRAPHICS
   tilemap_draw( map, state );
#endif /* !DISABLE_GRAPHICS */

   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      if(
         mobiles[i].coords.x < state->screen_scroll_tx ||
         mobiles[i].coords.y < state->screen_scroll_ty ||
         mobiles[i].coords.x > state->screen_scroll_tx + SCREEN_TW ||
         mobiles[i].coords.y > state->screen_scroll_ty + SCREEN_TH
      ) {
         /* Mobile is off-screen. */
         continue;
      }
#ifndef DISABLE_GRAPHICS
      mobile_draw(
         &(mobiles[i]),
         state->walk_offset, state->screen_scroll_x, state->screen_scroll_y );
      assert( NULL != &(mobiles[state->player_idx]) );
#endif /* !DISABLE_GRAPHICS */
   }

   /* Keep running. */
   return 1;
}

int topdown_loop( MEMORY_HANDLE state_handle ) {
   int i = 0;
   uint8_t in_char = 0;
   static int initialized = 0;
   struct DSEKAI_STATE* state = NULL;
   MEMORY_HANDLE welcome_string_handle = NULL;
   char* welcome_string = NULL;
   struct TILEMAP* map = NULL;
   struct MOBILE* mobiles = NULL;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );

   if( !initialized ) {

      assert( NULL == state->map );
      state->map = memory_alloc( sizeof( struct TILEMAP ), 1 );
      assert( NULL != state->map );
      map = memory_lock( state->map );
      assert( NULL != map );

      assert( NULL == state->mobiles );
      state->mobiles = memory_alloc( sizeof( struct MOBILE ), MOBILES_MAX );
      assert( NULL != state->mobiles );
      mobiles = memory_lock( state->mobiles );
      assert( NULL != mobiles );

#ifdef USE_JSON_MAPS
      tilemap_load( map_field, map );
#else
      memory_copy_ptr( (MEMORY_PTR)map, (MEMORY_PTR)&gc_map_field,
         sizeof( struct TILEMAP ) );
#endif /* USE_JSON_MAPS */
      memory_copy_ptr( (MEMORY_PTR)&(map->tileset[0]),
         (MEMORY_PTR)&(gc_tiles_field[0]),
         TILEMAP_TILESETS_MAX * sizeof( struct TILESET_TILE ) );
      tilemap_refresh_tiles( map );

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      tilemap_draw( map, state );

      /* TODO: Generate this dynamically. */
      for( i = 0 ; map->spawns_count > i ; i++ ) {
         mobiles[i].hp = 100;
         mobiles[i].mp = 100;
         mobiles[i].coords.x = map->spawns[i].coords.x;
         mobiles[i].coords.y = map->spawns[i].coords.y;
         mobiles[i].coords_prev.x = map->spawns[i].coords.x;
         mobiles[i].coords_prev.y = map->spawns[i].coords.y;
         mobiles[i].steps_x = 0;
         mobiles[i].steps_y = 0;
         mobiles[i].inventory = NULL;
         state->mobiles_count++;
         switch( map->spawns[i].type ) {
         case MOBILE_TYPE_PLAYER:
            mobiles[i].active = 1;
            state->player_idx = i;
            mobiles[i].sprite = sprite_robe;
            break;

         case MOBILE_TYPE_PRINCESS:
            mobiles[i].active = 1;
            mobiles[i].sprite = sprite_princess;
            break;

         default:
            break;
         }
      }

      mobiles = memory_unlock( state->mobiles );
      map = memory_unlock( state->map );

      window_push(
         0x111, WINDOW_STATUS_VISIBLE,
         0, (SCREEN_TH * 16), STATUS_WINDOW_W, STATUS_WINDOW_H, 0, state );

#ifndef HIDE_WELCOME_DIALOG
      state->engine_state = TOPDOWN_STATE_WELCOME;
      window_push(
         0x1212, WINDOW_STATUS_MODAL,
         WINDOW_CENTERED, WINDOW_CENTERED, 80, 64, 0, state );
      welcome_string_handle = memory_alloc( 9, 1 );
      welcome_string = (char*)memory_lock( welcome_string_handle );
      memory_copy_ptr( welcome_string, "Welcome!", 8 );
      welcome_string = (char*)memory_unlock( welcome_string_handle );
      control_push(
         0x2323, CONTROL_TYPE_LABEL, CONTROL_STATE_ENABLED,
         -1, -1, -1, -1, GRAPHICS_COLOR_BLACK, GRAPHICS_COLOR_MAGENTA, 1,
         welcome_string_handle, 0, 0x1212, state );
      control_push(
         0x2324, CONTROL_TYPE_SPRITE, CONTROL_STATE_ENABLED,
         -1, 6, -1, -1, GRAPHICS_COLOR_BLACK, GRAPHICS_COLOR_MAGENTA, 1,
         NULL, sprite_princess, 0x1212, state );
#endif /* !HIDE_WELCOME_DIALOG */

      initialized = 1;
   }

   graphics_loop_start();

   in_char = 0;

   map = memory_lock( state->map );
   assert( NULL != map );
   mobiles = memory_lock( state->mobiles );
   assert( NULL != mobiles );

   if( 0 >= window_modal( state ) ) {
      topdown_draw( state, map, mobiles );
   }

   window_draw_all( state );

   if( state->input_blocked_countdown ) {
      state->input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   switch( in_char ) {
   case INPUT_KEY_UP:
      if( 0 < window_modal( state ) ) { break; }
      if( !tilemap_collide(
         map, mobiles[state->player_idx].coords.x,
         mobiles[state->player_idx].coords.y - 1 )
      ) {
         mobile_walk_start( &(mobiles[state->player_idx]), 0, -1 );
      }
      break;

   case INPUT_KEY_LEFT:
      if( 0 < window_modal( state ) ) { break; }
      if( !tilemap_collide(
         map, mobiles[state->player_idx].coords.x - 1,
         mobiles[state->player_idx].coords.y )
      ) {
         mobile_walk_start( &(mobiles[state->player_idx]), -1, 0 );
      }
      break;

   case INPUT_KEY_DOWN:
      if( 0 < window_modal( state ) ) { break; }
      if( !tilemap_collide(
         map, mobiles[state->player_idx].coords.x,
         mobiles[state->player_idx].coords.y + 1 )
      ) {
         mobile_walk_start( &(mobiles[state->player_idx]), 0, 1 );
      }
      break;

   case INPUT_KEY_RIGHT:
      if( 0 < window_modal( state ) ) { break; }
      if( !tilemap_collide(
         map, mobiles[state->player_idx].coords.x + 1,
         mobiles[state->player_idx].coords.y )
      ) {
         mobile_walk_start( &(mobiles[state->player_idx]), 1, 0 );
      }
      break;

   case INPUT_KEY_OK:
      window_pop( 0x1212, state );
      tilemap_refresh_tiles( map );
      break;

   case INPUT_KEY_QUIT:
      window_pop( 0x111, state );
      mobiles =  memory_unlock( state->mobiles );
      map = memory_unlock( state->map );
      state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
      topdown_deinit( state_handle );
      return 0;
   }

   /* Animate. */
   if( ANI_SEMICYCLES_MAX < state->semi_cycles ) {
      state->semi_cycles = 0;

      if( 0 == state->walk_offset ) {
         /* Walk offset must be divisible by 2 for CGA blitting to work. */
         state->walk_offset = 2;
      } else {
         state->walk_offset = 0;
      }

   } else {
      state->semi_cycles++;
   }

   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      mobile_animate( &(mobiles[i]), map );
   }

   /* Scroll the screen by one if the player goes off-screen. */
   if(
      mobiles[state->player_idx].coords.x >= state->screen_scroll_tx + SCREEN_TW
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x + SCREEN_MAP_W;
      debug_printf( 1, "scrolling screen right to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      mobiles[state->player_idx].coords.y >= state->screen_scroll_y + SCREEN_TH
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y + SCREEN_MAP_H;
      debug_printf( 1, "scrolling screen down to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      mobiles[state->player_idx].coords.x < state->screen_scroll_tx
   ) {
      state->screen_scroll_x_tgt = state->screen_scroll_x - SCREEN_MAP_W;
      debug_printf( 1, "scrolling screen left to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );

   } else if(
      mobiles[state->player_idx].coords.y < state->screen_scroll_ty
   ) {
      state->screen_scroll_y_tgt = state->screen_scroll_y - SCREEN_MAP_H;
      debug_printf( 1, "scrolling screen up to %d, %d...",
         state->screen_scroll_x_tgt, state->screen_scroll_y_tgt );
   }
   mobiles = memory_unlock( state->mobiles );
   map = memory_unlock( state->map );
   state = (struct DSEKAI_STATE*)memory_unlock( state_handle );

   graphics_loop_end();

   return 1;
}

void topdown_deinit( MEMORY_HANDLE state_handle ) {
   int i = 0;
   struct DSEKAI_STATE* state = NULL;
   struct TILEMAP* map = NULL;
   struct MOBILE* mobiles = NULL;

   state = (struct DSEKAI_STATE*)memory_lock( state_handle );
   mobiles = memory_lock( state->mobiles );
   map = memory_lock( state->map );

   for( i = 0 ; state->mobiles_count > i ; i++ ) {
      mobile_deinit( &(mobiles[i]) );
   }

   tilemap_deinit( map );

   map = memory_unlock( state->map );
   mobiles = memory_unlock( state->mobiles );
   state = (struct DSEKAI_STATE*)memory_unlock( state_handle );
}

