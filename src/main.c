
#include <stdio.h>
#include <string.h>

#include "data/sprites.h"
#include "data/maps.h"
#include "graphics.h"
#include "input.h"
#include "mobile.h"

int main( int argc, char* argv[] ) {
   uint8_t running = 1,
      in_char = 0,
      walk_offset = 0;
   uint32_t i = 0, x = 10, j = 0;
   uint8_t tiles_flags[TILEMAP_TH][TILEMAP_TW];
   struct MOBILE player = {
      &gc_sprite_player,
      3, 
      3,
      4,
      4,
      SPRITE_W
   };

   graphics_init( GRAPHICS_MODE_320_200_4_CGA );
   //graphics_init( GRAPHICS_MODE_320_200_256_VGA );

   memset( &tiles_flags, 0x01, TILEMAP_TH * TILEMAP_TW );

   while( running ) {
      graphics_loop_start();

      in_char = 0;

      tilemap_draw( &gc_map_field, &tiles_flags );

      //graphics_string_at( "abc", 3, 10, 10, GRAPHICS_COLOR_CYAN );

      /* Draw. */
      /* Note that sprite is drawn at prev_x/y + steps, NOT current x/y. */
      graphics_sprite_at(
         *player.sprite,
         (player.tx_prev * SPRITE_W) +
            (player.tx != player.tx_prev ? player.steps : 0),
         (player.ty_prev * SPRITE_H) - walk_offset +
            (player.ty != player.ty_prev ? player.steps : 0),
         GRAPHICS_COLOR_MAGENTA, 2 );

      graphics_flip();

      in_char = input_poll();

		/* Quit on Q. */
      switch( in_char ) {
      case INPUT_KEY_W:
         if( !tilemap_collide( &gc_map_field, player.tx, player.ty - 1 ) ) {
            mobile_walk_start( &player, 0, -1 );
         }
         break;

      case INPUT_KEY_A:
         if( !tilemap_collide( &gc_map_field, player.tx - 1, player.ty ) ) {
            mobile_walk_start( &player, -1, 0 );
         }
         break;

      case INPUT_KEY_S:
         if( !tilemap_collide( &gc_map_field, player.tx, player.ty + 1 ) ) {
            mobile_walk_start( &player, 0, 1 );
         }
         break;

      case INPUT_KEY_D:
         if( !tilemap_collide( &gc_map_field, player.tx + 1, player.ty ) ) {
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

