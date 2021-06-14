
#include <stdio.h>
#include "mobs.h"
#include "graphics.h"

int main( int argc, char* argv[] ) {
   uint8_t running = 1,
      in_char = 0,
      walk_offset = 0;
   uint32_t i = 0;

   graphics_init();

   while( running ) {
      in_char = 0;

      graphics_string_at( "abc", 3, 10, 10, 1 );

		graphics_draw_block( 10, 20, FONT_W, FONT_H + 1, 0 );
      graphics_sprite_at( &(mob_sprites[0]), 10, 20 + walk_offset, 2 );

      graphics_flip();

      if( 0 == walk_offset ) {
         walk_offset = 1;
      } else {
         walk_offset = 0;
      }

		if( kbhit() ) {
			in_char = getch();
		}

		/* Quit on Q. */
		if( 113 == in_char ) {
			running = 0;
		}

      for( i = 0 ; 1000000 > i ; i++ ) {}
   }

   return 0;
}

