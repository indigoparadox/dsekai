
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

/**
 * \addtogroup dsekai_crops_flags Crop Flags
 *
 * \brief Flags which affect an individual ::CROP_PLOT.
 *
 * \{
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
 * 0. Seed (if CROP_PLOT::crop_name[0] != 0)
 * 1. Bud
 * 2. Intermediate
 * 3. Mature / Ready for Harvest
 */
#define CROP_FLAG_STAGE_MASK 0x03

/*! \} */

/**
 * \brief Maximum potential crop growth stage, corresponds to
 *        ::CROP_FLAG_STAGE_MASK.
 */
#define CROP_STAGE_MAX 3

/*! \brief Return value indicating ::CROP_DEF was not found for provided GID. */
#define CROP_ERROR_DEF_NOT_FOUND -1
/**
 * \brief Return value indicating ::CROP_PLOT was not found for provided
 *        ::TILEMAP::name and ::TILEMAP_COORDS.
 */
#define CROP_ERROR_PLOT_NOT_FOUND -2

#define CROP_ERROR_PRODUCE_NOT_FOUND -3

struct CROP_PLOT {
   /**
    * \brief Name of the ::TILEMAP on which this crop is growing.
    */
   char map_name[TILEMAP_NAME_MAX];
   /**
    * \brief Compared to GIDs in TILEMAP::crop_defs to find crop details.
    */
   uint8_t crop_gid;
   /*! \brief See \ref dsekai_crops_flags for more information. */
   uint8_t flags;
   /**
    * \brief Nominal number of ms until crop reaches next growth stage.
    *
    * Intentionally duplicated from ::CROP_DEF::cycle so that crops are able
    * to continue growing even when the ::TILEMAP they exist on is not loaded.
    */
   uint16_t cycle;
   /**
    * \brief Next absolute ms at which this crop may advance to next growth
    *        stage.
    */
   uint32_t next_at_ticks;
   struct TILEMAP_COORDS coords;
};


/**
 * \brief Check the crop's current cycle/flags and grow to the next stage if
 *        able to.
 */
void crop_grow( struct CROP_PLOT* plot, struct DSEKAI_STATE* state );

/**
 * \brief Call crop_grow() on all crops active in the engine state.
 */
void crop_grow_all( struct DSEKAI_STATE* state );

struct CROP_PLOT* crop_find_plot(
   struct TILEMAP* t, uint8_t x, uint8_t y, struct DSEKAI_STATE* state );

/**
 * \brief Given a ::CROP_DEF::gid, plant on a plot at the given coordinates on
 *        the currently loaded ::TILEMAP.
 */
int8_t crop_plant(
   uint8_t crop_gid, struct CROP_PLOT* plot, struct DSEKAI_STATE* state );

int8_t crop_harvest(
   struct MOBILE* harvester, struct CROP_PLOT* plot,
   struct DSEKAI_STATE* state );

/**
 * \brief Given a ::CROP_DEF::gid, find the index in ::TILEMAP::crop_defs.
 */
int8_t crop_get_def_idx( uint8_t gid, struct DSEKAI_STATE* state );

/*! \} */

#endif /* !CROP_H */

