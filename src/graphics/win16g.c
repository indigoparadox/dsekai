
#define GRAPHICS_C
#include "../graphics.h"

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
   PAINTSTRUCT ps;
   HDC hdcScreen;
   HDC hdcBuffer;
   BITMAP srcBitmap;
   HBITMAP srcHbm;

   switch( message )
   {
      case WM_PAINT:
         hdcScreen = BeginPaint( hWnd, &ps );
         hdcBuffer = CreateCompatibleDC( hdcScreen );
         srcHbm = SelectObject( hdcBuffer, g_screen->surface );

         GetObject( g_screen->surface, sizeof( BITMAP ), &srcBitmap );

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

   g_instance = hInstance;

   if( hPrevInstance ) {
      error_printf( "previous instance detected" );
      return 1;
   }

   wc.lpfnWndProc   = &MainWndProc;
   wc.hInstance     = g_hInstance;
   wc.hIcon         = LoadIcon( g_hInstance, MAKEINTRESOURCE( IDI_APPICON ) );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );
   wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAINMENU );
   wc.lpszClassName = MainWndClass;

   if( !RegisterClass( &wc ) ) {
      error_printf( "unable to register main window class" );
      return 1;
   }
}

#if 0
void graphics_screen_new(
   GRAPHICS** g, SCAFFOLD_SIZE w, SCAFFOLD_SIZE h,
   SCAFFOLD_SIZE vw, SCAFFOLD_SIZE vh,  int32_t arg1, void* arg2
) {
   int result;
   HWND hWnd;
   INT nRetVal = 0;
   WNDCLASSEX winclass;
   HBRUSH hBrush;
   ZeroMemory( &winclass, sizeof( WNDCLASSEX ) );

   hBrush = CreateSolidBrush( RGB( 0, 0, 0 ) );

   winclass.cbSize = sizeof( WNDCLASSEX );
   winclass.hbrBackground = hBrush;
   winclass.hCursor = LoadCursor( NULL, IDC_ARROW );
   winclass.hInstance = arg2;
   winclass.lpfnWndProc = (WNDPROC)WndProc;
   winclass.lpszClassName = "IPWindowClass";
   winclass.style = CS_HREDRAW | CS_VREDRAW;

   result = RegisterClassEx( &winclass );
   if( !result ) {
      /* TODO: Display error. */
      GetLastError();
      MessageBox( NULL, "Error registering window class.", "Error", MB_ICONERROR | MB_OK );
      nRetVal = 1;
      goto cleanup;
   }

   hWnd = CreateWindowEx(
      0, "IPWindowClass", "ProIRCd",
      WS_OVERLAPPED | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
      0, 0, w, h, NULL, NULL, arg2, NULL
   );

   /* SetWindowDisplayAflfinity( hWnd, WDA_MONITOR ); */

   ShowWindow( hWnd, arg1 );

cleanup:
   return;
}

void graphics_surface_init( GRAPHICS* g, SCAFFOLD_SIZE w, SCAFFOLD_SIZE h ) {
   if( 0 < w && 0 < h) {
      g->surface =
   } else {
      g->surface = NULL;
   }
   graphics_surface_set_h( g, h );
   graphics_surface_set_w( g, w );
   g->palette = NULL;
   return;
}

void graphics_surface_cleanup( GRAPHICS* g ) {

}

void graphics_flip_screen( GRAPHICS* g ) {

}

void graphics_shutdown( GRAPHICS* g ) {

}

void graphics_set_image_path( GRAPHICS* g, const bstring path ) {

}

void graphics_set_image_data(
   GRAPHICS* g, const BYTE* data, SCAFFOLD_SIZE length
) {
   BITMAPFILEHEADER bfh = *(BITMAPFILEHEADER*)data;
   BITMAPINFOHEADER bih = *(BITMAPINFOHEADER*)(data +
                                 sizeof( BITMAPFILEHEADER ));
   RGBQUAD             rgb = *(RGBQUAD*)(data +
                                 sizeof( BITMAPFILEHEADER ) +
                                 sizeof( BITMAPINFOHEADER ));
   BITMAPINFO bi;
   const BYTE* pPixels = NULL;
   BYTE* ppvBits = NULL;

   bi.bmiColors[0] = rgb;
   bi.bmiHeader = bih;
   pPixels = (data + bfh.bfOffBits);

   if( NULL != g->surface ) {
      DeleteObject( g->surface );
      g->surface = NULL;
   }

   g->surface =
      CreateDIBSection( NULL, &bi, DIB_RGB_COLORS, (void**) &ppvBits, NULL, 0 );
   SetDIBits( NULL, g->surface, 0, bih.biHeight, pPixels, &bi, DIB_RGB_COLORS );

   /* GetObject( g->surface, sizeof( BITMAP ), NULL ); */
}

BYTE* graphics_export_image_data( GRAPHICS* g, SCAFFOLD_SIZE* out_len ) {

}

void graphics_draw_rect(
   GRAPHICS* g, SCAFFOLD_SIZE x, SCAFFOLD_SIZE y,
   SCAFFOLD_SIZE w, SCAFFOLD_SIZE h, GRAPHICS_COLOR color
) {

}

void graphics_transition( GRAPHICS* g, GRAPHICS_TRANSIT_FX fx ) {
}

void graphics_blit_partial(
   GRAPHICS* g, SCAFFOLD_SIZE x, SCAFFOLD_SIZE y,
   SCAFFOLD_SIZE s_x, SCAFFOLD_SIZE s_y,
   SCAFFOLD_SIZE s_w, SCAFFOLD_SIZE s_h, const GRAPHICS* src
) {
   HDC hdcDest = NULL;
   HDC hdcSrc = NULL;
   BITMAP srcBitmap;
   HBITMAP srcHbm = NULL;

   hdcSrc = CreateCompatibleDC( hdcDest );
   srcHbm = SelectObject( hdcSrc, src->surface );

   GetObject( src->surface, sizeof( BITMAP ), &srcBitmap );

   BitBlt(
      hdcDest, 0, 0, srcBitmap.bmWidth, srcBitmap.bmHeight, hdcSrc, 0, 0,
      SRCCOPY
   );

   SelectObject( hdcDest, srcHbm );

   DeleteDC( hdcSrc );
   DeleteDC( hdcDest );
}

void graphics_sleep( uint16_t milliseconds ) {

}

void graphics_draw_char(
   GRAPHICS* g, SCAFFOLD_SIZE_SIGNED x, SCAFFOLD_SIZE_SIGNED y,
   GRAPHICS_COLOR color, GRAPHICS_FONT_SIZE size, char c
) {
}

void graphics_set_window_title( GRAPHICS* g, bstring title, void* icon ) {
}

uint32_t graphics_get_ticks() {
}

#endif
