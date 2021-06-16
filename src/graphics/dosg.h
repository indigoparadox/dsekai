
#ifndef DOSG_H
#define DOSG_H

typedef uint8_t GRAPHICS_COLOR;

#include "../dstypes.h"
#include "../graphics.h"

typedef struct GRAPHICS_SPRITE { uint32_t bits[SPRITE_H]; } GRAPHICS_SPRITE;
typedef struct GRAPHICS_TILE { uint32_t bits[TILE_H]; } GRAPHICS_TILE;

#include "../mobile.h"

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* DOSG_H */

