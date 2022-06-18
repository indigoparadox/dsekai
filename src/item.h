
#ifndef ITEM_H
#define ITEM_H

/**
 * \addtogroup dsekai_items Items
 *
 * \{
 */

/*! \file item.h
 *  \brief Definitions, functions, and macros pertaining to inventory objects.
 */

#define ITEM_TABLE( f ) f( none, 0 ) f( seed, 10 ) f( food, 10 ) f( shovel, 1 ) f( editor, 1 ) f( material, 10 ) f( watercan, 1 ) f( hoe, 1 )

/**
 * \addtogroup dsekai_items_owners Item Owner ID
 * \brief Item ownership by a ::MOBILE.
 * \{
 */

/**
 * \brief ITEM::owner value indicating no owner.
 */
#define ITEM_OWNER_NONE -1

/**
 * \brief ITEM::owner value indicating item is in player inventory.
 */
#define ITEM_OWNER_PLAYER -2

/*! \} */ /* dsekai_items_owners */

/**
 * \addtogroup dsekai_items_errors Item Errors
 * \{
 */

#define ITEM_ERROR_INV_FULL -1
#define ITEM_ERROR_DUPLICATE -2
#define ITEM_ERROR_MISSING_TEMPLATE -3
#define ITEM_ERROR_NOT_FOUND -4

/*! \} */ /* dsekai_items_errors */

/**
 * \relates ITEM
 * \brief Get the "true GID" of an item.
 * \param item
 *
 * The "true GID" of an item is its ITEM::type and ITEM::owner combined with
 * its ITEM::gid. Because of this, it is legal to have two items with the
 * same ITEM::gid in the world if they have a different ITEM::owner or
 * ITEM::type.
 */
#define item_true_gid( item ) ((uint32_t*)&((*item).type))

/**
 * \brief Determine if/where item with the given ITEM::gid exists in the
 *        inventory of an \ref dsekai_items_owners.
 * \param template_gid ITEM::gid of the definition in TILEMAP::items.
 * \param owner_id \ref dsekai_items_owners of the sought item.
 * \return Index of the item in DSEKAI_STATE::items if it exists or
 *         ::ITEM_ERROR_NOT_FOUND if not.
 */
int8_t item_exists_in_inventory(
   int16_t template_gid, int8_t owner_id, struct DSEKAI_STATE* state
) SECTION_ITEM;

int8_t item_decr_or_delete(
   int16_t template_gid, int8_t owner_id, struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \brief Create an item in DSEKAI_STATE::items from a template in
 *        TILEMAP::items.
 * \param m \ref dsekai_items_owners to give the item to.
 * \return Index of the new item or stack added to in DSEKAI_STATE::items.
 */
int8_t item_stack_or_add(
   int16_t template_gid, int8_t owner_id,
   struct TILEMAP* t, struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \param m \ref dsekai_items_owners to give the item to.
 * \return New item index in DSEKAI_STATE::items if give was successful,
 *         or error code otherwise.
 */

int8_t item_give_mobile(
   int8_t e_idx, int8_t m_idx, struct TILEMAP* t, struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \brief Drop am item on the map floor where its owner is standing.
 * \param e ::MEMORY_PTR to the item to drop.
 * \return 1 if give was successful, or error code otherwise.
 */
int8_t item_drop(
   int8_t e_idx, struct TILEMAP* t, struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \brief Pick up an item at the given x, y tile coordinates on the current
 *        DSEKAI_STATE::map_handle.
 * \param owner \ref dsekai_items_owners to give the item to if found.
 * \return Index of picked up item, or ::DSEKAI_ITEMS_MAX if no item was
 *         picked up.
 */
int8_t item_pickup_xy(
   uint8_t x, uint8_t y, int8_t owner, struct TILEMAP* t,
   struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \return 1 if the item was used successfully or 0 otherwise. -1 if the item
 *         was used and the menu should be closed.
 */
typedef int8_t (*ITEM_USE_CB)(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state );

#define ITEM_TABLE_USE_CB_PROTOS( type, max ) int8_t item_use_ ## type( int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state ) SECTION_ITEM;

ITEM_TABLE( ITEM_TABLE_USE_CB_PROTOS )

#ifdef ITEM_C

#  define ITEM_TABLE_MAX( type, max ) max,

RES_CONST uint8_t gc_items_max[] = {
   ITEM_TABLE( ITEM_TABLE_MAX )
};

#  define ITEM_TABLE_TYPES( type, max ) #type,

RES_CONST char* gc_items_types[] = {
   ITEM_TABLE( ITEM_TABLE_TYPES )
   ""
};

#  define ITEM_TABLE_USE_CBS( type, max ) item_use_ ## type,

RES_CONST ITEM_USE_CB gc_item_use_cbs[] = {
   ITEM_TABLE( ITEM_TABLE_USE_CBS )
};

#else

/*! \brief Lookup table of maximum of each item type permitted in inventory. */
extern RES_CONST uint8_t gc_items_max[];

extern RES_CONST ITEM_USE_CB gc_item_use_cbs[];

extern RES_CONST char* gc_items_types[];

#endif /* ITEM_C */

/* !\} */

#endif /* ITEM_H */

