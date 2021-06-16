
#ifndef SPRITES_H
#define SPRITES_H

#include "../dstypes.h"
#include "../mobile.h"
#include "../graphics.h"

/*
These are stored as 2bpp 16x16 pixel CGA images in little-endian order.
This makes a huge difference in performance on a real 8086.
*/

const GRAPHICS_SPRITE gc_sprite_player = {
   {0xffff00, 0xffff00, 0xf050fc0f, 0xf050fc0f, 0xf050f03f, 0xf050f03f, 0xf055f53f, 0xf055f53f, 0xafff3f, 0xafff3f, 0xf0a0f000, 0xf0a0f000, 0xf0f0f000, 0xf0f0f000, 0xff0f00, 0xff0f00}
};

#endif /* SPRITES_H */

