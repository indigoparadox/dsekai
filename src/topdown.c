
#include "data/maps.h"
#include "data/patterns.h"

#include "graphics.h"
#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "memory.h"

#define INPUT_BLOCK_DELAY 5

static
void topdown_refresh_tiles( uint8_t* tiles_flags ) {
   int x = 0, y = 0;
   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         tiles_flags[(y * TILEMAP_TW) + x] |= TILEMAP_TILE_FLAG_DIRTY;
      }
   }
}

int topdown_loop(
) {
   uint32_t i = 0;
   uint8_t in_char = 0;
   struct WINDOW* w = NULL;
   static struct MOBILE* player = NULL;
   static int initialized = 0;
static struct MOBILE* mobiles = NULL;
static uint8_t mobiles_count = 0; 
static uint8_t* tiles_flags = NULL;
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

   if( !initialized ) {
      /* TODO: Generate this dynamically. */
      g_map_field.tileset = memory_alloc(
         sizeof( struct GRAPHICS_BITMAP* ), TILEMAP_TILESETS_MAX );
      assert( NULL != g_map_field.tileset );
      if( NULL == g_map_field.tileset ) {
         return 1;
      }
#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( gc_tile_field_grass, &(g_map_field.tileset[0]) );
      graphics_load_bitmap( gc_tile_field_brick_wall, &(g_map_field.tileset[1]) );
      graphics_load_bitmap( gc_tile_field_tree, &(g_map_field.tileset[2]) );
#endif /* !DISABLE_GRAPHICS */

      tiles_flags = memory_alloc( TILEMAP_TH * TILEMAP_TW, 1 );
      assert( NULL != tiles_flags );
      mobiles = memory_alloc( 2, sizeof( struct MOBILE ) );
      assert( NULL != mobiles );

#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( gc_sprite_robe, &(mobiles[0].sprite) );
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
      mobiles_count++;

      player = &(mobiles[0]);
      assert( NULL != player );

#ifndef DISABLE_GRAPHICS
      graphics_load_bitmap( gc_sprite_princess, &(mobiles[1].sprite) );
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
      mobiles_count++;

      initialized = 1;
   }

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

         g_input_blocked_countdown = INPUT_BLOCK_DELAY;

         assert( 0 <= g_screen_scroll_x );
         assert( 0 <= g_screen_scroll_tx );

         topdown_refresh_tiles( tiles_flags );
#ifdef ANIMATE_SCREEN_MOVEMENT
#ifndef DISABLE_GRAPHICS
         tilemap_draw( &g_map_field, tiles_flags,
            TILEMAP_TW, TILEMAP_TH,
            g_screen_scroll_x, g_screen_scroll_y, 1 );

         graphics_flip();
#endif /* !DISABLE_GRAPHICS */
#endif /* ANIMATE_SCREEN_MOVEMENT */

         /* Drain input. */
         in_char = input_poll();

         if(
            g_screen_scroll_y == g_screen_scroll_y_tgt &&
            g_screen_scroll_x == g_screen_scroll_x_tgt
         ) {
            /* Screen scroll finished. */
            player->coords_prev.x = player->coords.x;
            player->coords_prev.y = player->coords.y;
            player->steps_x = 0;
            player->steps_y = 0;

         }

         return 1;
      }

#ifndef DISABLE_GRAPHICS
      tilemap_draw( &g_map_field, tiles_flags,
         TILEMAP_TW, TILEMAP_TH,
         g_screen_scroll_x, g_screen_scroll_y, 0 );
#endif /* !DISABLE_GRAPHICS */

      for( i = 0 ; mobiles_count > i ; i++ ) {
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
            assert( NULL != player );
#endif /* !DISABLE_GRAPHICS */
      }
   }

   window_draw_all();

   graphics_flip();

   if( !g_window_shown ) {
#ifndef HIDE_WELCOME_DIALOG
      w = window_push();
      w->pattern = &(gc_patterns[0]);
      w->w = 80;
      w->h = 64;
      w->dirty = 1;
      w->state = WINDOW_STATE_VISIBLE;
      w->strings[0] = "Welcome!";
      w->strings_count = 1;
      w->strings_color = GRAPHICS_COLOR_BLACK;
#endif /* !HIDE_WELCOME_DIALOG */

      g_window_shown = 1;

   }

   assert( mobiles[1].coords_prev.x == 5 );
   assert( mobiles[1].coords_prev.y == 5 );

   if( g_input_blocked_countdown ) {
      g_input_blocked_countdown--;
   } else {
      in_char = input_poll();
   }

   switch( in_char ) {
   case INPUT_KEY_UP:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &g_map_field, player->coords.x, player->coords.y - 1 )
      ) {
         mobile_walk_start( player, 0, -1 );
      }
      break;

   case INPUT_KEY_LEFT:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &g_map_field, player->coords.x - 1, player->coords.y )
      ) {
         mobile_walk_start( player, -1, 0 );
      }
      break;

   case INPUT_KEY_DOWN:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &g_map_field, player->coords.x, player->coords.y + 1 )
      ) {
         mobile_walk_start( player, 0, 1 );
      }
      break;

   case INPUT_KEY_RIGHT:
      if( 0 < windows_visible() ) { break; }
      if( !tilemap_collide(
         &g_map_field, player->coords.x + 1, player->coords.y )
      ) {
         mobile_walk_start( player, 1, 0 );
      }
      break;

   case INPUT_KEY_OK:
      window_pop();
      topdown_refresh_tiles( tiles_flags );
      break;

   case INPUT_KEY_QUIT:
#ifndef DISABLE_GRAPHICS
      graphics_unload_bitmap( &(g_map_field.tileset[0]) );
      graphics_unload_bitmap( &(g_map_field.tileset[1]) );
      graphics_unload_bitmap( &(g_map_field.tileset[2]) );
      graphics_unload_bitmap( &(mobiles[0].sprite) );
      graphics_unload_bitmap( &(mobiles[1].sprite) );
      memory_free( &mobiles );
      memory_free( &tiles_flags );
#endif /* !DISABLE_GRAPHICS */
      return 0;
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

   for( i = 0 ; mobiles_count > i ; i++ ) {
      mobile_animate( &(mobiles[i]), tiles_flags, TILEMAP_TW, TILEMAP_TH );
   }

   /* Scroll the screen by one if the player goes off-screen. */
   assert( NULL != player );
   if( player->coords.x >= g_screen_scroll_tx + SCREEN_TW ) {
      g_screen_scroll_x_tgt = g_screen_scroll_x + SCREEN_W;
   } else if( player->coords.y >= g_screen_scroll_y + SCREEN_TH ) {
      g_screen_scroll_y_tgt = g_screen_scroll_y + SCREEN_H;
   } else if( player->coords.x < g_screen_scroll_tx ) {
      g_screen_scroll_x_tgt = g_screen_scroll_x - SCREEN_W;
   } else if( player->coords.y < g_screen_scroll_ty ) {
      g_screen_scroll_y_tgt = g_screen_scroll_y - SCREEN_H;
   }

   graphics_loop_end();

   return 1;
}

