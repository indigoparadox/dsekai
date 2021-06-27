
#include "window.h"

#include <string.h>

#include "graphics.h"

#define MASKS_C
#ifdef ANCIENT_C
#include "../gen/mrtl.h"
#include "../gen/mrtr.h"
#include "../gen/mrbl.h"
#include "../gen/mrbr.h"
#else
#include "../gen/mask_rounded_tl.h"
#include "../gen/mask_rounded_tr.h"
#include "../gen/mask_rounded_bl.h"
#include "../gen/mask_rounded_br.h"
#endif /* ANCIENT_C */

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

            /* TODO: Define window definitions in JSON, then compile them to
            headers that can be attached to window definitions and called from
            here. */
            
            if( x_min == x && y_min == y ) {
               /* Top Left */
               graphics_blit_masked_at(
                  &(g_windows[i].pattern), mask_rounded_tl, mask_rounded_tl_sz,
                  0, 0, x, y );

            } else if( x_max - PATTERN_W == x && y_min == y ) {
               /* Top Right */
               graphics_blit_masked_at(
                  &(g_windows[i].pattern), mask_rounded_tr, mask_rounded_tr_sz,
                  0, 0, x, y );

            } else if( x_min == x && y_max - PATTERN_H == y ) {
               /* Bottom Left */
               graphics_blit_masked_at(
                  &(g_windows[i].pattern), mask_rounded_bl, mask_rounded_bl_sz,
                  0, 0, x, y );
            
            } else if( x_max - PATTERN_W == x && y_max - PATTERN_H == y ) {
               /* Bottom Right */
               graphics_blit_masked_at(
                  &(g_windows[i].pattern), mask_rounded_br, mask_rounded_br_sz,
                  0, 0, x, y );
            
            } else {
               graphics_blit_at( &(g_windows[i].pattern), x, y,
                  PATTERN_W, PATTERN_H );
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

