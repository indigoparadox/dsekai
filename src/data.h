
#ifndef DATA_H
#define DATA_H

#include "dstypes.h"

/* TODO: Automatically generate this mess with a script during make. */

#ifdef MAIN_C

#ifdef RESOURCE_SPRITE_HEADERS

#include "data/sprites.h"
#include "data/tilebmps.h"

#else /* !RESOURCE_SPRITE_HEADERS */

#include "palmrsc.h"
#include "../data/palm/palm_ids.h"
#include "../data/palm/palm_rc.h"

#endif /* RESOURCE_SPRITE_HEADERS */

#include "data/maps.h"
#include "data/patterns.h"

#else /* !MAIN_C */

extern const struct TILEMAP gc_map_field;
extern const GRAPHICS_PATTERN gc_patterns[];

/* This file should be auto-generated. See Makefile for details. */
#include "resext.h"

#endif /* MAIN_C */

#endif /* DATA_H */

