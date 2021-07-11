
#define GRAPHICS_C
#include "../dstypes.h"

#include "../win16s.h"

#include <string.h>

extern uint8_t g_running;
extern uint8_t g_last_key;
extern HINSTANCE g_instance;
extern HWND g_window;
extern MEMORY_HANDLE g_state_handle;

struct GRAPHICS_BITMAP g_screen;
volatile uint32_t g_ms;

const uint32_t gc_ms_target = 1000 / FPS;
static uint32_t g_ms_start = 0; 

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
   PAINTSTRUCT ps;
   HDC hdcScreen;
   HDC hdcBuffer;
   BITMAP srcBitmap;
   HBITMAP oldHbm;

   switch( message ) {
      case WM_CREATE:
         break;

      case WM_PAINT:

         /* Create HDC for screen. */
         hdcScreen = BeginPaint( hWnd, &ps );
         if( NULL == hdcScreen ) {
            error_printf( "screen HDC is NULL" );
            break;
         }

         if( 0 == g_screen.initialized ) {
            debug_printf( 2, "creating screen buffer..." );
            g_screen.bitmap = CreateCompatibleBitmap( hdcScreen,
               SCREEN_REAL_W, SCREEN_REAL_H );
            g_screen.initialized = 1;
         }
         if( NULL == g_screen.bitmap ) {
            error_printf( "screen buffer bitmap is NULL" );
            break;
         }

         /* Create a new HDC for buffer and select buffer into it. */
         hdcBuffer = CreateCompatibleDC( hdcScreen );
         if( NULL == hdcBuffer ) {
            error_printf( "screen buffer HDC is NULL" );
            break;
         }
         oldHbm = SelectObject( hdcBuffer, g_screen.bitmap );

         /* Load parameters of the buffer into info object (srcBitmap). */
         GetObject( g_screen.bitmap, sizeof( BITMAP ), &srcBitmap );

         debug_printf( 0, "blitting buffer bitmap (%d x %d)",
            srcBitmap.bmWidth, srcBitmap.bmHeight );

         BitBlt(
            hdcScreen,
            0, 0,
            srcBitmap.bmWidth,
            srcBitmap.bmHeight,
            hdcBuffer,
            0, 0,
            SRCCOPY
         );

         /* Return the default object into the HDC. */
         SelectObject( hdcScreen, oldHbm );

         DeleteDC( hdcBuffer );
         DeleteDC( hdcScreen );

         EndPaint( hWnd, &ps );
         break;

      case WM_ERASEBKGND:
         return 1;

      case WM_KEYDOWN:
         g_last_key = wParam;
         break;

      case WM_DESTROY:
         if( 0 != g_screen.initialized ) {
            debug_printf( 2, "destroying screen buffer..." );
            DeleteObject( g_screen.bitmap );
         }
         PostQuitMessage( 0 );
         break;

      case WM_TIMER:
         g_running = topdown_loop( g_state_handle );
         break;

      default:
         return DefWindowProc( hWnd, message, wParam, lParam );
         break;
   }

   return 0;
}

int16_t graphics_platform_init() {
   MSG msg;
   WNDCLASS wc = { 0 };

   memset( &g_screen, '\0', sizeof( struct GRAPHICS_BITMAP ) );
   memset( &wc, '\0', sizeof( WNDCLASS ) );

   wc.lpfnWndProc   = (WNDPROC)&WndProc;
   wc.hInstance     = g_instance;
   wc.hIcon         = LoadIcon( g_instance, MAKEINTRESOURCE( icon_dsekai ) );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );
   /* wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAINMENU ); */
   wc.lpszClassName = "DSekaiWindowClass";

   if( !RegisterClass( &wc ) ) {
      error_printf( "unable to register main window class" );
      return 0;
   }

   g_window = CreateWindowEx(
      0, "DSekaiWindowClass", "dsekai",
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT,
      SCREEN_REAL_W, SCREEN_REAL_H, NULL, NULL, g_instance, NULL
   );

   if( !g_window ) {
      error_printf( "unable to create main window" );
      return 0;
   }

   SetTimer( g_window, WIN_GFX_TIMER_ID, 1000 / FPS, NULL );

   return 1;
}

void graphics_platform_shutdown() {
}

void graphics_flip() {
   /*UpdateWindow( g_screen.window );*/
   if( NULL != g_window ) {
      InvalidateRect( g_window, 0, TRUE );
   }
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
   HDC hdcBuffer = NULL;
   HDC hdcSrc = NULL;
   BITMAP srcBitmap;
   HBITMAP oldHbmSrc = NULL;
   HBITMAP oldHbmBuffer = NULL;

   if( NULL == bmp || NULL == bmp->bitmap ) {
      error_printf( "blit bmp is NULL" );
      return 0;
   }

   debug_printf( 0, "blitting resource #%d to %d, %d x %d, %d...",
      bmp->id, x, y, w, h );

   /* Create HDC for the off-screen buffer to blit to. */
   hdcBuffer = CreateCompatibleDC( NULL );
   oldHbmBuffer = SelectObject( hdcBuffer, g_screen.bitmap );

   /* Create HDC for source bitmap compatible with the buffer. */
   hdcSrc = CreateCompatibleDC( NULL );
   oldHbmSrc = SelectObject( hdcSrc, bmp->bitmap );

   GetObject( bmp->bitmap, sizeof( BITMAP ), &srcBitmap );

   StretchBlt(
      hdcBuffer,
      x * SCREEN_SCALE, y * SCREEN_SCALE,
      srcBitmap.bmWidth * SCREEN_SCALE,
      srcBitmap.bmHeight * SCREEN_SCALE,
      hdcSrc,
      0, 0,
      srcBitmap.bmWidth,
      srcBitmap.bmHeight,
      SRCCOPY
   );

   /* Reselect the initial objects into the provided DCs. */
   SelectObject( hdcSrc, oldHbmSrc );
   SelectObject( hdcBuffer, oldHbmBuffer );

   DeleteDC( hdcSrc );
   DeleteDC( hdcBuffer );

   return 1;
}

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
   HDC hdcBuffer = NULL;
   HBITMAP oldHbmBuffer = NULL;
   RECT rect;
   HBRUSH brush = NULL;

   rect.left = x_orig;
   rect.top = y_orig;
   rect.right = x_orig + w;
   rect.bottom = y_orig + h;

   /* Create HDC for the off-screen buffer to blit to. */
   hdcBuffer = CreateCompatibleDC( NULL );
   if( NULL == hdcBuffer ) {
      error_printf( "screen buffer HDC is NULL" );
      return;
   }
   oldHbmBuffer = SelectObject( hdcBuffer, g_screen.bitmap );

   brush = CreateSolidBrush( color );
   if( NULL == brush ) {
      error_printf( "brush is NULL" );
      return;
   }

   FillRect( hdcBuffer, &rect, brush );

   DeleteObject( brush );

   /* Reselect the initial objects into the provided DCs. */
   SelectObject( hdcBuffer, oldHbmBuffer );
   DeleteDC( hdcBuffer );
}

void graphics_draw_rect(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   uint16_t thickness, const GRAPHICS_COLOR color
) {
   HDC hdcBuffer = NULL;
   HBITMAP oldHbmBuffer = NULL;
   HPEN pen = NULL;
   HPEN oldPen = NULL;
   POINT points[5];

#ifndef PLATFORM_WINCE /* TODO */
   /* Create HDC for the off-screen buffer to blit to. */
   hdcBuffer = CreateCompatibleDC( NULL );
   if( NULL == hdcBuffer ) {
      error_printf( "screen buffer HDC is NULL" );
      return;
   }
   oldHbmBuffer = SelectObject( hdcBuffer, g_screen.bitmap );

   pen = CreatePen( PS_SOLID, thickness * SCREEN_SCALE, color );
   if( NULL == pen ) {
      error_printf( "pen is NULL" );
      return;
   }
   oldPen = SelectObject( hdcBuffer, pen );
   /* MoveTo( hdcBuffer, x1, y1 );
   LineTo( hdcBuffer, x2, y2 );*/
   points[0].x = SCREEN_SCALE * (x_orig);
   points[0].y = SCREEN_SCALE * (y_orig);
   
   points[1].x = SCREEN_SCALE * (x_orig);
   points[1].y = SCREEN_SCALE * (y_orig + h);

   points[2].x = SCREEN_SCALE * (x_orig + w);
   points[2].y = SCREEN_SCALE * (y_orig + h);
   
   points[3].x = SCREEN_SCALE * (x_orig + w);
   points[3].y = SCREEN_SCALE * (y_orig);

   points[4].x = SCREEN_SCALE * (x_orig);
   points[4].y = SCREEN_SCALE * (y_orig);
   Polyline( hdcBuffer, points, 5 );
   SelectObject( hdcBuffer, oldPen );
   DeleteObject( pen );

   /* Reselect the initial objects into the provided DCs. */
   SelectObject( hdcBuffer, oldHbmBuffer );
   DeleteDC( hdcBuffer );
#endif /* PLATFORM_WINCE */
}

void graphics_draw_line(
   uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
   uint16_t thickness, const GRAPHICS_COLOR color
) {
   HDC hdcBuffer = NULL;
   HBITMAP oldHbmBuffer = NULL;
   HPEN pen = NULL;
   HPEN oldPen = NULL;
   POINT points[2];

#ifndef PLATFORM_WINCE /* TODO */
   /* Create HDC for the off-screen buffer to blit to. */
   hdcBuffer = CreateCompatibleDC( NULL );
   oldHbmBuffer = SelectObject( hdcBuffer, g_screen.bitmap );

   pen = CreatePen( PS_SOLID, thickness, color );
   if( NULL == pen ) {
      error_printf( "pen is NULL" );
      return;
   }
   oldPen = SelectObject( hdcBuffer, pen );
   /* MoveTo( hdcBuffer, x1, y1 );
   LineTo( hdcBuffer, x2, y2 );*/
   points[0].x = x1;
   points[0].y = y1;
   points[1].x = x2;
   points[1].y = y2;
   Polyline( hdcBuffer, points, 2 );
   SelectObject( hdcBuffer, oldPen );
   DeleteObject( pen );

   /* Reselect the initial objects into the provided DCs. */
   SelectObject( hdcBuffer, oldHbmBuffer );
   DeleteDC( hdcBuffer );
#endif /* PLATFORM_WINCE */
}
/*
 * @return 1 if bitmap is loaded and 0 otherwise.
 */
int32_t graphics_load_bitmap( uint32_t id_in, struct GRAPHICS_BITMAP* b ) {
   uint8_t* buffer = NULL;
   int32_t buffer_sz = 0;
   uint32_t id = 0;
   HBITMAP res_handle = NULL;

   assert( NULL != b );
   assert( 0 == b->ref_count );

   if( 0 < id_in ) {
      id = id_in;
   } else {
      id = b->id;
   }

   /* Load resource into bitmap. */
   res_handle = resource_get_bitmap_handle( id );
   if( NULL != res_handle ) {
      /* TODO: Handle non-Windows resources. */
      b->bitmap = res_handle;
      free( res_handle );
   } else {
      error_printf( "NULL handle returned" );
   }
   if( !b->bitmap ) {
      error_printf( "unable to load resource %u", id );
      return 0;
   }

   b->id = id_in;
   b->ref_count++;
   b->initialized = 1;

   return 1;
}

/*
 * @return 1 if bitmap is unloaded and 0 otherwise.
 */
int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   if( NULL == b ) {
      return 0;
   }
   b->ref_count--;
   if( 0 >= b->ref_count ) {
      debug_printf( 2, "unloading bitmap resource %d", b->id );
      b->initialized = 0;
      DeleteObject( b->bitmap );
      return 1;
   }
   return 0;
}

