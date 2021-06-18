
#include <stdio.h>
#include <string.h>

#include "data/sprites.h"
#include "data/maps.h"
#include "data/patterns.h"
#include "graphics.h"
#include "input.h"
#include "mobile.h"
#include "window.h"

int main( int argc, char* argv[] ) {
   uint32_t i = 0, j = 0, x = 0, y = 0;
   uint8_t running = 1,
      in_char = 0,
      window_shown = 0,
      walk_offset = 0;
   uint8_t tiles_flags[TILEMAP_TH][TILEMAP_TW];
   struct WINDOW* w = NULL;
   struct MOBILE player = {
      &gc_sprite_robe,
      100,
      100,
      {3, 4},
      {3, 4},
      SPRITE_W
   };
   struct MOBILE mobiles[MOBILES_MAX];
   int mobiles_count = 0;

   graphics_init();
   window_init();

   memset( &tiles_flags, 0x01, TILEMAP_TH * TILEMAP_TW );
   memset( mobiles, 0x0, sizeof( struct MOBILE ) * MOBILES_MAX );

   mobiles[0].sprite = &gc_sprite_princess;
   mobiles[0].hp = 100;
   mobiles[0].mp = 100;
   mobiles[0].coords.x = 5;
   mobiles[0].coords.y = 5;
   mobiles[0].coords_prev.x = 5;
   mobiles[0].coords_prev.y = 5;
   mobiles[0].steps = SPRITE_W;
   mobiles_count++;

   while( running ) {
      graphics_loop_start();

      in_char = 0;

      if( 0 >= windows_visible() ) {
         tilemap_draw( &gc_map_field, &tiles_flags );
         mobile_draw( &player, walk_offset );
         for( i = 0 ; mobiles_count > i ; i++ ) {
            mobile_draw( &(mobiles[i]), walk_offset );
         }
      }

      window_draw_all();

      graphics_flip();

      if( !window_shown ) {
         w = window_push();
         w->pattern = &(gc_patterns[0]);
         w->w = 80;
         w->h = 64;
         w->dirty = 1;
         w->state = WINDOW_STATE_VISIBLE;
         w->strings[0] = "Welcome!";
         w->strings_count = 1;
         window_shown = 1;
      }

      in_char = input_poll();

		/* Quit on Q. */
      switch( in_char ) {
      case INPUT_KEY_W:
         if( 0 < windows_visible() ) { break; }
         if( !tilemap_collide(
            &gc_map_field, player.coords.x, player.coords.y - 1 )
         ) {
            mobile_walk_start( &player, 0, -1 );
         }
         break;

      case INPUT_KEY_A:
         if( 0 < windows_visible() ) { break; }
         if( !tilemap_collide(
            &gc_map_field, player.coords.x - 1, player.coords.y )
         ) {
            mobile_walk_start( &player, -1, 0 );
         }
         break;

      case INPUT_KEY_S:
         if( 0 < windows_visible() ) { break; }
         if( !tilemap_collide(
            &gc_map_field, player.coords.x, player.coords.y + 1 )
         ) {
            mobile_walk_start( &player, 0, 1 );
         }
         break;

      case INPUT_KEY_D:
         if( 0 < windows_visible() ) { break; }
         if( !tilemap_collide(
            &gc_map_field, player.coords.x + 1, player.coords.y )
         ) {
            mobile_walk_start( &player, 1, 0 );
         }
         break;

      case INPUT_KEY_Z:
         window_pop();
         for( y = 0 ; TILEMAP_TH > y ; y++ ) {
            for( x = 0 ; TILEMAP_TW > x ; x++ ) {
               tiles_flags[y][x] |= TILEMAP_TILE_FLAG_DIRTY;
            }
         }
         break;

      case INPUT_KEY_Q:
			running = 0;
         break;
		}

      /* Animate. */
      if( 10 < j ) {
         j = 0;

         if( 0 == walk_offset ) {
            walk_offset = 1;
         } else {
            walk_offset = 0;
         }

      } else {
         j++;
      }

      mobile_animate( &player, &tiles_flags );
      for( i = 0 ; mobiles_count > i ; i++ ) {
         mobile_animate( &(mobiles[i]), &tiles_flags );
      }

      graphics_loop_end();
   }

   graphics_shutdown();

   return 0;
}

