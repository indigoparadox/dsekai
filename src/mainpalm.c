
#include <string.h>
#include <PalmOS.h>

#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "data/dio.h"
#include "item.h"

#ifndef NDEBUG
char g_assert_failed[256];
int g_assert_failed_len;
#endif /* !NDEBUG */

#define PSPRINTF_BUF_LEN 255

UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags ) {
   uint32_t i = 0, j = 0, x = 0, y = 0;
   uint8_t running = 1;
#ifndef NO_PALM_DEBUG_LINE
   char palm_debug_line[PSPRINTF_BUF_LEN + 1];
   int palm_debug_line_len = 0;
   struct ITEM items[ITEMS_MAX];

   memset( palm_debug_line, '\0', PSPRINTF_BUF_LEN + 1 );
#endif /* !NO_PALM_DEBUG_LINE */

   if( cmd == sysAppLaunchCmdNormalLaunch ) {

      graphics_init();
      window_init();

#ifndef NDEBUG
      memset( &g_assert_failed, '\0', 256 );
#endif /* NDEBUG */

      while( running ) {

         /* assert( NULL != &gc_sprite_princess );
         assert( 0 != gc_sprite_princess ); */

         running = topdown_loop();

#ifndef NO_PALM_DEBUG_LINE
         palm_debug_line_len = dio_snprintf( palm_debug_line, PSITOA_BUF_LEN,
            "%d, %d (%d, %d)",
            player.coords.x,
            player.coords.y,
            player.coords_prev.x,
            player.coords_prev.y );
         WinDrawChars( palm_debug_line, palm_debug_line_len, 10, 10 );
#endif /* !NO_PALM_DEBUG_LINE */

#ifndef NDEBUG
         if( 0 < g_assert_failed_len ) {
            WinDrawChars( g_assert_failed, g_assert_failed_len, 10, 20 );
         }
#endif /* NDEBUG */
 
      }

      graphics_shutdown();
   }

   return;

}

