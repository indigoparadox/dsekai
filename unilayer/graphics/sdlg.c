
#define GRAPHICS_C
#define SDLG_C
#include "../unilayer.h"

#include <SDL.h>

SDL_Window* g_window = NULL;
SDL_Surface* g_screen = NULL;
SDL_Renderer* g_renderer = NULL;

volatile uint32_t g_ms;

const uint32_t gc_ms_target = 1000 / FPS;
static uint32_t g_ms_start = 0; 

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int16_t graphics_platform_init( struct GRAPHICS_ARGS* args ) {
#ifdef DEBUG_CGA_EMU
   SDL_Rect area;
#endif /* DEBUG_CGA_EMU */

   if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      error_printf( "error initializing SDL: %s", SDL_GetError() );
   }

   /*SDL_CreateWindowAndRenderer(
      SCREEN_REAL_W, SCREEN_REAL_H, 0, &g_window, &g_renderer );*/
   g_window = SDL_CreateWindow( "dsekai",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_REAL_W, SCREEN_REAL_H, 0 );
   if( NULL ) {
      return 0;
   }
   g_screen = SDL_GetWindowSurface( g_window );
   if( NULL == g_screen ) {
      return 0;
   }
   g_renderer = SDL_CreateSoftwareRenderer( g_screen );
   if( NULL == g_renderer ) {
      return 0;
   }

#ifdef DEBUG_CGA_EMU
   area.x = 0;
   area.y = 0;
   area.w = SCREEN_REAL_W;
   area.h = SCREEN_REAL_H;
   SDL_SetRenderDrawColor( g_renderer,  0, 0, 0, 255 );
   SDL_RenderFillRect( g_renderer, &area );
#endif /* DEBUG_CGA_EMU */

   return 1;
}

void graphics_platform_shutdown( struct GRAPHICS_ARGS* args ) {
   SDL_DestroyWindow( g_window );
   SDL_Quit();
}

void graphics_flip( struct GRAPHICS_ARGS* args ) {
   /* SDL_RenderPresent( g_renderer ); */
   SDL_UpdateWindowSurface( g_window );
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
   int x_s = 0, y_s = 0;
   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   x *= SCREEN_SCALE;
   y *= SCREEN_SCALE;
   for( y_s = 0 ; SCREEN_SCALE > y_s ; y_s++ ) {
      for( x_s = 0 ; SCREEN_SCALE > x_s ; x_s++ ) {
         SDL_RenderDrawPoint( g_renderer, x + x_s, y + y_s );
      }
   }
}

/*
 * @return 1 if blit was successful and 0 otherwise.
 */
int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   SDL_Rect dest_rect = {
      x * SCREEN_SCALE, 
      y * SCREEN_SCALE,
      w * SCREEN_SCALE, 
      h * SCREEN_SCALE};

   if( NULL == bmp || NULL == bmp->texture ) {
      error_printf( "NULL bitmap passed" );
      return 0;
   }

   debug_printf( 0, "blitting resource #%d to %d, %d x %d, %d...",
      bmp->id, x, y, w, h );
   SDL_RenderCopy( g_renderer, bmp->texture, NULL, &dest_rect );

   return 1;
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
   SDL_Rect area;

   area.x = x_orig * SCREEN_SCALE;
   area.y = y_orig * SCREEN_SCALE;
   area.w = w * SCREEN_SCALE;
   area.h = h * SCREEN_SCALE;

   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderFillRect( g_renderer, &area );
}

void graphics_draw_rect(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   uint16_t thickness, const GRAPHICS_COLOR color
) {
   SDL_Rect area;

   area.x = x_orig * SCREEN_SCALE;
   area.y = y_orig * SCREEN_SCALE;
   area.w = w * SCREEN_SCALE;
   area.h = h * SCREEN_SCALE;

   /* TODO: Handle thickness. */

   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderDrawRect( g_renderer, &area );
}

void graphics_draw_line(
   uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t thickness,
   const GRAPHICS_COLOR color
) {
   SDL_SetRenderDrawColor( g_renderer, color->r, color->g, color->b, 255 );
   /* TODO: Handle thickness. */
   SDL_RenderDrawLine( g_renderer, x1, y1, x2, y2 );
}

/**
 * @return 1 if bitmap is loaded and 0 otherwise.
 */
int16_t graphics_platform_load_bitmap(
   RESOURCE_BITMAP_HANDLE res_handle, struct GRAPHICS_BITMAP* b
) {
   uint8_t* buffer = NULL;
   int16_t retval = 1;
   uint32_t buffer_sz = 0;
   SDL_RWops* bmp_stream;

   assert( NULL != b );
   assert( 0 == b->ref_count );

   buffer_sz = memory_sz( res_handle );
   buffer = resource_lock_handle( res_handle );

   /* Parse buffered resource into SDL. */
   bmp_stream = SDL_RWFromMem( buffer, buffer_sz );
   b->surface = SDL_LoadBMP_RW( bmp_stream, 1 ); /* Free stream on close. */
   if( NULL == b->surface ) {
      error_printf( "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = 0;
      goto cleanup;
   }
   debug_printf( 1, "SDL loaded surface for bitmap" );
   b->texture = SDL_CreateTextureFromSurface( g_renderer, b->surface );
   if( NULL == b->texture ) {
      error_printf( "SDL unable to create texture: %s", SDL_GetError() );
      retval = 0;
      if( NULL != b->surface ) {
         SDL_FreeSurface( b->surface );
         b->surface = NULL;
      }
      goto cleanup;
   }
   debug_printf( 1, "SDL created texture for bitmap" );

cleanup:

   if( NULL != buffer ) {
      buffer = resource_unlock_handle( res_handle );
   }

   if( NULL != res_handle ) {
      resource_free_handle( res_handle );
   }

   return retval;
}

/*
 * @return 1 if bitmap is unloaded and 0 otherwise.
 */
int16_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   if( NULL == b ) {
      return 0;
   }
   b->ref_count--;
   if( 0 >= b->ref_count ) {
      debug_printf( 2, "unloading texture and surface for bitmap resource %d",
         b->id );
      SDL_DestroyTexture( b->texture );
      SDL_FreeSurface( b->surface );
      b->initialized = 0;
      return 1;
   }
   return 0;
}

