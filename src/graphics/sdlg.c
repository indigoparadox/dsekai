
#define SDLG_C
#include "../graphics.h"

#include <assert.h>
#include <SDL.h>

SDL_Window* g_window = NULL;
SDL_Surface* g_screen = NULL;
SDL_Renderer* g_renderer = NULL;

volatile uint32_t g_ms;

const uint32_t gc_ms_target = 1000 / FPS;
static uint32_t g_ms_start = 0; 

void graphics_init() {
   if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      printf( "error initializing SDL: %s\n", SDL_GetError() );
   }

   SDL_CreateWindowAndRenderer(
      SCREEN_W, SCREEN_H, 0, &g_window, &g_renderer );
   assert( NULL != g_window );
   g_screen = SDL_GetWindowSurface( g_window );
}

void graphics_shutdown() {
   SDL_DestroyWindow( g_window );
   SDL_Quit();
}

void graphics_flip() {
   SDL_RenderPresent( g_renderer );
}

void graphics_loop_start() {
   g_ms_start = SDL_GetTicks();
}

void graphics_loop_end() {
   int16_t delta = 0;
   
   do {
      delta = gc_ms_target - (SDL_GetTicks() - g_ms_start);
   } while( 0 < delta );  
}

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderDrawPoint( g_renderer, x, y );
}

void graphics_sprite_at( const GRAPHICS_SPRITE* spr, uint16_t x, uint16_t y ) {
   int x_offset = 0,
      y_offset = 0;
   uint32_t bitmask_spr = 0;
   GRAPHICS_COLOR pixel = GRAPHICS_COLOR_BLACK;

   for( y_offset = 0 ; SPRITE_H > y_offset ; y_offset++ ) {
      bitmask_spr = spr->bits[y_offset];
      for( x_offset = 0 ; SPRITE_W > x_offset ; x_offset++ ) {
         if( bitmask_spr & 0x01 ) {
            pixel = GRAPHICS_COLOR_WHITE;
         } else {
            pixel = GRAPHICS_COLOR_BLACK;
         }
         graphics_draw_px( x + x_offset, y + y_offset, pixel );
         bitmask_spr >>= 1;
      }
   }
}

void graphics_tile_at( const GRAPHICS_TILE* spr, uint16_t x, uint16_t y ) {
   int x_offset = 0,
      y_offset = 0;
   uint32_t bitmask_spr = 0;
   GRAPHICS_COLOR pixel = GRAPHICS_COLOR_BLACK;

   for( y_offset = 0 ; SPRITE_H > y_offset ; y_offset++ ) {
      bitmask_spr = spr->bits[y_offset];
      for( x_offset = 0 ; SPRITE_W > x_offset ; x_offset++ ) {
         if( bitmask_spr & 0x01 ) {
            pixel = GRAPHICS_COLOR_WHITE;
         } else {
            pixel = GRAPHICS_COLOR_BLACK;
         }
         graphics_draw_px( x + x_offset, y + y_offset, pixel );
         bitmask_spr >>= 1;
      }
   }
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
   SDL_Rect area;

#ifdef SCALE_2X
   area.x = x_orig * 2;
   area.y = y_orig * 2;
   area.w = w * 2;
   area.h = h * 2;
#else
   area.x = x_orig;
   area.y = y_orig;
   area.w = w;
   area.h = h;
#endif /* SCALE_2X */

   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderFillRect( g_renderer, &area );
}

