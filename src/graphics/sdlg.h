
#ifndef SDLG_H
#define SDLG_H

#include <SDL.h>

#include "../graphics.h"

typedef const SDL_Color* GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   uint32_t id;
   uint8_t initialized;
   uint16_t ref_count;
   SDL_Surface* surface;
   SDL_Texture* texture;
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

typedef struct GRAPHICS_PATTERN {
   PATTERN_TYPE bits[PATTERN_H]; } GRAPHICS_PATTERN;

#define GRAPHICS_COLOR_BLACK     &gc_black
#define GRAPHICS_COLOR_CYAN      &gc_cyan
#define GRAPHICS_COLOR_MAGENTA   &gc_magenta
#define GRAPHICS_COLOR_WHITE     &gc_white

#endif /* SDLG_H */

