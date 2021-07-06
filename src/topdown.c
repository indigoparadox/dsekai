
#include "graphics.h"
#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "memory.h"

#define INPUT_BLOCK_DELAY 5
#define TOPDOWN_MOBILES_MAX 10

static MEMORY_HANDLE g_mobiles_handle = NULL;
static MEMORY_HANDLE g_map_handle = NULL;
static uint8_t g_mobiles_count = 0; 
static int g_semi_cycles = 0;
static int g_walk_offset = 0;
static int g_screen_scroll_x = 0;
static int g_screen_scroll_y = 0;
static int g_screen_scroll_tx = 0;
static int g_screen_scroll_ty = 0;
static int g_screen_scroll_x_tgt = 0;
static int g_screen_scroll_y_tgt = 0;
static uint8_t g_window_shown = 0;
static uint8_t g_input_blocked_countdown = 0;
static int g_player_idx = 0;

int topdown_draw() {
   int in_char = 0,
      i = 0;
   struct MOBILE* mobiles = NULL;
   struct TILEMAP* map = NULL;

   /*
   If the screen is scrolling, prioritize that before accepting more
   commands or drawing mobiles.
   */
   if(
      g_screen_scroll_x_tgt != g_screen_scroll_x ||
      g_screen_scroll_y_tgt != g_screen_scroll_y 
   ) {
      if( g_screen_scroll_x_tgt > g_screen_scroll_x ) {
         g_screen_scroll_x += TILE_W;
         g_screen_scroll_tx = g_screen_scroll_x / TILE_W;
      }
      if( g_screen_scroll_y_tgt > g_screen_scroll_y ) {
         g_screen_scroll_y += TILE_H;
         g_screen_scroll_ty = g_screen_scroll_y / TILE_H;
      }
      if( g_screen_scroll_x_tgt < g_screen_scroll_x ) {
         g_screen_scroll_x -= TILE_W;
         g_screen_scroll_tx = g_screen_scroll_x / TILE_W;
      }
      if( g_screen_scroll_y_tgt < g_screen_scroll_y ) {
         g_screen_scroll_y -= TILE_H;
         g_screen_scroll_ty = g_screen_scroll_y / TILE_H;
      }

      g_input_blocked_countdown = INPUT_BLOCK_DELAY;

      assert( 0 <= g_screen_scroll_x );
      assert( 0 <= g_screen_scroll_tx );

      map = memory_lock( g_map_handle );
      tilemap_refresh_tiles( map );
#ifdef ANIMATE_SCREEN_MOVEMENT
#ifndef DISABLE_GRAPHICS
      tilemap_draw( map, g_screen_scroll_x, g_screen_scroll_y, 1 );

      graphics_flip();
#endif /* !DISABLE_GRAPHICS */
#endif /* ANIMATE_SCREEN_MOVEMENT */
      map = memory_unlock( g_map_handle );

      /* Drain input. */
      in_char = input_poll();
      if( INPUT_KEY_QUIT == in_char ) {
         return 0;
      }

      mobiles = memory_lock( g_mobiles_handle );
      if(
         g_screen_scroll_y == g_screen_scroll_y_tgt &&
         g_screen_scroll_x == g_screen_scroll_x_tgt
      ) {
         /* Screen scroll finished. */
         mobiles[g_player_idx].coords_prev.x = mobiles[g_player_idx].coords.x;
         mobiles[g_player_idx].coords_prev.y = mobiles[g_player_idx].coords.y;
         mobiles[g_player_idx].steps_x = 0;
         mobiles[g_player_idx].steps_y = 0;
      }
      mobiles = memory_unlock( g_mobiles_handle );

      /* Keep running. */
      return 1;
   }

#ifndef DISABLE_GRAPHICS
   map = memory_lock( g_map_handle );
   tilemap_draw( map, g_screen_scroll_x, g_screen_scroll_y, 0 );
   map = memory_unlock( g_map_handle );
#endif /* !DISABLE_GRAPHICS */

   mobiles = memory_lock( g_mobiles_handle );
   for( i = 0 ; g_mobiles_count > i ; i++ ) {
      if(
         mobiles[i].coords.x < g_screen_scroll_tx ||
         mobiles[i].coords.y < g_screen_scroll_ty ||
         mobiles[i].coords.x > g_screen_scroll_tx + SCREEN_TW ||
         mobiles[i].coords.y > g_screen_scroll_ty + SCREEN_TH
      ) {
         /* Mobile is off-screen. */
         continue;
      }
#ifndef DISABLE_GRAPHICS
      mobile_draw(
         &(mobiles[i]),
         g_walk_offset, g_screen_scroll_x, g_screen_scroll_y );
      assert( NULL != &(mobiles[g_player_idx]) );
#endif /* !DISABLE_GRAPHICS */
   }
   mobiles = memory_unlock( g_mobiles_handle );

   /* Keep running. */
   return 1;
}

int topdown_loop() {
   int i = 0;
   uint8_t in_char = 0;
   struct WINDOW* w = NULL;
   static int initialized = 0;
   struct MOBILE* mobiles = NULL;
   struct TILEMAP* map = NULL;

   if( !initialized ) {

      g_map_handle = memory_alloc( sizeof( struct TILEMAP ), 1 );
      g_mobiles_handle = memory_alloc( 2, sizeof( struct MOBILE ) );
      if( NULL == g_mobiles_handle ) {
         error_printf( "unable to allocate space for mobiles" );
         return 0;
      }

      /* TODO: Generate this dynamically. */
#ifndef DISABLE_GRAPHICS
      map = memory_lock( g_map_handle );
      graphics_load_bitmap( tile_field_grass, &(map->tileset[0]) );
      graphics_load_bitmap( tile_field_brick_wall, &(map->tileset[1]) );
      graphics_load_bitmap( tile_field_tree, &(map->tileset[2]) );
      map = memory_unlock( g_map_handle );
#endif /* !DISABLE_GRAPHICS */

      mobiles = memory_lock( g_mobiles_handle );
#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( sprite_robe, &(mobiles[0].sprite) );
#endif /* !DISABLE_GRAPHICS */
      mobiles[0].hp = 100;
      mobiles[0].mp = 100;
      mobiles[0].coords.x = 3;
      mobiles[0].coords.y = 4;
      mobiles[0].coords_prev.x = 3;
      mobiles[0].coords_prev.y = 4;
      mobiles[0].steps_x = 0;
      mobiles[0].steps_y = 0;
      mobiles[0].inventory = NULL;
      g_mobiles_count++;

#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( sprite_princess, &(mobiles[1].sprite) );
#endif /* !DISABLE_GRAPHICS */
      mobiles[1].hp = 100;
      mobiles[1].mp = 100;
      mobiles[1].coords.x = 5;
      mobiles[1].coords.y = 5;
      mobiles[1].coords_prev.x = 5;
      mobiles[1].coords_prev.y = 5;
      mobiles[1].steps_x = 0;
      mobiles[1].steps_y = 0;
      mobiles[1].inventory = NULL;
      g_mobiles_count++;
      mobiles = memory_unlock( g_mobiles_handle );

      map = memory_lock( g_map_handle );
      tilemap_load( map_field, map );

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      tilemap_draw( map, g_screen_scroll_x, g_screen_scroll_y, 1 );
      map = memory_unlock( g_map_handle );

      initialized = 1;
   }

   graphics_loop_start();

   in_char = 0;

   if( 0 == windows_visible() ) {
      topdown_draw();
   }

   if( !g_window_shown ) {
#ifndef HIDE_WELCOME_DIALOG
      w = window_push();
      w->frame_idx = 0;
      w->w = 80;
      w->h = 64;
      w->dirty = DIRTY_THRESHOLD;
      w->state = WINDOW_STATE_VISIBLE;
      w->strings[0] = "Welcome!";
      w->strings_count = 1;
      w->strings_color = GRAPHICS_COLOR_BLACK;
#endif /* !HIDE_WELCOME_DIALOG */

      g_window_shown = 1;

   }

   window_draw_all();

   graphics_flip();

   if( g_input_blocked_countdown ) {
      g_input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   mobiles = memory_lock( g_mobiles_handle );
   map = memory_lock( g_map_handle );
   switch( in_char ) {
   case INPUT_KEY_UP:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         map, mobiles[g_player_idx].coords.x,
         mobiles[g_player_idx].coords.y - 1 )
      ) {
         mobile_walk_start( &(mobiles[g_player_idx]), 0, -1 );
      }
      break;

   case INPUT_KEY_LEFT:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         map, mobiles[g_player_idx].coords.x - 1,
         mobiles[g_player_idx].coords.y )
      ) {
         mobile_walk_start( &(mobiles[g_player_idx]), -1, 0 );
      }
      break;

   case INPUT_KEY_DOWN:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         map, mobiles[g_player_idx].coords.x,
         mobiles[g_player_idx].coords.y + 1 )
      ) {
         mobile_walk_start( &(mobiles[g_player_idx]), 0, 1 );
      }
      break;

   case INPUT_KEY_RIGHT:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         map, mobiles[g_player_idx].coords.x + 1,
         mobiles[g_player_idx].coords.y )
      ) {
         mobile_walk_start( &(mobiles[g_player_idx]), 1, 0 );
      }
      break;

   case INPUT_KEY_OK:
      window_pop();
      tilemap_refresh_tiles( map );
      break;

   case INPUT_KEY_QUIT:
      mobiles = memory_unlock( g_mobiles_handle );
      map = memory_unlock( g_map_handle );
      topdown_deinit();
      return 0;
   }

   /* Animate. */
   if( 10 < g_semi_cycles ) {
      g_semi_cycles = 0;

      if( 0 == g_walk_offset ) {
         /* Walk offset must be divisible by 2 for CGA blitting to work. */
         g_walk_offset = 2;
      } else {
         g_walk_offset = 0;
      }

   } else {
      g_semi_cycles++;
   }

   for( i = 0 ; g_mobiles_count > i ; i++ ) {
      mobile_animate( &(mobiles[i]), map );
   }

   /* Scroll the screen by one if the player goes off-screen. */
   if( mobiles[g_player_idx].coords.x >= g_screen_scroll_tx + SCREEN_TW ) {
      g_screen_scroll_x_tgt = g_screen_scroll_x + SCREEN_W;
   } else if(
      mobiles[g_player_idx].coords.y >= g_screen_scroll_y + SCREEN_TH
   ) {
      g_screen_scroll_y_tgt = g_screen_scroll_y + SCREEN_H;
   } else if( mobiles[g_player_idx].coords.x < g_screen_scroll_tx ) {
      g_screen_scroll_x_tgt = g_screen_scroll_x - SCREEN_W;
   } else if( mobiles[g_player_idx].coords.y < g_screen_scroll_ty ) {
      g_screen_scroll_y_tgt = g_screen_scroll_y - SCREEN_H;
   }
   mobiles = memory_unlock( g_mobiles_handle );
   map = memory_unlock( g_map_handle );

   graphics_loop_end();

   return 1;
}

void topdown_deinit() {
   int i = 0;
   struct MOBILE* mobiles = NULL;
   struct TILEMAP* map = NULL;

   mobiles = memory_lock( g_mobiles_handle );
   for( i = 0 ; g_mobiles_count > i ; i++ ) {
      mobile_deinit( &(mobiles[i]) );
   }
   mobiles = memory_unlock( g_mobiles_handle );

   map = memory_lock( g_map_handle );
   tilemap_deinit( map );
   map = memory_unlock( g_map_handle );

   memory_free( g_mobiles_handle );
   memory_free( g_map_handle );
}

