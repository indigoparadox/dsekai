
#include "dosi.h"

int input_poll() {
   if( kbhit() ) {
      return getch();
   }
   return 0;
}

