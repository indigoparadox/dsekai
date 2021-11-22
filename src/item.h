
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

#define ITEM_TABLE( f ) f( 0, NONE, 0 ) f( 1, SEED, 10 )

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
   struct ITEM* e, struct MOBILE* m, struct DSEKAI_STATE* state );

#ifdef ITEM_C

#define ITEM_TABLE_MAX( idx, type, max ) max,

const uint8_t gc_items_max[] = {
   ITEM_TABLE( ITEM_TABLE_MAX )
};

#else

/*! \brief Lookup table of maximum of each item type permitted in inventory. */
extern const uint8_t gc_items_max[];

#endif

/* !\} */

#endif /* ITEM_H */

