
#ifndef MAC7G_H
#define MAC7G_H

#include <Quickdraw.h>
#include <MacMemory.h>
#include <Sound.h>
#include <Events.h>
#include <Fonts.h>
#include <NumberFormatting.h>

#include "../graphics.h"

typedef int GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   uint32_t id;
   uint8_t initialized;
   uint16_t ref_count;

   /* Unused Overrides */
   void* ptr1;
   void* ptr2;
   uint32_t res1;
   uint32_t res2;
   uint8_t res3;
   uint8_t res4;
};

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* MAC7G_H */

