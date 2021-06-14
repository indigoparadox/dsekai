
#include "graphics.h"

#include <string.h>
#include <i86.h>

static union REGS g_regs;
static uint8_t huge buffer[76800];

void graphics_init() {
	g_regs.h.ah = 0;
	g_regs.h.al = 0x13;
	int86( 0x10, &g_regs, &g_regs );
}

void graphics_flip() {
	char far* screen = (char far*)0xA0000000L;

   _fmemcpy( screen, buffer, SCREEN_W * SCREEN_H );
}

void graphics_draw_px( uint16_t x, uint16_t y, uint8_t color ) {
	int offset = 0;
	offset = (y * SCREEN_W) + x;
	buffer[offset] = color;
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h, uint8_t color
) {
	int x = 0;
	int y = 0;

	for( x = x_orig ; x_orig + w > x ; x++ ) {
		for( y = y_orig ; y_orig + h > y ; y++ ) {
			graphics_draw_px( x, y, color );
		}
	}
}

void graphics_sprite_at(
   char spr[SPRITE_H], uint16_t x_orig, uint16_t y_orig, uint8_t color
) {
	int x = 0;
	int y = 0;
	int bitmask = 0;
	int pixel = 0;

	for( y = 0 ; SPRITE_H > y ; y++ ) {
		bitmask = spr[y];
		for( x = 0 ; SPRITE_W > x ; x++ ) {
			if( bitmask & 0x01 ) {
				pixel = color;
			} else {
				pixel = 0;
			}
			graphics_draw_px( x_orig + x, y_orig + y, pixel );
			bitmask >>= 1;
		}
	}
}

void graphics_char_at(
   char c, uint16_t x_orig, uint16_t y_orig, uint8_t color
) {
	int x = 0;
	int y = 0;
	int bitmask = 0;
	int pixel = 0;

	for( y = 0 ; FONT_H > y ; y++ ) {
		bitmask = font8x8_basic[c][y];
		for( x = 0 ; FONT_W > x ; x++ ) {
			if( bitmask & 0x01 ) {
				pixel = color;
			} else {
				pixel = 0;
			}
			graphics_draw_px( x_orig + x, y_orig + y, pixel );
			bitmask >>= 1;
		}
	}
}

void graphics_string_at(
   char* s, uint16_t slen, uint16_t x_orig, uint16_t y_orig, uint8_t color
) {
   uint16_t i = 0,
      x_o = 0; /* X offset. */

   while(
      i < slen &&
      '\0' != s[i] &&
      x_orig + FONT_W < SCREEN_W && /* On-screen (x-axis). */
      y_orig + FONT_H < SCREEN_H    /* On-screen (y-axis). */
   ) {
      graphics_char_at( s[i], x_orig + x_o, y_orig, color );
      x_o += FONT_W + FONT_SPACE;
      i++;
   }
}

