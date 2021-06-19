
#ifndef INPUT_H
#define INPUT_H

#ifdef USE_DOS
#include "input/dosi.h"
#elif defined( USE_SDL )
#include "input/sdli.h"
#elif defined( USE_PALM )
#include "input/palmi.h"
#endif /* USE_DOS, USE_SDL, USE_PALM */

int input_poll();

#endif /* INPUT_H */

