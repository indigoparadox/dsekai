
#include "../graphics.h"

#include <string.h>
#include <i86.h>
#include <dos.h>
#include <conio.h>

#ifdef USE_LOOKUPS
#include "../data/offsets.h"
#endif /* USE_LOOKUPS */

static uint8_t g_mode = 0;
static uint8_t huge g_buffer[76800]; /* Sized for 0x13. */
static uint8_t huge *g_buffer_p2 = NULL;

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

void graphics_init( uint8_t mode ) {
   union REGS r;

	r.h.ah = 0;
	r.h.al = mode;
	int86( 0x10, &r, &r );
   graphics_install_timer();

   g_mode = mode;
   if( GRAPHICS_MODE_320_200_4_CGA == mode ) {
      g_buffer_p2 = &(g_buffer[8192]);
   }
}

void graphics_shutdown() {
   graphics_remove_timer();
}

void graphics_flip() {
   if( GRAPHICS_MODE_320_200_256_VGA == g_mode ) {
      _fmemcpy( (char far *)GRAPHICS_MODE_320_200_256_VGA_ADDR,
         g_buffer, SCREEN_W * SCREEN_H );
   } else if( GRAPHICS_MODE_320_200_4_CGA == g_mode ) {
      /* memcpy both planes. */
      _fmemcpy( (char far *)0xB8000000, g_buffer, 16000 );
   }
}

void graphics_draw_px( uint16_t x, uint16_t y, GRAPHICS_COLOR color ) {
	int byte_offset = 0,
      bit_offset = 0,
      bit_mask = 0;
   uint16_t scaled_x = x,
      scaled_y = y;

   if( GRAPHICS_MODE_320_200_256_VGA == g_mode ) {
      byte_offset = ((y * SCREEN_W) + x);
      g_buffer[byte_offset] = color;
   } else if( GRAPHICS_MODE_320_200_4_CGA == g_mode ) {
#ifdef USE_LOOKUPS
      /* Use pre-generated lookup tables for offsets to improve performance. */
      byte_offset = gc_offsets_cga_bytes_p1[scaled_y][scaled_x];
      bit_offset = gc_offsets_cga_bits_p1[scaled_y][scaled_x];
#else
      /* Divide y by 2 since both planes are SCREEN_H / 2 high. */
      /* Divide result by 4 since it's 2 bits per pixel. */
      byte_offset = (((scaled_y / 2) * SCREEN_W) + scaled_x) / 4;
      /* Shift the bits over by the remainder. */
      bit_offset = 
         6 - (((((scaled_y / 2) * SCREEN_W) + scaled_x) % 4) * 2);
#endif /* USE_LOOKUPS */

      /* Clear the existing pixel. */
      if( 1 == scaled_y % 2 ) {
         g_buffer[0x2000 + byte_offset] &= ~(0x03 << bit_offset);
         g_buffer[0x2000 + byte_offset] |= (color << bit_offset);
      } else {
         g_buffer[byte_offset] &= ~(0x03 << bit_offset);
         g_buffer[byte_offset] |= (color << bit_offset);
      }
   }

}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   GRAPHICS_COLOR color
) {
	int x = 0;
	int y = 0;

	for( x = x_orig ; x_orig + w > x ; x++ ) {
		for( y = y_orig ; y_orig + h > y ; y++ ) {
			graphics_draw_px( x, y, color );
		}
	}
}

