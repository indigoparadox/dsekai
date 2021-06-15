
#include "dosi.h"

#include <dos.h>

int input_poll() {
   if( kbhit() ) {
      return getch();
   }
   return 0;
}

