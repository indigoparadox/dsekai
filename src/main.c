
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
   uint8_t running = 1,
      in_char = 0,
      window_shown = 0,
      walk_offset = 0;
   uint32_t i = 0, x = 10, j = 0;
   uint8_t tiles_flags[TILEMAP_TH][TILEMAP_TW];
   struct WINDOW* w = NULL;
   struct MOBILE player = {
      &gc_sprite_player,
      100,
      100,
      {3, 4},
      {3, 4},
      SPRITE_W
   };

   graphics_init();
   window_init();

   memset( &tiles_flags, 0x01, TILEMAP_TH * TILEMAP_TW );

   while( running ) {
      graphics_loop_start();

      in_char = 0;

      if( 0 >= windows_visible() ) {
         tilemap_draw( &gc_map_field, &tiles_flags );

         mobile_draw( &player, walk_offset );
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
         window_shown = 1;
      }

      in_char = input_poll();

		/* Quit on Q. */
      switch( in_char ) {
      case INPUT_KEY_W:
         if( !tilemap_collide(
            &gc_map_field, player.coords.x, player.coords.y - 1 )
         ) {
            mobile_walk_start( &player, 0, -1 );
         }
         break;

      case INPUT_KEY_A:
         if( !tilemap_collide(
            &gc_map_field, player.coords.x - 1, player.coords.y )
         ) {
            mobile_walk_start( &player, -1, 0 );
         }
         break;

      case INPUT_KEY_S:
         if( !tilemap_collide(
            &gc_map_field, player.coords.x, player.coords.y + 1 )
         ) {
            mobile_walk_start( &player, 0, 1 );
         }
         break;

      case INPUT_KEY_D:
         if( !tilemap_collide(
            &gc_map_field, player.coords.x + 1, player.coords.y )
         ) {
            mobile_walk_start( &player, 1, 0 );
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

      graphics_loop_end();
   }

   graphics_shutdown();

   return 0;
}

