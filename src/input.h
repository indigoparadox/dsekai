
#ifndef INPUT_H
#define INPUT_H

#ifdef USE_DOS
#include "input/dosi.h"
#elif defined( USE_SDL )
#include "input/sdli.h"
#endif /* USE_DOS, USE_SDL */

int input_poll();

#endif /* INPUT_H */

