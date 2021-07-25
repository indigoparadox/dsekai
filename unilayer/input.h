
#ifndef INPUT_H
#define INPUT_H

#ifdef PLATFORM_DOS
#include "input/dosi.h"
#elif defined( PLATFORM_SDL )
#include "input/sdli.h"
#elif defined( PLATFORM_XLIB )
#include "input/xi.h"
#elif defined( PLATFORM_PALM )
#include "input/palmi.h"
#elif defined( PLATFORM_WIN )
#include "input/wini.h"
#elif defined( PLATFORM_MAC7 )
#include "input/mac7i.h"
#elif defined( PLATFORM_NDS )
#include "input/ndsi.h"
#elif defined( PLATFORM_GL )
#include "input/gli.h"
#elif defined( PLATFORM_NULL )
#include "input/nulli.h"
#endif /*
PLATFORM_DOS, PLATFORM_SDL, PLATFORM_PALM, PLATFORM_WIN16, PLATFORM_NULL */

int input_init();
int input_poll();

#endif /* INPUT_H */

