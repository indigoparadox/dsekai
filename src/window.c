
#include "window.h"

#include <string.h>
#include <assert.h>

#include "graphics.h"

static struct WINDOW g_windows[WINDOW_COUNT_MAX];
static uint8_t g_windows_count = 0;

void window_init() {
   memset( g_windows, '\0', sizeof( struct WINDOW ) * WINDOW_COUNT_MAX );
}

uint8_t windows_visible() {
   return g_windows_count;
}

void window_draw_all() {
   int i = 0, x = 0, y = 0, x_max = 0, y_max = 0;

   for( i = 0 ; g_windows_count > i ; i++ ) {
      if(
         WINDOW_STATE_VISIBLE != g_windows[i].state ||
         0 == g_windows[i].dirty
      ) {
         continue;
      }

      assert( 0 == g_windows[i].w % PATTERN_W );
      assert( 0 == g_windows[i].h % PATTERN_H );
      x_max = (SCREEN_W / 2) + (g_windows[i].w / 2) ; 
      y_max = (SCREEN_H / 2) + (g_windows[i].h / 2) ; 

      for(
         y = (SCREEN_H / 2) - (g_windows[i].h / 2);
         y < y_max;
         y += PATTERN_H
      ) {
         for(
            x = (SCREEN_W / 2) - (g_windows[i].w / 2);
            x < x_max;
            x += PATTERN_W
         ) {
            graphics_pattern_at( g_windows[i].pattern, x, y );
         }

         g_windows[i].dirty = 0;
      }
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

