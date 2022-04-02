
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

struct CROP_PLOT {
   char map_name[TILEMAP_NAME_MAX];
   /**
    * \brief System name for currently growing crop, used to index
    *        TILEMAP::crop_defs.
    */
   char crop_name[CROP_NAME_MAX];
   uint8_t flags;
   uint32_t next_at_ticks;
   struct TILEMAP_COORDS coords;
};

/*! \} */

#endif /* !CROP_H */

