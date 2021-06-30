
#define GRAPHICS_C
#include "../graphics.h"

#include <string.h>

extern HINSTANCE g_instance;

struct GRAPHICS_BITMAP g_screen;

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
   PAINTSTRUCT ps;
   HDC hdcScreen;
   HDC hdcBuffer;
   BITMAP srcBitmap;
   HBITMAP srcHbm;

   switch( message ) {
      case WM_CREATE:
         if( NULL == g_screen.bitmap ) {
            g_screen.bitmap = CreateBitmap(
               SCREEN_REAL_W, SCREEN_REAL_H, 1, SCREEN_BPP, NULL );
            assert( NULL != g_screen.bitmap );
            g_screen.initialized = 1;
         }
         break;

      case WM_PAINT:

         /* Create HDC for screen. */
         hdcScreen = BeginPaint( hWnd, &ps );
         hdcBuffer = CreateCompatibleDC( hdcScreen );

         /* Load into on offscreen bitmap. */
         srcHbm = SelectObject( hdcBuffer, g_screen.bitmap );
         GetObject( g_screen.bitmap, sizeof( BITMAP ), &srcBitmap );

         BitBlt(
            hdcScreen,
            0, 0,
            srcBitmap.bmWidth,
            srcBitmap.bmHeight,
            hdcBuffer,
            0, 0,
            SRCCOPY
         );

         SelectObject( hdcScreen, srcHbm );

         DeleteDC( hdcBuffer );

         EndPaint( hWnd, &ps );
         break;

      case WM_DESTROY:
         PostQuitMessage( 0 );
         break;

      default:
         return DefWindowProc( hWnd, message, wParam, lParam );
         break;
   }

   return 0;
}

int graphics_init() {
   HWND hWnd;
   MSG msg;
   WNDCLASS wc = { 0 };

   memset( &g_screen, '\0', sizeof( struct GRAPHICS_BITMAP ) );
   memset( &wc, '\0', sizeof( WNDCLASS ) );

   wc.lpfnWndProc   = (WNDPROC)&WndProc;
   wc.hInstance     = g_instance;
   /* wc.hIcon         = LoadIcon( g_hInstance, MAKEINTRESOURCE( IDI_APPICON ) ); */
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );
   /* wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAINMENU ); */
   wc.lpszClassName = "DSekaiWindowClass";

   if( !RegisterClass( &wc ) ) {
      error_printf( "unable to register main window class" );
      return 0;
   }

   return 1;
}

void graphics_shutdown() {
}

void graphics_flip() {
}

void graphics_loop_start() {
}

void graphics_loop_end() {
}

/*
 * @return 1 if blit was successful and 0 otherwise.
 */
int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   HDC hdcScreen = NULL;
   HDC hdcSrc = NULL;
   BITMAP srcBitmap;
   HBITMAP srcHbm = NULL;

   if( NULL == bmp || NULL == bmp->bitmap ) {
      return 0;
   }

   debug_printf( 1, "blitting resource #%d to %d, %d x %d, %d...",
      bmp->id, x, y, w, h );

   /* Create HDC for source bitmap compatible with the screen. */
   hdcSrc = CreateCompatibleDC( NULL );
   srcHbm = SelectObject( hdcSrc, bmp->bitmap );

   GetObject( bmp->bitmap, sizeof( BITMAP ), &srcBitmap );

   BitBlt(
      hdcScreen, 0, 0, srcBitmap.bmWidth, srcBitmap.bmHeight, hdcSrc, 0, 0,
      SRCCOPY
   );

   SelectObject( hdcScreen, srcHbm );

   DeleteDC( hdcSrc );
   DeleteDC( hdcScreen );

   return 1;
}

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
}

/*
 * @return 1 if bitmap is loaded and 0 otherwise.
 */
int32_t graphics_load_bitmap( uint32_t id_in, struct GRAPHICS_BITMAP* b ) {
}

/*
 * @return 1 if bitmap is unloaded and 0 otherwise.
 */
int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
}

