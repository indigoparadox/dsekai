
#include "graphics.h"

#include <string.h>
#include <i86.h>
#include <dos.h>
#include <conio.h>

static uint8_t huge g_buffer[76800];

typedef void (__interrupt __far* INTFUNCPTR)( void );
INTFUNCPTR g_old_timer_interrupt;
volatile long int g_ms;

void __interrupt __far graphics_timer_handler() {
   static unsigned long count = 0;

   ++g_ms;
   count += 1103; /* Original DOS timer in parallel. */
   if( 65536 <= count ) {
      /* Call the original handler. */
      count -= 65536;
      _chain_intr( g_old_timer_interrupt );
   } else {
      /* Acknowledge interrupt */
      outp( 0x20, 0x20 );
   }
}

static void graphics_install_timer() {
   union REGS r;
   struct SREGS s;

   _disable();

   /* Backup original handler for later. */
   segread( &s );
   r.h.al = 0x08;
   r.h.ah = 0x35;
   int86x( 0x21, &r, &r, &s );
   g_old_timer_interrupt = (INTFUNCPTR)MK_FP( s.es, r.x.bx );

   /* Install new interrupt handler. */
   g_ms = 0;
   r.h.al = 0x08;
   r.h.ah = 0x25;
   s.ds = FP_SEG( graphics_timer_handler );
   r.x.dx = FP_OFF( graphics_timer_handler );
   int86x( 0x21, &r, &r, &s );

   /* Set resolution of timer chip to 1ms. */
   outp( 0x43, 0x36 );
   outp( 0x40, (uint8_t)(1103 & 0xff) );
   outp( 0x40, (uint8_t)((1103 >> 8) & 0xff) );

   _enable();
}

static void graphics_remove_timer() {
   union REGS r;
   struct SREGS s;

   /* Re-install original interrupt handler. */
   _disable();
   segread( &s );
   r.h.al = 0x08;
   r.h.ah = 0x25;
   s.ds = FP_SEG( g_old_timer_interrupt );
   r.x.dx = FP_OFF( g_old_timer_interrupt );
   int86x( 0x21, &r, &r, &s );

   /* Reset timer chip resolution to 18.2...ms. */
   outp( 0x43,0x36 );
   outp( 0x40,0x00 );
   outp( 0x40,0x00 );

   _enable();
}

void graphics_init() {
   union REGS r;
	r.h.ah = 0;
	r.h.al = 0x13;
	int86( 0x10, &r, &r );
   graphics_install_timer();
}

void graphics_shutdown() {
   graphics_remove_timer();
}

void graphics_flip() {
	char far* screen = (char far*)0xA0000000L;

   _fmemcpy( screen, g_buffer, SCREEN_W * SCREEN_H );
}

void graphics_draw_px( uint16_t x, uint16_t y, uint8_t color ) {
	int offset = 0;
	offset = (y * SCREEN_W) + x;
	g_buffer[offset] = color;
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

