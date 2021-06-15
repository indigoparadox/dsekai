
#include <stdio.h>
#include "data/mobs.h"
#include "data/maps.h"
#include "graphics.h"
#include "input.h"

#ifdef __DOS__
#include "input/dosi.h"
#else
#include "input/sdli.h"
#endif /* __DOS__ */

int main( int argc, char* argv[] ) {
   uint8_t running = 1,
      in_char = 0,
      walk_offset = 0;
   uint32_t i = 0, x = 10, j = 0;

   graphics_init( GRAPHICS_MODE_320_200_4_CGA );
   //graphics_init( GRAPHICS_MODE_320_200_256_VGA );

   while( running ) {
      in_char = 0;

      tilemap_draw( &gc_map_room );

      graphics_string_at( "abc", 3, 10, 10, 1 );

		graphics_draw_block( x, 20, FONT_W, FONT_H + 1, 0 );
      if( 10 < j ) {
         j = 0;
         //x++;

         if( 0 == walk_offset ) {
            walk_offset = 1;
         } else {
            walk_offset = 0;
         }

      } else {
         j++;
      }
      graphics_sprite_at(
         (uint8_t*)&(gc_mob_sprites[0]), x, 20 + walk_offset, 2 );

      graphics_flip();

      in_char = input_poll();

		/* Quit on Q. */
		if( INPUT_KEY_Q == in_char ) {
			running = 0;
		}

      for( i = 0 ; 1000000 > i ; i++ ) {}
   }

   graphics_shutdown();

   return 0;
}

