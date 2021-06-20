
#include <string.h>
#include <PalmOS.h>

#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"
#include "psprintf.h"

#define MAIN_C
#include "data.h"

#ifndef NDEBUG
char g_assert_failed[256];
int g_assert_failed_len;
#endif /* !NDEBUG */

#define PSPRINTF_BUF_LEN 255

UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags ) {
   uint32_t i = 0, j = 0, x = 0, y = 0;
   uint8_t running = 1,
      window_shown = 0;
   uint8_t tiles_flags[TILEMAP_TH][TILEMAP_TW];
   struct MOBILE player = {
      &gc_sprite_robe,
      100,
      100,
      {3, 4},
      {3, 4},
      0,
      0,
      NULL
   };
   struct MOBILE mobiles[MOBILES_MAX];
   int mobiles_count = 0;
#ifndef NO_PALM_DEBUG_LINE
   char palm_debug_line[PSPRINTF_BUF_LEN + 1];
   int palm_debug_line_len = 0;

   memset( palm_debug_line, '\0', PSPRINTF_BUF_LEN + 1 );
#endif /* !NO_PALM_DEBUG_LINE */

   if( cmd == sysAppLaunchCmdNormalLaunch ) {

      graphics_init();
      window_init();

      memset( &tiles_flags, 0x01, TILEMAP_TH * TILEMAP_TW );
      memset( mobiles, 0x0, sizeof( struct MOBILE ) * MOBILES_MAX );
#ifndef NDEBUG
      memset( &g_assert_failed, '\0', 256 );
#endif /* NDEBUG */

      while( running ) {

         running = 
            topdown_loop( &player, mobiles, &mobiles_count, &tiles_flags );

#ifndef NO_PALM_DEBUG_LINE
         /*palm_debug_line_len = psprintf( palm_debug_line, PSITOA_BUF_LEN,
            "%d, %d (%d, %d)",
            player.coords.x,
            player.coords.y,
            player.coords_prev.x,
            player.coords_prev.y );*/
         //WinDrawChars( palm_debug_line, palm_debug_line_len, 10, 10 );
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

