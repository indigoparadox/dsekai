
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

#define ITEM_TABLE( f ) f( 0, NONE ) f( 1, SEED )

/*! \brief ITEM::owner value indicating no owner. */
#define ITEM_OWNER_NONE -1
/*! \brief ITEM::owner value indicating item is in player inventory. */
#define ITEM_OWNER_PLAYER -2
/**
 * \brief ITEM::owner value indicating item is a "meta" item handed out by a
 *        script.
 *
 * See \ref scripting_commands_h for more information.
 */
#define ITEM_OWNER_META -3

/**
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

#ifdef ITEM_C

#else
#endif

/* !\} */

#endif /* ITEM_H */

