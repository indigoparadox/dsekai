
#include <string.h>
#include <PalmOS.h>

#include "input.h"
#include "mobile.h"
#include "window.h"
#include "engines.h"

#define MAIN_C
#include "data.h"

UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags ) {
   EventType event;
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
      SPRITE_W
   };
   struct MOBILE mobiles[MOBILES_MAX];
   int mobiles_count = 0;

   if( cmd == sysAppLaunchCmdNormalLaunch ) {

      graphics_init();
      window_init();

      memset( &tiles_flags, 0x01, TILEMAP_TH * TILEMAP_TW );
      memset( mobiles, 0x0, sizeof( struct MOBILE ) * MOBILES_MAX );

      // Main event loop:
      do {

         // Doze until an event arrives.
         EvtGetEvent( &event, evtWaitForever );

         // System gets first chance to handle the event.
         SysHandleEvent( &event );

         // Normally, we would do other 
         // event processing here.

         // Return from PilotMain when an 
         // appStopEvent is received.
         running = 
            topdown_loop( &player, mobiles, &mobiles_count, &tiles_flags );
 
      } while( event.eType != appStopEvent );

      graphics_shutdown();
   }

   return;

}

