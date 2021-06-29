
#include "dosi.h"

#include <dos.h>
#include <conio.h>

int input_init() {
   return 1;
}

int input_poll() {
   if( kbhit() ) {
      return getch();
   }
   return 0;
}

