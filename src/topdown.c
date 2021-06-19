
//#include <stdio.h>
#include <string.h>

#include "data/sprites.h"
#include "data/maps.h"
#include "data/patterns.h"
#include "graphics.h"
#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"

int g_semi_cycles = 0;
int g_walk_offset = 0;

int loop_topdown(
   struct MOBILE* player,
   struct MOBILE mobiles[MOBILES_MAX], int* mobiles_count,
   uint8_t (*tiles_flags)[TILEMAP_TH][TILEMAP_TW]
) {
   uint32_t i = 0, x = 0, y = 0;
   uint8_t in_char = 0;
   static uint8_t window_shown = 0;
   struct WINDOW* w = NULL;

   graphics_loop_start();

   in_char = 0;

   if( 0 == windows_visible() ) {
      tilemap_draw( &gc_map_field, tiles_flags );
      mobile_draw( player, g_walk_offset );
      for( i = 0 ; *mobiles_count > i ; i++ ) {
         mobile_draw( &(mobiles[i]), g_walk_offset );
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
      w->strings_color = GRAPHICS_COLOR_BLACK;
      window_shown = 1;
   }

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
      for( y = 0 ; TILEMAP_TH > y ; y++ ) {
         for( x = 0 ; TILEMAP_TW > x ; x++ ) {
            (*tiles_flags)[y][x] |= TILEMAP_TILE_FLAG_DIRTY;
         }
      }
      break;

   case INPUT_KEY_Q:
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

   mobile_animate( player, tiles_flags );
   for( i = 0 ; *mobiles_count > i ; i++ ) {
      mobile_animate( &(mobiles[i]), tiles_flags );
   }

   graphics_loop_end();


   return 1;
}

