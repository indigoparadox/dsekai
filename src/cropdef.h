
#ifndef CROPDEF_H
#define CROPDEF_H

/**
 * \addtogroup dsekai_crops
 *
 * \{
 */

/*! \file crops.h
 *  \brief Crop definition struct to be included in a ::TILEMAP.
 */

/**
 * \addtogroup dsekai_crops_def_flags Crop Definition Flags
 *
 * \brief Flags which affect all crops that share a ::CROP_DEF universally.
 *
 * \{
 */

/**
 * \brief CROP_DEF::flags indicating crop definition is populated.
 */
#define CROP_DEF_FLAG_ACTIVE 0x01

/**
 * \brief CROP_DEF::flags indicating crop regrows when harvested.
 */
#define CROP_DEF_FLAG_REGROWS 0x02

/*! \} */

/**
 * \brief Definition of a crop to be stored with the ::TILEMAP on which the 
 *        crop may be grown.
 */
struct CROP_DEF {
   /*! \brief Sprite sheet for this crop as it's growing. */
   RESOURCE_NAME sprite_name;
   int16_t sprite_cache_id;
   char name[CROP_NAME_MAX];
   /*! \brief System identifier for this crop. Unique among all ::CROP_DEF. */
   uint8_t gid;
   /*! \brief See \ref dsekai_crops_def_flags for more information. */
   uint8_t flags;
   /*! \brief Nominal number of ms until crop reaches next growth stage. */
   uint16_t cycle;
   /**
    * \brief ::ITEM::gid of the item to be given when this crop is harvested.
    */
   int16_t produce_gid;
};

#endif /* !CROPDEF_H */

