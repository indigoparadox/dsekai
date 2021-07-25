
#ifndef DSEKAI_CONFIG_H
#define DSEKAI_CONFIG_H

/* ------ */
#if defined( PLATFORM_PALM )
/* ------ */

#define USE_JSON_MAPS

/* ------ */
#endif
/* ------ */

#define SCREEN_MAP_W (SCREEN_W)
#define SCREEN_MAP_H (160 - 32)

#define WINDOW_STRINGS_MAX 5
#define WINDOW_STRING_LEN_MAX 64

#define STATUS_WINDOW_W SCREEN_W
#define STATUS_WINDOW_H 32

#endif /* !DSEKAI_CONFIG_H */

