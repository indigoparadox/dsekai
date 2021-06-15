
#define SDLG_C
#include "../graphics.h"

#include <assert.h>
#include <SDL.h>

SDL_Window* g_window = NULL;
SDL_Surface* g_screen = NULL;
SDL_Renderer* g_renderer = NULL;

void graphics_init( uint8_t mode ) {
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

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderDrawPoint( g_renderer, x, y );
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
   SDL_Rect area;

   area.x = x_orig;
   area.y = y_orig;
   area.w = w;
   area.h = h;

   SDL_FillRect( g_screen, &area, 0x0 );
}

