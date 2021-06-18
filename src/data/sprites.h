
#ifndef SPRITES_H
#define SPRITES_H

#include "../dstypes.h"
#include "../mobile.h"
#include "../graphics.h"

/*
These are stored as 2bpp 16x16 pixel CGA images in little-endian order.
This makes a huge difference in performance on a real 8086.
*/

const GRAPHICS_SPRITE gc_sprite_robe = {
   {0x0, 0xff0300, 0xc0ff3f00, 0xf051fc00, 0xf051f403, 0xf055f503, 0xf055f503, 0xc055f500, 0xaf3f00, 0xc0afff00, 0xf0a3fc00, 0xf0a0f000, 0xf0f0f000, 0xc0f03000, 0xff0f00, 0x0}
};

const GRAPHICS_SPRITE gc_sprite_princess = {
   {0x0, 0xfc0f00, 0xcf3300, 0xcb3200, 0xff0f00, 0x3d0100, 0x4c0400, 0x101100, 0x4c3400, 0x9c3900, 0x646600, 0x999901, 0x666602, 0x999901, 0xa8aa00, 0x0}
};

#endif /* SPRITES_H */

