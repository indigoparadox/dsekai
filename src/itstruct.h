
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

/**
 * \addtogroup dsekai_items_flags Item Flags
 * \brief Flags which affect an individual ::ITEM.
 * \{
 */

/**
 * \brief ITEM::flags bit indicating item exists in the world (and is not
 *        just a free slot in DSEKAI_STATE::items.
 */
#define ITEM_FLAG_ACTIVE      0x1000
#define ITEM_FLAG_CRAFTABLE   0x2000

/*! \brief Engine-defined functionality of this item. */
#define ITEM_TYPE_MASK        0x0f00

#define item_get_type_flag( e ) (((e)->flags & ITEM_TYPE_MASK) >> 8)

#define item_set_type_flag( e, v ) (e)->flags = (((e)->flags & ~ITEM_TYPE_MASK) | (((v) << 8) & ITEM_TYPE_MASK))

#define ITEM_COUNT_MASK       0x00ff

#define item_get_count_flag( e ) ((e)->flags & ITEM_COUNT_MASK)

#define item_incr_count( e, v ) (e)->flags = (((e)->flags & ~ITEM_COUNT_MASK) | ((item_get_count_flag( e ) + (((v) & ITEM_COUNT_MASK))) & ITEM_COUNT_MASK))

/*! \} */ /* dsekai_items_flags */

/**
 * \relates ITEM_NAME
 * \brief The maximum characters in an ITEM::name.
 */
#define ITEM_NAME_SZ 8

/**
 * An instance of an item in the world.
 *
 * See ::DSEKAI_STATE for context on how items are stored.
 */
/* TODO: Consolidate some of these byte-sized fields. */
struct ITEM {
   RESOURCE_ID sprite;
   int16_t sprite_id;
   /*! \brief Meaningful name of this tiem. */
   char name[ITEM_NAME_SZ + 1]; /* +1 for NULL. */
   /**
    * \brief Index in DSEKAI_STATE::mobiles holding this item.
    *
    * If the owner is ::ITEM_OWNER_NONE, this item has no owner and is sitting
    * free on the map.
    *
    * If the owner is ::ITEM_OWNER_PLAYER, this item is in the player's
    * inventory.
    *
    * Items without ::MOBILE owners are garbage-collected when the map changes.
    * See engines_warp_loop() for more information.
    */
   int8_t owner;
   /**
    * \brief Arbitrary portion of the GID of the item.
    */
   int16_t gid;
   /**
    * \brief Qualitative value specific to each ITEM::type:
    *
    * | Item | Data        |
    * |------|-------------|
    * | Seed | Crop GID    |
    * | Food | HP+         |
    *
    */
   uint8_t data; 
   /*! \brief \ref dsekai_items_flags pertaining to this item. */
   uint16_t flags;
   /*! \brief If ITEM::owner is ::ITEM_OWNER_NONE, the X coord of this item on
    *         the ::TILEMAP. */
   uint8_t x;
   /*! \brief If ITEM::owner is ::ITEM_OWNER_NONE, the Y coord of this item on
    *         the ::TILEMAP. */
   uint8_t y;
   uint16_t map_gid;
};

/*! \} */

#endif /* !ITSTRUCT_H */

