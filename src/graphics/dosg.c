
#include "../graphics.h"

#include <string.h>
#include <i86.h>
#include <dos.h>
#include <conio.h>

#ifdef USE_LOOKUPS
#include "../data/offsets.h"
#endif /* USE_LOOKUPS */

//static uint8_t g_mode = 0;
#ifdef USE_DOUBLEBUF
static uint8_t huge g_buffer[76800]; /* Sized for 0x13. */
#else
static uint8_t far* g_buffer = (uint8_t far *)GRAPHICS_ADDR;
#endif /* USE_DOUBLEBUF */

typedef void (__interrupt __far* INTFUNCPTR)( void );
INTFUNCPTR g_old_timer_interrupt;
volatile uint16_t g_ms;

const uint16_t gc_ms_target = 1000 / FPS;
static uint16_t g_ms_start = 0; 

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
	r.h.al = GRAPHICS_MODE;
	int86( 0x10, &r, &r );
   graphics_install_timer();

   //g_mode = mode;
}

void graphics_shutdown() {
   graphics_remove_timer();
}

void graphics_flip() {
#ifdef USE_DOUBLEBUF
   /*if( GRAPHICS_MODE_320_200_256_VGA == g_mode ) {
      _fmemcpy( (char far *)GRAPHICS_MODE_320_200_256_VGA_ADDR,
         g_buffer, SCREEN_W * SCREEN_H );
   } else if( GRAPHICS_MODE_320_200_4_CGA == g_mode ) { */
      /* memcpy both planes. */
      _fmemcpy( (char far *)0xB8000000, g_buffer, 16000 );
   //}
#endif /* USE_DOUBLEBUF */
}

void graphics_loop_start() {
   g_ms_start = g_ms;
}

void graphics_loop_end() {
   int16_t delta = 0;
   
   do {
      delta = gc_ms_target - (g_ms - g_ms_start);
   } while( 0 < delta );  
}

void graphics_draw_px( uint16_t x, uint16_t y, GRAPHICS_COLOR color ) {
	int byte_offset = 0,
      bit_offset = 0,
      bit_mask = 0;
   uint16_t scaled_x = x,
      scaled_y = y;

   //if( GRAPHICS_MODE_320_200_256_VGA == g_mode ) {
   //   byte_offset = ((y * SCREEN_W) + x);
   //   g_buffer[byte_offset] = color;
   //} else if( GRAPHICS_MODE_320_200_4_CGA == g_mode ) {
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
   //}
}

void graphics_sprite_at( const GRAPHICS_SPRITE* spr, uint16_t x, uint16_t y ) {
	int y_offset = 0;
   uint16_t byte_offset = 0;
   uint8_t y_is_odd = /* Interlacing compensation do % once to save cycles. */
      0 == y % 2 ? 0 : 1;

	for( y_offset = 0 ; SPRITE_H > y_offset ; y_offset++ ) {
      byte_offset = gc_offsets_cga_bytes_p1[y + y_offset][x];
      switch( y_offset + y_is_odd ) {
      case 0x0:
      case 0x2:
      case 0x4:
      case 0x6:
      case 0x8:
      case 0xa:
      case 0xc:
      case 0xe:
         _fmemcpy( &(g_buffer[byte_offset]), &(spr->bits[y_offset]),
            SPRITE_BYTES );
         break;

      case 0x1:
      case 0x3:
      case 0x5:
      case 0x7:
      case 0x9:
      case 0xb:
      case 0xd:
      case 0xf:
         _fmemcpy(
            &(g_buffer[0x2000 + byte_offset]), &(spr->bits[y_offset]),
               SPRITE_BYTES );
         break;
      }
	}
}

void graphics_tile_at( const GRAPHICS_TILE* til, uint16_t x, uint16_t y ) {
	int y_offset = 0;
   uint16_t byte_offset = 0;
   uint8_t y_is_odd = /* Interlacing compensation do % once to save cycles. */
      0 == y % 2 ? 0 : 1;

	for( y_offset = 0 ; TILE_H > y_offset ; y_offset++ ) {
      byte_offset = gc_offsets_cga_bytes_p1[y + y_offset][x];
      switch( y_offset + y_is_odd ) {
      case 0x0:
      case 0x2:
      case 0x4:
      case 0x6:
      case 0x8:
      case 0xa:
      case 0xc:
      case 0xe:
         _fmemcpy( &(g_buffer[byte_offset]), &(til->bits[y_offset]),
            TILE_BYTES );
         break;

      case 0x1:
      case 0x3:
      case 0x5:
      case 0x7:
      case 0x9:
      case 0xb:
      case 0xd:
      case 0xf:
         _fmemcpy(
            &(g_buffer[0x2000 + byte_offset]), &(til->bits[y_offset]),
               TILE_BYTES );
         break;
      }
	}
}


void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   GRAPHICS_COLOR color
) {
	int x = 0;
	int y = 0;
   uint16_t byte_offset = 0;

   for( y = y_orig ; y < y + h ; y++ ) {
      byte_offset = gc_offsets_cga_bytes_p1[y][x_orig];
      _fmemset( (char far *)0xB8000000 + byte_offset, color, 2 );
      _fmemset( (char far *)0xB8002000 + byte_offset, color, 2 );
   }
}

