
#ifndef CROPS_H
#define CROPS_H

/**
 * \addtogroup dsekai_crops Crops
 *
 * \{
 */

/*! \file crops.h
 *  \brief Structs, functions, and macros pertaining to crops.
 */

/**
 * \brief CROP_PLOT::flags indicating plot is active (tilled).
 */
#define CROP_FLAG_ACTIVE 0x04

/**
 * \brief CROP_PLOT::flags mask of current crop growth stage.
 *
 * Stages are as follows:
 *
 * 0. Seed (if CROP_PLOT::crop_index > 0)
 * 1. Bud
 * 2. Intermediate
 * 3. Mature / Ready for Harvest
 */
#define CROP_FLAG_STAGE_MASK 0x03

/**
 * \brief Table of available crops. Format is (index, name, regrowing).
 */
#define CROP_TABLE( f ) f( none, 0, 0 ) f( 1, turnip, 0 ) f( 2, tomato, 1 )

struct CROP_PLOT {
   char map_name[TILEMAP_NAME_MAX];
   uint8_t flags;
   uint8_t crop_index;
   uint32_t ticks_to_next;
   struct TILEMAP_COORDS coords;
};

/*! \} */

#endif /* !CROPS_H */

