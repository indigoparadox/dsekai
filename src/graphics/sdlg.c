
#define SDLG_C
#include "../graphics.h"

#include <assert.h>
#include <SDL.h>
#include <assert.h>

SDL_Window* g_window = NULL;
SDL_Surface* g_screen = NULL;
SDL_Renderer* g_renderer = NULL;

volatile uint32_t g_ms;

const uint32_t gc_ms_target = 1000 / FPS;
static uint32_t g_ms_start = 0; 

void graphics_init() {
#ifdef DEBUG_CGA_EMU
   SDL_Rect area;
#endif /* DEBUG_CGA_EMU */

if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      printf( "error initializing SDL: %s\n", SDL_GetError() );
   }

   SDL_CreateWindowAndRenderer(
      SCREEN_W * 2, SCREEN_H * 2, 0, &g_window, &g_renderer );
   assert( NULL != g_window );
   g_screen = SDL_GetWindowSurface( g_window );

#ifdef DEBUG_CGA_EMU
   area.x = 0;
   area.y = 0;
   area.w = SCREEN_W * 2;
   area.h = SCREEN_H * 2;
   SDL_SetRenderDrawColor( g_renderer,  0, 0, 0, 255 );
   SDL_RenderFillRect( g_renderer, &area );
#endif /* DEBUG_CGA_EMU */
}

void graphics_shutdown() {
   SDL_DestroyWindow( g_window );
   SDL_Quit();
}

void graphics_flip() {
   SDL_RenderPresent( g_renderer );
}

void graphics_loop_start() {
   g_ms_start = SDL_GetTicks();
}

void graphics_loop_end() {
   int16_t delta = 0;
   
   do {
      delta = gc_ms_target - (SDL_GetTicks() - g_ms_start);
   } while( 0 < delta );  
}

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   x *= 2;
   y *= 2;
   SDL_RenderDrawPoint( g_renderer, x, y );
   SDL_RenderDrawPoint( g_renderer, x + 1, y );
   SDL_RenderDrawPoint( g_renderer, x, y + 1 );
   SDL_RenderDrawPoint( g_renderer, x + 1, y + 1 );
}

/*
void graphics_sprite_at( const GRAPHICS_SPRITE* spr, uint16_t x, uint16_t y ) {
   int x_offset = 0,
      y_offset = 0;
   uint32_t bitmask_spr = 0;
   GRAPHICS_COLOR pixel = GRAPHICS_COLOR_BLACK;

   for( y_offset = 0 ; TILE_H > y_offset ; y_offset++ ) {
      bitmask_spr = spr->bits[y_offset];
      for( x_offset = 0 ; TILE_W * 2 > x_offset ; x_offset++ ) {
         if( bitmask_spr & 0x01 ) {
            pixel = GRAPHICS_COLOR_CYAN;
         } else if( bitmask_spr & 0x02 ) {
            pixel = GRAPHICS_COLOR_MAGENTA;
         } else if( bitmask_spr & 0x03 ) {
            pixel = GRAPHICS_COLOR_WHITE;
         } else {
            pixel = GRAPHICS_COLOR_BLACK;
         }
         graphics_draw_px( x + x_offset, y + y_offset, pixel );
         bitmask_spr >>= 2;
      }
   }
}
*/

#ifdef DEBUG_CGA_EMU
extern GRAPHICS_TILE gc_tiles_field[TILEMAP_TILESETS_MAX][TILE_W];
#endif /* DEBUG_CGA_EMU */

void graphics_tile_at( const GRAPHICS_TILE* spr, uint16_t x, uint16_t y ) {
   //int x_offset = 0,
   int y_offset = 0,
      byte_offset = 0,
      bit_offset = 0,
      x_scr_offset = 0,
      raw_byte = 0,
      masked_byte = 0;
   GRAPHICS_COLOR pixel = GRAPHICS_COLOR_BLACK;

#ifdef DEBUG_CGA_EMU
   char drawn_bytes[5] = { 0, 0, 0, 0, 0 };
   /*if( &(gc_tiles_field[2][0]) != spr ) {
      return;
   }*/
#endif /* DEBUG_CGA_EMU */

   for( y_offset = 0 ; TILE_H > y_offset ; y_offset++ ) {
      x_scr_offset = 0;
#ifdef DEBUG_CGA_EMU
      printf( "line %02d (0x%08x):\n", y_offset, spr->bits[y_offset] );
#endif /* DEBUG_CGA_EMU */

      /* Start at the last byte and move backwards. */
      for( byte_offset = 24 ; 0 <= byte_offset ; byte_offset -= 8 ) {
         /* Get full line and shift it to the current byte. */
         raw_byte = (spr->bits[y_offset] >> byte_offset) & 0xff;

#ifdef DEBUG_CGA_EMU
         printf( " by%02d: 0x%02x:\n", byte_offset / 8, raw_byte );

         if( 0x0b == raw_byte ) {
            printf( "0x%02x\n", (raw_byte >> 2) & 0x02 );
            /* assert( (masked_byte >> 2) & 0x02 == 0x02 ); */
         }
#endif /* DEBUG_CGA_EMU */

         /* Iterate through the line 2 bits at a time. */
         for( bit_offset = 0 ; 8 > bit_offset ; bit_offset += 2 ) {
            masked_byte = raw_byte & 0x03;
            if( 0x01 == masked_byte ) {
               pixel = GRAPHICS_COLOR_CYAN;
#ifdef DEBUG_CGA_EMU
               drawn_bytes[bit_offset / 2] = 'c';
#endif /* DEBUG_CGA_EMU */
               graphics_draw_px(
                  x + x_scr_offset, y + y_offset, GRAPHICS_COLOR_CYAN );
            } else if( 0x02 == masked_byte ) {
               pixel = GRAPHICS_COLOR_MAGENTA;
#ifdef DEBUG_CGA_EMU
               drawn_bytes[bit_offset / 2] = 'm';
#endif /* DEBUG_CGA_EMU */
               graphics_draw_px(
                  x + x_scr_offset, y + y_offset, GRAPHICS_COLOR_MAGENTA );
            } else if( 0x03 == masked_byte ) {
               pixel = GRAPHICS_COLOR_WHITE;
#ifdef DEBUG_CGA_EMU
               drawn_bytes[bit_offset / 2] = 'w';
#endif /* DEBUG_CGA_EMU */
               graphics_draw_px(
                  x + x_scr_offset, y + y_offset, GRAPHICS_COLOR_WHITE );
            } else if( 0x00 == masked_byte ) {
               pixel = GRAPHICS_COLOR_BLACK;
#ifdef DEBUG_CGA_EMU
               drawn_bytes[bit_offset / 2] = 'b';
#endif /* DEBUG_CGA_EMU */
               graphics_draw_px(
                  x + x_scr_offset, y + y_offset, GRAPHICS_COLOR_BLACK );
            } else {
#ifdef DEBUG_CGA_EMU
               drawn_bytes[bit_offset / 2] = '.';
               printf( "\nERROR: 0x%x\n", masked_byte );
#endif /* DEBUG_CGA_EMU */
            }

#ifdef DEBUG_CGA_EMU
            printf( "  bi%d(x%x):(%c%c%c%c%c%c%c%c)",
               bit_offset, x_scr_offset,
               raw_byte & 0x80 ? '1' : '0',
               raw_byte & 0x40 ? '1' : '0',
               raw_byte & 0x20 ? '1' : '0',
               raw_byte & 0x10 ? '1' : '0',
               raw_byte & 0x08 ? '1' : '0',
               raw_byte & 0x04 ? '1' : '0',
               raw_byte & 0x02 ? '1' : '0',
               raw_byte & 0x01 ? '1' : '0');
#endif /* DEBUG_CGA_EMU */

            raw_byte >>= 2;
            x_scr_offset++;
         }

#ifdef DEBUG_CGA_EMU
         printf( "\n  drew %c %c %c %c @ %d, %d\n---\n",
            drawn_bytes[0], drawn_bytes[1], drawn_bytes[2], drawn_bytes[3],
            x_scr_offset - 4, y_offset );
#endif /* DEBUG_CGA_EMU */
      }
      assert( TILE_W == x_scr_offset );
   }
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
   SDL_Rect area;

#ifdef SCALE_2X
   area.x = x_orig * 2;
   area.y = y_orig * 2;
   area.w = w * 2;
   area.h = h * 2;
#else
   area.x = x_orig;
   area.y = y_orig;
   area.w = w;
   area.h = h;
#endif /* SCALE_2X */

   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderFillRect( g_renderer, &area );
}

