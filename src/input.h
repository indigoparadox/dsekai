
#ifndef INPUT_H
#define INPUT_H

#include "dstypes.h"

#ifdef PLATFORM_DOS
#include "input/dosi.h"
#elif defined( PLATFORM_SDL )
#include "input/sdli.h"
#elif defined( PLATFORM_PALM )
#include "input/palmi.h"
#elif defined( PLATFORM_WIN16 ) || defined( PLATFORM_WINCE )
#include "input/win16i.h"
#elif defined( PLATFORM_MAC7 )
#include "input/mac7i.h"
#elif defined( PLATFORM_NULL )
#include "input/nulli.h"
#endif /*
PLATFORM_DOS, PLATFORM_SDL, PLATFORM_PALM, PLATFORM_WIN16, PLATFORM_NULL */

int input_init();
int input_poll();

#endif /* INPUT_H */

