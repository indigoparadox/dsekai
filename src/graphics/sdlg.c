
#include "../graphics.h"

#include <assert.h>
#include <SDL.h>

SDL_Surface* g_screen = NULL;

void graphics_init( uint8_t mode ) {
   if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      printf( "error initializing SDL: %s\n", SDL_GetError() );
   }

   g_screen = SDL_SetVideoMode(
      SCREEN_W, SCREEN_H, 0, SDL_SWSURFACE | SDL_DOUBLEBUF );
   assert( NULL != g_screen );
}

void graphics_shutdown() {
}

void graphics_flip() {
   SDL_Flip( g_screen );
}

void graphics_draw_px( uint16_t x, uint16_t y, uint8_t colorg ) {
   int bpp;
   uint8_t* p;
   SDL_Color scolor = {255, 255, 255};
   uint32_t color = *((uint32_t*)&scolor);

   SDL_LockSurface( g_screen );
   bpp = g_screen->format->BytesPerPixel;
   p = (uint8_t*)g_screen->pixels + y * g_screen->pitch + x * bpp;

   switch( bpp ) {
   case 1:
      *p = color;
      break;

   case 2:
      *(uint16_t*)p = color;
      break;

   case 3:
      if( SDL_BYTEORDER == SDL_BIG_ENDIAN ) {
         p[0] = (color >> 16) & 0xff;
         p[1] = (color >> 8) & 0xff;
         p[2] = color & 0xff;
      } else {
         p[0] = color & 0xff;
         p[1] = (color >> 8) & 0xff;
         p[2] = (color >> 16) & 0xff;
      }
      break;

   case 4:
      *(uint32_t*)p = color;
      break;
   }
   SDL_UnlockSurface( g_screen );
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h, uint8_t color
) {
}

