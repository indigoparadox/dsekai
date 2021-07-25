
#ifndef DOSG_H
#define DOSG_H

#define GRAPHICS_M_320_200_4_CGA  0x05
#define GRAPHICS_M_320_200_256_V  0x13

#define GRAPHICS_M_320_200_256_VGA_A   0xA0000000L
#define GRAPHICS_M_320_200_4_CGA_A     0xB8000000L

#ifndef GRAPHICS_MODE
#define GRAPHICS_MODE      0x05
#endif /* !GRAPHICS_MODE */

#ifndef CGA_COLD
#define CGA_COLD           0x01
#endif /* CGA_COLD */

#if GRAPHICS_M_320_200_4_CGA == GRAPHICS_MODE
#define GRAPHICS_ADDR     GRAPHICS_M_320_200_4_CGA_A
#elif GRAPHICS_M_320_200_256_VGA == GRAPHICS_MODE
#define GRAPHICS_ADDR     GRAPHICS_M_320_200_256_VGA_A
#endif /* GRAPHICS_MODE */

typedef uint8_t GRAPHICS_COLOR;

struct GRAPHICS_BITMAP {
   RESOURCE_ID id;
   uint8_t initialized;
   uint16_t ref_count;

   uint8_t* plane_1;
   uint8_t* plane_2;
   uint32_t w;
   uint32_t h;
   uint8_t palette;

   /* Unused Overrides */
   uint8_t res4;
};

struct GRAPHICS_ARGS {
   uint8_t mode;
};

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* DOSG_H */

