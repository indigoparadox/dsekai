
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
 * \relates ITEM
 * \brief ITEM::owner value indicating no owner.
 */
#define ITEM_OWNER_NONE -1

/**
 * \relates ITEM
 * \brief ITEM::owner value indicating item is in player inventory.
 */
#define ITEM_OWNER_PLAYER -2

/**
 * \relates ITEM
 * \brief ITEM::owner value indicating item is a "meta" item handed out by a
 *        script.
 *
 * See \ref scripting_commands_h for more information.
 */
#define ITEM_OWNER_META -3

#define ITEM_ERROR_INV_FULL -1
#define ITEM_ERROR_DUPLICATE -2

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

void item_draw( const struct ITEM*, int16_t, int16_t );

/**
 * \param e ::MEMORY_PTR to the template to give copy of to the ::MOBILE.
 * \param m ::MEMORY_PTR to the ::MOBILE to receive new ::ITEM.
 * \return 1 if give was successful, or error code otherwise.
 */

int8_t item_give_mobile(
   struct ITEM* e, struct MOBILE* m, struct DSEKAI_STATE* state
) SECTION_ITEM;

/**
 * \return 1 if the item was used successfully or 0 otherwise. -1 if the item
 *         was used and the menu should be closed.
 */
typedef int8_t (*ITEM_USE_CB)(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state );

#define ITEM_TABLE_USE_CB_PROTOS( type, max ) int8_t item_use_ ## type( struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state ) SECTION_ITEM;

ITEM_TABLE( ITEM_TABLE_USE_CB_PROTOS )

#ifdef ITEM_C

#define ITEM_TABLE_MAX( type, max ) max,

RES_CONST uint8_t gc_items_max[] = {
   ITEM_TABLE( ITEM_TABLE_MAX )
};

#define ITEM_TABLE_TYPES( type, max ) #type,

RES_CONST char* gc_items_types[] = {
   ITEM_TABLE( ITEM_TABLE_TYPES )
   ""
};

#define ITEM_TABLE_USE_CBS( type, max ) item_use_ ## type,

RES_CONST ITEM_USE_CB gc_item_use_cbs[] = {
   ITEM_TABLE( ITEM_TABLE_USE_CBS )
};

#else

/*! \brief Lookup table of maximum of each item type permitted in inventory. */
extern RES_CONST uint8_t gc_items_max[];

extern RES_CONST ITEM_USE_CB gc_item_use_cbs[];

extern RES_CONST char* gc_items_types[];

#endif

/* !\} */

#endif /* ITEM_H */

