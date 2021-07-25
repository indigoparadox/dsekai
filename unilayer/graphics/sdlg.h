
#ifndef SDLG_H
#define SDLG_H

#include <SDL.h>

typedef const SDL_Color* GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   RESOURCE_ID id;
   uint8_t initialized;
   uint16_t ref_count;

   SDL_Surface* surface;
   SDL_Texture* texture;

   /* Unused Overrides */
   uint32_t res1;
   uint32_t res2;
   uint8_t res3;
   uint8_t res4;
};

struct GRAPHICS_ARGS {
};

#ifdef SDLG_C
const SDL_Color gc_black =    {0,   0,   0};
const SDL_Color gc_cyan =     {0,   255, 255};
const SDL_Color gc_magenta =  {255, 0,   255};
const SDL_Color gc_white =    {255, 255, 255};
#else
extern const SDL_Color gc_black;
extern const SDL_Color gc_cyan;
extern const SDL_Color gc_magenta;
extern const SDL_Color gc_white;
#endif /* SDLG_C */

#define GRAPHICS_COLOR_BLACK     &gc_black
#define GRAPHICS_COLOR_CYAN      &gc_cyan
#define GRAPHICS_COLOR_MAGENTA   &gc_magenta
#define GRAPHICS_COLOR_WHITE     &gc_white

#endif /* SDLG_H */

