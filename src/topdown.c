
#include "data/maps.h"

#include "graphics.h"
#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#ifndef MEMORY_STATIC
#include "memory.h"
#endif /* !MEMORY_STATIC */

#include "data/windows.h"

#define INPUT_BLOCK_DELAY 5
#define TOPDOWN_MOBILES_MAX 10

#ifdef MEMORY_STATIC
static uint8_t g_tiles_flags[TILEMAP_TH * TILEMAP_TW];
static struct MOBILE g_mobiles[TOPDOWN_MOBILES_MAX];
#else
static struct MOBILE* g_mobiles = NULL;
static uint8_t* g_tiles_flags = NULL;
#endif /* MEMORY_STATIC */
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
static struct MOBILE* g_player = NULL;

static
void topdown_refresh_tiles( uint8_t* tiles_flags ) {
   int x = 0, y = 0;
   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         tiles_flags[(y * TILEMAP_TW) + x] |= TILEMAP_TILE_FLAG_DIRTY;
      }
   }
}

int topdown_draw() {
   int in_char = 0,
      i = 0;

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

      topdown_refresh_tiles( g_tiles_flags );
#ifdef ANIMATE_SCREEN_MOVEMENT
#ifndef DISABLE_GRAPHICS
      tilemap_draw( &g_map_field, g_tiles_flags,
         TILEMAP_TW, TILEMAP_TH,
         g_screen_scroll_x, g_screen_scroll_y, 1 );

      graphics_flip();
#endif /* !DISABLE_GRAPHICS */
#endif /* ANIMATE_SCREEN_MOVEMENT */

      /* Drain input. */
      in_char = input_poll();
      if( INPUT_KEY_QUIT == in_char ) {
         return 0;
      }

      if(
         g_screen_scroll_y == g_screen_scroll_y_tgt &&
         g_screen_scroll_x == g_screen_scroll_x_tgt
      ) {
         /* Screen scroll finished. */
         g_player->coords_prev.x = g_player->coords.x;
         g_player->coords_prev.y = g_player->coords.y;
         g_player->steps_x = 0;
         g_player->steps_y = 0;

      }

      /* Keep running. */
      return 1;
   }

#ifndef DISABLE_GRAPHICS
   tilemap_draw( &g_map_field, g_tiles_flags,
      TILEMAP_TW, TILEMAP_TH,
      g_screen_scroll_x, g_screen_scroll_y, 0 );
#endif /* !DISABLE_GRAPHICS */

   for( i = 0 ; g_mobiles_count > i ; i++ ) {
      if(
         g_mobiles[i].coords.x < g_screen_scroll_tx ||
         g_mobiles[i].coords.y < g_screen_scroll_ty ||
         g_mobiles[i].coords.x > g_screen_scroll_tx + SCREEN_TW ||
         g_mobiles[i].coords.y > g_screen_scroll_ty + SCREEN_TH
      ) {
         /* Mobile is off-screen. */
         continue;
      }
#ifndef DISABLE_GRAPHICS
      mobile_draw(
         &(g_mobiles[i]),
         g_walk_offset, g_screen_scroll_x, g_screen_scroll_y );
      assert( NULL != g_player );
#endif /* !DISABLE_GRAPHICS */
   }

   /* Keep running. */
   return 1;
}

int topdown_loop() {
   int i = 0;
   uint8_t in_char = 0;
   struct WINDOW* w = NULL;
   static int initialized = 0;

   if( !initialized ) {
      /* TODO: Generate this dynamically. */
#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( tile_field_grass, &(g_map_field.tileset[0]) );
      graphics_load_bitmap( tile_field_brick_wall, &(g_map_field.tileset[1]) );
      graphics_load_bitmap( tile_field_tree, &(g_map_field.tileset[2]) );
#endif /* !DISABLE_GRAPHICS */

#ifndef MEMORY_STATIC
      g_tiles_flags = memory_alloc( TILEMAP_TH * TILEMAP_TW, 1 );
      assert( NULL != g_tiles_flags );
      g_mobiles = memory_alloc( 2, sizeof( struct MOBILE ) );
      assert( NULL != g_mobiles );
#endif /* MEMORY_STATIC */

#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( sprite_robe, &(g_mobiles[0].sprite) );
#endif /* !DISABLE_GRAPHICS */
      g_mobiles[0].hp = 100;
      g_mobiles[0].mp = 100;
      g_mobiles[0].coords.x = 3;
      g_mobiles[0].coords.y = 4;
      g_mobiles[0].coords_prev.x = 3;
      g_mobiles[0].coords_prev.y = 4;
      g_mobiles[0].steps_x = 0;
      g_mobiles[0].steps_y = 0;
      g_mobiles[0].inventory = NULL;
      g_mobiles_count++;

      g_player = &(g_mobiles[0]);
      assert( NULL != g_player );

#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( sprite_princess, &(g_mobiles[1].sprite) );
#endif /* !DISABLE_GRAPHICS */
      g_mobiles[1].hp = 100;
      g_mobiles[1].mp = 100;
      g_mobiles[1].coords.x = 5;
      g_mobiles[1].coords.y = 5;
      g_mobiles[1].coords_prev.x = 5;
      g_mobiles[1].coords_prev.y = 5;
      g_mobiles[1].steps_x = 0;
      g_mobiles[1].steps_y = 0;
      g_mobiles[1].inventory = NULL;
      g_mobiles_count++;

      /* Make sure the tilemap is drawn at least once behind any initial
       * windows.
       */
      tilemap_draw( &g_map_field, g_tiles_flags,
         TILEMAP_TW, TILEMAP_TH,
         g_screen_scroll_x, g_screen_scroll_y, 1 );

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
      w->frame = &g_pattern_cm_checker;
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

   assert( g_mobiles[1].coords_prev.x == 5 );
   assert( g_mobiles[1].coords_prev.y == 5 );

   if( g_input_blocked_countdown ) {
      g_input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   switch( in_char ) {
   case INPUT_KEY_UP:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &g_map_field, g_player->coords.x, g_player->coords.y - 1 )
      ) {
         mobile_walk_start( g_player, 0, -1 );
      }
      break;

   case INPUT_KEY_LEFT:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &g_map_field, g_player->coords.x - 1, g_player->coords.y )
      ) {
         mobile_walk_start( g_player, -1, 0 );
      }
      break;

   case INPUT_KEY_DOWN:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &g_map_field, g_player->coords.x, g_player->coords.y + 1 )
      ) {
         mobile_walk_start( g_player, 0, 1 );
      }
      break;

   case INPUT_KEY_RIGHT:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &g_map_field, g_player->coords.x + 1, g_player->coords.y )
      ) {
         mobile_walk_start( g_player, 1, 0 );
      }
      break;

   case INPUT_KEY_OK:
      window_pop();
      topdown_refresh_tiles( g_tiles_flags );
      break;

   case INPUT_KEY_QUIT:
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
      mobile_animate( &(g_mobiles[i]), g_tiles_flags, TILEMAP_TW, TILEMAP_TH );
   }

   /* Scroll the screen by one if the player goes off-screen. */
   assert( NULL != g_player );
   if( g_player->coords.x >= g_screen_scroll_tx + SCREEN_TW ) {
      g_screen_scroll_x_tgt = g_screen_scroll_x + SCREEN_W;
   } else if( g_player->coords.y >= g_screen_scroll_y + SCREEN_TH ) {
      g_screen_scroll_y_tgt = g_screen_scroll_y + SCREEN_H;
   } else if( g_player->coords.x < g_screen_scroll_tx ) {
      g_screen_scroll_x_tgt = g_screen_scroll_x - SCREEN_W;
   } else if( g_player->coords.y < g_screen_scroll_ty ) {
      g_screen_scroll_y_tgt = g_screen_scroll_y - SCREEN_H;
   }

   graphics_loop_end();

   return 1;
}

void topdown_deinit() {
   int i = 0;

   for( i = 0 ; g_mobiles_count > i ; i++ ) {
      mobile_deinit( &(g_mobiles[i]) );
   }

   tilemap_deinit( &g_map_field );

#ifndef MEMORY_STATIC
   memory_free( &g_mobiles );
   memory_free( &g_tiles_flags );
#endif /* !MEMORY_STATIC */
}

