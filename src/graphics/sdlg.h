
#ifndef SDLG_H
#define SDLG_H

#include <SDL.h>

#include "../graphics.h"

typedef const SDL_Color* GRAPHICS_COLOR;

#define graphics_sprite_at( spr, x, y ) \
   graphics_blit_at( (const GRAPHICS_SPRITE*)spr, x, y, 1 )
#define graphics_tile_at( spr, x, y ) \
   graphics_blit_at( (const GRAPHICS_TILE*)spr, x, y, 0 )

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

