
#ifndef INPUT_H
#define INPUT_H

#ifdef USE_DOS
#include "input/dosi.h"
#elif defined( USE_SDL )
#include "input/sdli.h"
#elif defined( USE_PALM )
#include "input/palmi.h"
#elif defined( USE_WIN16 )
#include "input/win16i.h"
#elif defined( USE_NULL )
#include "input/nulli.h"
#endif /* USE_DOS, USE_SDL, USE_PALM, USE_WIN16, USE_NULL */

int input_poll();

#endif /* INPUT_H */

