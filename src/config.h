
#ifndef CONFIG_H
#define CONFIG_H

#define TILEMAP_TW 20
#define TILEMAP_TH 11

#define TILEMAP_TILESETS_MAX  12

#define SCREEN_W 320
#define SCREEN_H 200
#define FONT_W 8
#define FONT_H 8
#define FONT_SPACE 1
#define SPRITE_H 16
#define SPRITE_W 16
#define SPRITE_BYTES 4
#define TILE_W 8
#define TILE_H 8
#define TILE_BYTES 1

#if 0
#define GRAPHICS_MODE_320_200_4_CGA    0x05
#define GRAPHICS_MODE_320_200_256_VGA  0x13

#define GRAPHICS_MODE_320_200_256_VGA_ADDR   0xA0000000L
#define GRAPHICS_MODE_320_200_4_CGA_ADDR     0xB8000000L
#endif

#define GRAPHICS_MODE      0x05
#define GRAPHICS_ADDR      0xB8000000L

#endif /* CONFIG_H */

