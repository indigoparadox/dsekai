
#ifndef CROP_H
#define CROP_H

/**
 * \addtogroup dsekai_crops Crops
 *
 * \{
 */

/*! \file crops.h
 *  \brief Structs, functions, and macros pertaining to crops.
 */

#define CROP_DEF_FLAG_ACTIVE 0x01
#define CROP_DEF_FLAG_REGROWS 0x02

/**
 * \brief CROP_PLOT::flags indicating plot is active (tilled).
 */
#define CROP_FLAG_ACTIVE 0x04

/**
 * \brief CROP_PLOT::flags mask of current crop growth stage.
 *
 * Stages are as follows:
 *
 * 0. Seed (if CROP_PLOT::crop_name[0] != 0)
 * 1. Bud
 * 2. Intermediate
 * 3. Mature / Ready for Harvest
 */
#define CROP_FLAG_STAGE_MASK 0x03

#define CROP_STAGE_MAX 3

#define CROP_ERROR_DEF_NOT_FOUND -1
#define CROP_ERROR_PLOT_NOT_FOUND -2

struct CROP_PLOT {
   char map_name[TILEMAP_NAME_MAX];
   /**
    * \brief Compared to GIDs in TILEMAP::crop_defs to find crop details.
    */
   uint8_t crop_gid;
   uint8_t flags;
   uint16_t cycle;
   uint32_t next_at_ticks;
   struct TILEMAP_COORDS coords;
};

void crop_grow( struct DSEKAI_STATE* state, struct CROP_PLOT* plot );
void crop_grow_all( struct DSEKAI_STATE* state );
int8_t crop_plant(
   struct DSEKAI_STATE* state, uint8_t crop_gid, uint8_t x, uint8_t y );
int8_t crop_get_def_idx( struct DSEKAI_STATE* state, uint8_t gid );

/*! \} */

#endif /* !CROP_H */

