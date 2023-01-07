
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

/**
 * \addtogroup dsekai_items_owners Item Owner ID
 * \brief Item ownership in-world.
 *
 * If an owner ID is not one of the special values on this page, then it is
 * considered to be the index of an owning ::MOBILE in DSEKAI_STATE::mobiles.
 * \{
 */

/**
 * \brief ITEM::owner value indicating no owner.
 *
 * Items with no owner are associated with their ITEM::map_gid to a ::TILEMAP,
 * and are visible in-world at the tile located by their ITEM::x and ITEM::y
 * when the player is on that ::TILEMAP.
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

struct CROP_PLOT;

/**
 * \addtogroup dsekai_items_use Item Types and Use
 * \brief Definitions and callbacks pertaining to ::ITEM use in-world.
 *
 * An item's type, defined by the bits marked by ::ITEM_TYPE_MASK in its
 * \ref dsekai_items_flags, primarily decides what function that item will
 * perform when used in-world.
 * \{
 */

/**
 * \brief List of possible ::ITEM_TYPE_MASK values and their maximum stack
 *        sizes
 */
#define ITEM_TABLE( f ) f( none, 0 ) f( seed, 10 ) f( food, 10 ) f( shovel, 1 ) f( editor, 1 ) f( material, 10 ) f( watercan, 1 ) f( hoe, 1 )

#define ITEM_USED_SUCCESSFUL -1
#define ITEM_USED_FAILED 0
#define ITEM_USED_SUCCESSFUL_SILENT 1

/*! \} */ /* dsekai_items_flags */

#define item_break_if_last( items, i ) if( ITEM_FLAG_NOT_LAST != (ITEM_FLAG_NOT_LAST & items[i].flags) ) { debug_printf( 1, "breaking early on item %d!", i ); break; }

/**
 * \brief Determine if/where item with the given ITEM::gid exists in the
 *        inventory of an \ref dsekai_items_owners.
 * \param template_gid ITEM::gid of the definition in TILEMAP::items.
 * \param owner_id \ref dsekai_items_owners of the sought item.
 * \param state Locked ::MEMORY_PTR to the current engine ::DSEKAI_STATE.
 * \return Index of the item in DSEKAI_STATE::items_handle if it exists or
 *         ::ITEM_ERROR_NOT_FOUND if not.
 */
int16_t item_exists_in_inventory(
   int16_t template_gid, int16_t owner_id, struct DSEKAI_STATE* state
) SECTION_ITEM;

int16_t item_decr_or_delete(
   int16_t e_idx, struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \brief Create an item in DSEKAI_STATE::items_handle from a template in
 *        TILEMAP::item_defs or increment its count if possible.
 *
 * If the item exists in the inventory of the owner it's being added to,
 * this function will attempt to increment its count (stored in its
 * \ref dsekai_items_flags).
 *
 * If the item does not exist, or its count in its owner's inventory has
 * exceeded the maximum defined for its \ref dsekai_items_use in
 * ::gc_items_max, then a new ::ITEM will be created in the inventory of that
 * owner (unless that inventory has reached ::ITEM_INVENTORY_MAX).
 *
 * \param template_gid ITEM::gid of the template in TILEMAP::item_defs to add.
 * \param owner_id \ref dsekai_items_owners to give the item to.
 * \param t Locked ::MEMORY_PTR to the ::TILEMAP containing template_gid in
 *          its TILEMAP::item_defs.
 * \param state Locked ::MEMORY_PTR to the current engine ::DSEKAI_STATE.
 * \return Index of the new item or stack added to in
 *         DSEKAI_STATE::items_handle.
 */
int16_t item_stack_or_add(
   int16_t template_gid, int16_t owner_id, struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \param e_idx Index of the ::ITEM to give in the current
 *        DSEKAI_STATE::items_handle.
 * \param owner_id \ref dsekai_items_owners to give the item to.
 * \param t Locked ::MEMORY_PTR to the ::TILEMAP containing template_gid in
 *          its TILEMAP::item_defs.
 * \param state Locked ::MEMORY_PTR to the current engine ::DSEKAI_STATE.
 * \return New item index in DSEKAI_STATE::items_handle if give was successful,
 *         or error code otherwise.
 */
int16_t item_give_mobile(
   int16_t e_idx, int16_t owner_id, struct DSEKAI_STATE* state ) SECTION_ITEM;

/**
 * \brief Drop am item on the map floor where its owner is standing.
 * \param e_idx Index of the ::ITEM to drop in the current
 *        DSEKAI_STATE::items_handle.
 * \param t Locked ::MEMORY_PTR to the ::TILEMAP on which the item is being
 *          dropped.
 * \param state Locked ::MEMORY_PTR to the current engine ::DSEKAI_STATE.
 * \return 1 if give was successful, or error code otherwise.
 */
/* TODO: Develop item GID. */
int8_t item_drop( int16_t e_idx, struct DSEKAI_STATE* state ) SECTION_ITEM;

/**
 * \brief Pick up an item at the given x, y tile coordinates on the given
 *        ::TILEMAP.
 * \param owner_id \ref dsekai_items_owners to give the item to if found.
 * \param t Locked ::MEMORY_PTR to the ::TILEMAP from which the item is being
 *          picked up.
 * \param state Locked ::MEMORY_PTR to the current engine ::DSEKAI_STATE.
 * \return Index of picked up item, or ::DSEKAI_ITEMS_MAX if no item was
 *         picked up.
 */
int16_t item_pickup_xy(
   uint8_t x, uint8_t y, int16_t owner_id, struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \addtogroup dsekai_items_use
 * \{
 */

/**
 * \brief Definition for a callback to execute when an ::ITEM is used.
 * \param e_idx Index of the ::ITEM to use in the current
 *        DSEKAI_STATE::items_handle.
 * \param owner_id \ref dsekai_items_owners to use the item.
 * \param state Locked ::MEMORY_PTR to the current engine ::DSEKAI_STATE.
 * \return ::ITEM_USED_SUCCESSFUL if the item was used successfully or
 *         ::ITEM_USED_FAILED if not. ::ITEM_USED_SUCCESSFUL_SILENT if the item
 *         was used and the menu should not be closed (the menu will always
 *         be closed if the item use failed.
 *
 * \attention In general, the menu should always be closed if the item's use
 *            will create a ::WINDOW, as those might conflict with the menu.
 */
typedef int8_t (*ITEM_USE_CB)(
   int16_t e_idx, int16_t owner_id, struct DSEKAI_STATE* state );

/**
 * \brief Macro to define ::ITEM_USE_CB prototypes from ::ITEM_TABLE.
 */
#define ITEM_TABLE_USE_CB_PROTOS( type, max ) int8_t item_use_ ## type( int16_t e_idx, int16_t owner_id, struct DSEKAI_STATE* state ) SECTION_ITEM;

ITEM_TABLE( ITEM_TABLE_USE_CB_PROTOS )

/*! \} */ /* dsekai_items_use */

#ifdef ITEM_C

#  define ITEM_TABLE_MAX( type, max ) max,

/* TODO: Custom item types with scripts. */

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

#  ifdef NO_ITEM_HANDLERS

#     define ITEM_TABLE_USE_CB_STUBS( type, max ) int8_t item_use_ ## type( int16_t e_idx, int16_t owner_id, struct DSEKAI_STATE* state ) { return 0; }

ITEM_TABLE( ITEM_TABLE_USE_CB_STUBS );

ITEM_TABLE( ITEM_TABLE_USE_CB_PROTOS )

#  endif /* NO_ITEM_HANDLERS */

#else

/**
 * \addtogroup dsekai_items_use
 * \{
 */

/*! \brief Lookup table of maximum of each item type permitted in inventory. */
extern RES_CONST uint8_t gc_items_max[];

/*! \brief Lookup table of callbacks to execute for item types. */
extern RES_CONST ITEM_USE_CB gc_item_use_cbs[];

/*! \brief Lookup table of text names for item types. */
extern RES_CONST char* gc_items_types[];

/*! \} */

#endif /* ITEM_C */

/* !\} */ /* dsekai_items */

#endif /* ITEM_H */

