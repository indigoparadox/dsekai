
#ifndef GID_H
#define GID_H

/**
 * \addtogroup dsekai_gid Globally Unique Identifiers
 * \brief Identifiers for objects that are unique among ALL ::TILEMAP objects
 *        and persistant between \ref dsekai_engines_locking cycles.
 *
 * These should be used in cases longer than one unilayer_loop_iter(), as a
 * ::MEMORY_PTR may be relocated in memory when unlocked and array indexes
 * may change if the array is compacted.
 * \{
 */

/**
 * \file gid.h
 * \brief Contains macros and typedefs for globally unique identifiers.
 */

/*! \brief Globally unique identifier for ::MOBILE objects. */
typedef uint32_t MOBILE_GID;
/*! \brief Globally unique identifier for ::TILEMAP_SPAWN objects. */
typedef uint16_t SPAWN_GID;
/*! \brief Globally unique identifier for ::TILEMAP objects. */
typedef uint16_t TILEMAP_GID;

/**
 * \brief Special case unique identifier for the spawner of the 
 *        player-controlled ::MOBILE.
 *
 * This should exist only on one tilemap (the first tilemap loaded in a new
 * world).
 */
#define SPAWN_GID_PLAYER 0xffff

#define MOBILE_GID_NONE 0

/*! \brief Special case unique identifier for the player-controlled ::MOBILE. */
#define MOBILE_GID_PLAYER 0xffffffff

/**
 * \relates MOBILE
 * \brief MOBILE::map_gid value indicating mobile is present on all tilemaps.
 *
 * This is generally only used for the player.
 */
#define MOBILE_MAP_GID_ALL 0xffff

/**
 * \relates MOBILE
 * \brief Insert into logging format strings when a mobile's
 *        \ref dsekai_gid is needed.
 */
#define MOBILE_GID_FMT "%04d:%04d"

/**
 * \relates MOBILE
 * \brief Insert into the args provided to a format string using
 *        ::MOBILE_GID_FMT.
 * \param m Locked ::MEMORY_PTR to the mobile to examine.
 */
#define mobile_get_gid_fmt( m ) (m)->map_gid, (m)->spawner_gid

/*! \} */

#endif /* !GID_H */

