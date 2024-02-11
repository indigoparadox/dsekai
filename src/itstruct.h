
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

/**
 * \brief ITEM::flags indicating this item is not the last used item slot.
 *
 * This is an optimization measure: The allocation always sets this bit,
 * dying doesn't unset it, reallocation can use item slots with this bit set,
 * and iteration loops can break early when they no longer see it.
 */
#define ITEM_FLAG_NOT_LAST    0x4000

/**
 * \brief Bitmask for ITEM::flags indicating that item's
 *        \ref dsekai_items_use.
 */
#define ITEM_TYPE_MASK        0x0f00

/**
 * \brief Bitmask for ITEM::flags indicating the number of copies of this
 *        item as a stack.
 */
#define ITEM_COUNT_MASK       0x00ff

/*! \} */ /* dsekai_items_flags */

/**
 * \relates ITEM
 * \brief Get the \ref dsekai_items_use for ::ITEM e, e.g. to index in
 *        ::gc_items_max.
 */
#define item_get_type_flag( e ) (((e)->flags & ITEM_TYPE_MASK) >> 8)

/**
 * \relates ITEM
 * \brief Set the \ref dsekai_items_use for ::ITEM e.
 */
#define item_set_type_flag( e, v ) (e)->flags = (((e)->flags & ~ITEM_TYPE_MASK) | (((v) << 8) & ITEM_TYPE_MASK))

/**
 * \relates ITEM
 * \brief Return the number of copies of ::ITEM e as a stack.
 */
#define item_get_count_flag( e ) ((e)->flags & ITEM_COUNT_MASK)

/**
 * \relates ITEM
 * \brief Increment the number of copies of ::ITEM e as a stack.
 */
#define item_incr_count( e, v ) (e)->flags = (((e)->flags & ~ITEM_COUNT_MASK) | ((item_get_count_flag( e ) + (((v) & ITEM_COUNT_MASK))) & ITEM_COUNT_MASK))

/**
 * \relates ITEM_NAME
 * \brief The maximum characters in an ITEM::name.
 */
#define ITEM_NAME_SZ 8

/**
 * \brief An instance of an item in the world.
 *
 * All extant items are stored in DSEKAI_STATE::items_handle after being
 * created in-world, typically through a ::SCRIPT attached to a ::TILEMAP.
 *
 * Example copies of items available to create on a given ::TILEMAP are stored
 * in that's tilemap's TILEMAP::item_defs.
 */
struct ITEM {
   retroflat_asset_path sprite_name;
   /**
    * \brief Index of the item's sprite loaded in the
    *        \ref unilayer_graphics_cache.
    */
   int16_t sprite_cache_id;
   /*! \brief Meaningful name of this item displayed in menus, etc. */
   char name[ITEM_NAME_SZ + 1]; /* +1 for NULL. */
   /**
    * \brief \ref dsekai_items_owners for this particular item.
    */
   int16_t owner;
   /**
    * \brief Arbitrary portion of the GID of the item.
    */
   int16_t gid;
   /**
    * \brief Qualitative value specific to each ::ITEM_TYPE_MASK.
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
   /**
    * \brief If ITEM::owner is ::ITEM_OWNER_NONE, the X coord (in tiles) of
    *        this item on the ::TILEMAP referred to by ITEM::map_gid.
    */
   uint8_t x;
   /**
    * \brief If ITEM::owner is ::ITEM_OWNER_NONE, the Y coord (in tiles) of
    *        this item on the ::TILEMAP referred to by ITEM::map_gid.
    */
   uint8_t y;
   /**
    * \brief TILEMAP::gid of the tilemap on which this item is sitting. Only
    *        has meaning when ITEM::owner is set to ::ITEM_OWNER_NONE.
    */
   TILEMAP_GID map_gid;
};

/*! \} */ /* dsekai_items */

#endif /* !ITSTRUCT_H */

