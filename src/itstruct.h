
#ifndef ITSTRUCT_H
#define ITSTRUCT_H

/**
 * \addtogroup dsekai_items Items
 *
 * \{
 */

/*! \file itstruct.h
 *  \brief Structs pertaining to inventory objects.
 */

/*! \brief The maximum characters in an item name. */
#define ITEM_NAME_SZ 8

/**
 * An instance of an item in the world.
 */
struct PACKED ITEM {
   RESOURCE_ID sprite;
   /*! \brief Meaningful name of this tiem. */
   char name[ITEM_NAME_SZ];
   /*! \brief Engine-defined functionality of this item. */
   uint8_t type;
   /**
    * \brief Index of the mobile holding this item in DSEKAI_STATE::mobiles.
    *        Part of item_true_gid().
    *
    * If the owner is ::ITEM_OWNER_NONE, this item has no owner and is sitting
    * free on the map.
    *
    * If the owner is ::ITEM_OWNER_PLAYER, this item is in the player's
    * inventory.
    *
    * If the owner is ::ITEM_OWNER_META, this item has no owner, but is not
    * visible on the map (e.g. it is waiting to be handed out by a script).
    *
    * Items without ::MOBILE owners are garbage-collected when the map changes.
    * See engines_change_map() for more information.
    */
   int8_t owner;
   /**
    * \brief Arbitrary portion of the GID of the item. Part of item_true_gid().
    */
   int16_t gid;
   /*! \brief Qualitative value specific to each ITEM::type (e.g. weap str). */
   uint8_t data; 
   /*! \brief This struct represents this many of this item in a "stack." */
   uint8_t count;
   /*! \brief If ITEM::owner is ::ITEM_OWNER_NONE, the coords of this item on
    *         the ::TILEMAP. */
   /* struct TILEMAP_COORDS coords; */
};

/*! \} */

#endif /* !ITSTRUCT_H */

