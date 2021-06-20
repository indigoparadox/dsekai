
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

const GRAPHICS_SPRITE gc_sprite_princess = gc_sprite_princess_id;
const GRAPHICS_SPRITE gc_sprite_robe = gc_sprite_robe_id;
const GRAPHICS_TILE gc_tile_field_grass = gc_tile_field_grass_id;
const GRAPHICS_TILE gc_tile_field_brick_wall = gc_tile_field_brick_wall_id;
const GRAPHICS_TILE gc_tile_field_tree = gc_tile_field_tree_id;

#endif /* RESOURCE_SPRITE_HEADERS */

#include "data/maps.h"
#include "data/patterns.h"

#else /* !MAIN_C */

extern const struct TILEMAP gc_map_field;
extern const GRAPHICS_PATTERN gc_patterns[];
extern const GRAPHICS_SPRITE gc_sprite_princess;

#endif /* MAIN_C */

#endif /* DATA_H */

