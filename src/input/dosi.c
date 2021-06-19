
#include "dosi.h"

#include <dos.h>
#include <conio.h>

int input_poll() {
   if( kbhit() ) {
      return getch();
   }
   return 0;
}

