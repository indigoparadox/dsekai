
#include <string.h>

#include "graphics.h"
#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"

static int g_semi_cycles = 0;
static int g_walk_offset = 0;
static int g_screen_scroll_x = 0;
static int g_screen_scroll_y = 0;
static int g_screen_scroll_tx = 0;
static int g_screen_scroll_ty = 0;
static int g_screen_scroll_x_tgt = 0;
static int g_screen_scroll_y_tgt = 0;
static uint8_t g_window_shown = 0;

extern const GRAPHICS_SPRITE gc_sprite_princess;
extern const struct TILEMAP gc_map_field;
extern const GRAPHICS_PATTERN gc_patterns[];

static
void topdown_refresh_tiles( uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW] ) {
   int x = 0, y = 0;
   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         (*tiles_flags)[y][x] |= TILEMAP_TILE_FLAG_DIRTY;
      }
   }
}

int topdown_loop(
   struct MOBILE* player,
   struct MOBILE mobiles[MOBILES_MAX], int* mobiles_count,
   uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW]
) {
   uint32_t i = 0;
   uint8_t in_char = 0;
   struct WINDOW* w = NULL;

   assert( MOBILES_MAX > *mobiles_count );
   assert( 0 <= *mobiles_count );

   graphics_loop_start();

   in_char = 0;

   if( 0 == windows_visible() ) {
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

         assert( 0 <= g_screen_scroll_x );
         assert( 0 <= g_screen_scroll_tx );

         topdown_refresh_tiles( tiles_flags );
#ifdef ANIMATE_SCREEN_MOVEMENT
         tilemap_draw( &gc_map_field, tiles_flags,
            g_screen_scroll_x, g_screen_scroll_y, 1 );

         graphics_flip();
#endif /* ANIMATE_SCREEN_MOVEMENT */
         return 1;
      }

      tilemap_draw( &gc_map_field, tiles_flags,
         g_screen_scroll_x, g_screen_scroll_y, 0 );

      mobile_draw(
         player, g_walk_offset, g_screen_scroll_x, g_screen_scroll_y );
      for( i = 0 ; *mobiles_count > i ; i++ ) {
         if(
            mobiles[i].coords.x < g_screen_scroll_tx ||
            mobiles[i].coords.y < g_screen_scroll_ty ||
            mobiles[i].coords.x > g_screen_scroll_tx + SCREEN_TW ||
            mobiles[i].coords.y > g_screen_scroll_ty + SCREEN_TH
         ) {
            /* Mobile is off-screen. */
            continue;
         }
         mobile_draw(
            &(mobiles[i]),
            g_walk_offset, g_screen_scroll_x, g_screen_scroll_y );
      }
   }

   window_draw_all();

   graphics_flip();

   if( !g_window_shown ) {
      w = window_push();
      w->pattern = &(gc_patterns[0]);
      w->w = 80;
      w->h = 64;
      w->dirty = 1;
      w->state = WINDOW_STATE_VISIBLE;
      w->strings[0] = "Welcome!";
      w->strings_count = 1;
      w->strings_color = GRAPHICS_COLOR_BLACK;
      g_window_shown = 1;

      mobiles[0].sprite = &gc_sprite_princess;
      mobiles[0].hp = 100;
      mobiles[0].mp = 100;
      mobiles[0].coords.x = 5;
      mobiles[0].coords.y = 5;
      mobiles[0].coords_prev.x = 5;
      mobiles[0].coords_prev.y = 5;
      mobiles[0].steps = SPRITE_W;
      (*mobiles_count)++;
   }

   assert( mobiles[0].sprite == &gc_sprite_princess );
   assert( mobiles[0].coords_prev.x == 5 );
   assert( mobiles[0].coords_prev.y == 5 );

   in_char = input_poll();

   /* Quit on Q. */
   switch( in_char ) {
   case INPUT_KEY_W:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &gc_map_field, player->coords.x, player->coords.y - 1 )
      ) {
         mobile_walk_start( player, 0, -1 );
      }
      break;

   case INPUT_KEY_A:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &gc_map_field, player->coords.x - 1, player->coords.y )
      ) {
         mobile_walk_start( player, -1, 0 );
      }
      break;

   case INPUT_KEY_S:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &gc_map_field, player->coords.x, player->coords.y + 1 )
      ) {
         mobile_walk_start( player, 0, 1 );
      }
      break;

   case INPUT_KEY_D:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &gc_map_field, player->coords.x + 1, player->coords.y )
      ) {
         mobile_walk_start( player, 1, 0 );
      }
      break;

   case INPUT_KEY_Z:
      window_pop();
      topdown_refresh_tiles( tiles_flags );
      break;

   case INPUT_KEY_Q:
      return 0;
   }

   /* Scroll the screen by one if the player goes off-screen. */
   if( player->coords.x >= g_screen_scroll_tx + SCREEN_TW ) {
      g_screen_scroll_x_tgt = g_screen_scroll_x + SCREEN_W;
   } else if( player->coords.y >= g_screen_scroll_y + SCREEN_TH ) {
      g_screen_scroll_y_tgt = g_screen_scroll_y + SCREEN_H;
   } else if( player->coords.x < g_screen_scroll_tx ) {
      g_screen_scroll_x_tgt = g_screen_scroll_x - SCREEN_W;
   } else if( player->coords.y < g_screen_scroll_ty ) {
      g_screen_scroll_y_tgt = g_screen_scroll_y - SCREEN_H;
   }

   /* Animate. */
   if( 10 < g_semi_cycles ) {
      g_semi_cycles = 0;

      if( 0 == g_walk_offset ) {
         g_walk_offset = 1;
      } else {
         g_walk_offset = 0;
      }

   } else {
      g_semi_cycles++;
   }

   mobile_animate( player, tiles_flags );
   for( i = 0 ; *mobiles_count > i ; i++ ) {
      assert( mobiles[i].coords_prev.x == 5 );
      assert( mobiles[i].coords_prev.y == 5 );
      mobile_animate( &(mobiles[i]), tiles_flags );
   }

   graphics_loop_end();

   return 1;
}

