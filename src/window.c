
#include "window.h"

#include <string.h>
#include <assert.h>

#include "graphics.h"

#include "data/masks.h"

static struct WINDOW g_windows[WINDOW_COUNT_MAX];
static uint8_t g_windows_count = 0;

void window_init() {
   memset( g_windows, '\0', sizeof( struct WINDOW ) * WINDOW_COUNT_MAX );
}

uint8_t windows_visible() {
   return g_windows_count;
}

void window_draw_all() {
   int i = 0,
      j = 0,
      x = 0,
      y = 0,
      x_max = 0,
      y_max = 0,
      x_min = 0,
      y_min = 0;

   for( i = 0 ; g_windows_count > i ; i++ ) {
      if(
         WINDOW_STATE_VISIBLE != g_windows[i].state
#ifndef IGNORE_DIRTY
         || 0 == g_windows[i].dirty
#endif /* !IGNORE_DIRTY */
      ) {
         continue;
      }

      assert( 0 == g_windows[i].w % PATTERN_W );
      assert( 0 == g_windows[i].h % PATTERN_H );
      x_max = (SCREEN_W / 2) + (g_windows[i].w / 2); 
      y_max = (SCREEN_H / 2) + (g_windows[i].h / 2); 
      x_min = (SCREEN_W / 2) - (g_windows[i].w / 2);
      y_min = (SCREEN_H / 2) - (g_windows[i].h / 2);

      for( y = y_min ; y < y_max ; y += PATTERN_H ) {
         for( x = x_min ; x < x_max ; x += PATTERN_W ) {
            if( x_min == x && y_min == y ) {
               graphics_pattern_masked_at(
                  g_windows[i].pattern, &(gc_masks[0]), 0, 0, x, y );
            } else if( x_max - PATTERN_W == x && y_min == y ) {
               graphics_pattern_masked_at(
                  g_windows[i].pattern, &(gc_masks[1]), 0, 0, x, y );
            } else if( x_min == x && y_max - PATTERN_H == y ) {
               graphics_pattern_masked_at(
                  g_windows[i].pattern, &(gc_masks[2]), 0, 0, x, y );
            } else if( x_max - PATTERN_W == x && y_max - PATTERN_H == y ) {
               graphics_pattern_masked_at(
                  g_windows[i].pattern, &(gc_masks[3]), 0, 0, x, y );
            } else {
               graphics_pattern_at( g_windows[i].pattern, x, y );
            }
         }
      }

      for( j = 0 ; g_windows[i].strings_count > j ; j++ ) {
         graphics_string_at( g_windows[i].strings[j],
            x_min + WINDOW_TEXT_X, y_min + (WINDOW_TEXT_Y * (i + 1)),
            g_windows[i].strings_color, 1 );
      }

      g_windows[i].dirty = 0;
   }
}

struct WINDOW* window_push() {
   int i = 0;

   assert( g_windows_count + 1 < WINDOW_COUNT_MAX );

   for( i = g_windows_count ; 0 < i ; i-- ) {
      memcpy( &(g_windows[i]), &(g_windows[i - 1]), sizeof( struct WINDOW ) );
   }

   memset( &(g_windows[0]), '\0', sizeof( struct WINDOW ) );

   g_windows_count++;

   return &(g_windows[0]);
}

void window_pop() {
   int i = 0;

   for( i = 0 ; g_windows_count > i ; i++ ) {
      memcpy( &(g_windows[i]), &(g_windows[i + 1]), sizeof( struct WINDOW ) );
   }

   g_windows_count--;
}

