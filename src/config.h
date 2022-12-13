
#ifndef DSEKAI_CONFIG_H
#define DSEKAI_CONFIG_H

/**
 * \addtogroup dsekai_engines
 *
 * \{
 */

/*! \addtogroup dsekai_engines_config DSekai Engines Configuration
 *  \brief Compile-time options defining engine behaviors.
 *  \note These may be overridden by passing a -D argument to the compiler.
 *
 * \{
 */

/**
 * \page dsekai_engines_config_defines Compiler Definitions
 *
 * These definitions can be enabled with -D flags to the compiler in order
 * to change dsekai's behavior:
 * 
 * | Define | Description
 * |--------|------------
 * | SCRIPT_TRACE | Enables detailed script engine execution trace output.
 * | MEMORY_FAKE_TRACE_LOCKS | Enables detailed memory lock trace output.
 * | WINDOW_TRACE | Enables detailed GUI-related trace output.
 *
 * \note \b TRACE definitions also required DEBUG_THRESHOLD to be set to 1 or
 *       lower.
 */

/*! \file config.h
 *  \brief Configuration settings defined at compile time.
 */

#define SPRITE_H 16
#define SPRITE_W 16

#ifndef STATUS_WINDOW_W
/*! \brief Width in pixels of the status ::WINDOW. */
#define STATUS_WINDOW_W SCREEN_W
#endif /* !STATUS_WINDOW_W */

#ifndef STATUS_WINDOW_H
/*! \brief Height in pixels of the status ::WINDOW. */
#define STATUS_WINDOW_H 32
#endif /* !STATUS_WINDOW_H */

#ifndef SCREEN_MAP_W
/**
 * \brief Width of the overview or viewport area in pixels.
 * \attention Changing this value may break compatibility with tilemaps!
 *            Please use caution.
 */
#define SCREEN_MAP_W (160)
#endif /* !SCREEN_MAP_W */

#ifndef SCREEN_MAP_H
/**
 * \brief Height of the overview or viewport area in pixels.
 * \attention Changing this value may break compatibility with tilemaps!
 *            Please use caution.
 */
#define SCREEN_MAP_H (160 - STATUS_WINDOW_H)
#endif /* !SCREEN_MAP_H */

#ifndef SCREEN_MAP_X
/*! \brief Left offset of the overview or viewport area in pixels. */
#define SCREEN_MAP_X ((SCREEN_W / 2) - (SCREEN_MAP_W / 2))
#endif /* !SCREEN_MAP_X */

#ifndef SCREEN_MAP_Y
/*! \brief Top offset of the overview or viewport area in pixels. */
#define SCREEN_MAP_Y (0)
#endif /* !SCREEN_MAP_Y */

#ifndef ANI_SPRITE_COUNTDOWN_MAX
/*! \brief Number of loops between DSEKAI_STATE::ani_sprite_x changes. */
#define ANI_SPRITE_COUNTDOWN_MAX 10
#endif /* !ANI_SPRITE_COUNTDOWN_MAX */

/*! \} */ /* dsekai_engines */

/*! \} */ /* dsekai_engines_config */

/**
 * \addtogroup dsekai_mobiles
 * \{
 */

#ifndef DSEKAI_MOBILES_MAX
/**
 * \brief Maximum count of DSEKAI_STATE::mobile.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#  define DSEKAI_MOBILES_MAX 200
#endif /* !MOBILES_MAX */

#ifndef SCRIPT_STACK_DEPTH
/**
 * \brief Maximum depth of available local stack for each ::MOBILE executing
 *        a script. Each stack is individual to that particular mobile. When
 *        this depth is exceeded, old excess items will be discarded off the
 *        bottom.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#  define SCRIPT_STACK_DEPTH 10
#endif /* !SCRIPT_STACK_DEPTH */

/*! \} */ /* dsekai_mobiles */

/**
 * \addtogroup scripting
 */

#ifndef SCRIPT_STEPS_MAX
/*! \brief Maximum number of steps in an individual script. */
#define SCRIPT_STEPS_MAX 128
#endif /* !SCRIPT_STEPS_MAX */

#ifndef SCRIPT_STR_MAX
/*! \brief Maximum length of a script in text form. */
#define SCRIPT_STR_MAX 4096
#endif /* !SCRIPT_STR_MAX */

/*! \} */ /* scripting */

/**
 * \addtogroup dsekai_items
 * \{
 */

#ifndef ITEM_INVENTORY_MAX
/**
 * \brief Maximum number of items in a single ::MOBILE inventory.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#define ITEM_INVENTORY_MAX 20
#endif /* !ITEM_INVENTORY_MAX */

#ifndef DSEKAI_ITEMS_MAX
/**
 * \brief Maximum number of items existing in the world at any time.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#  define DSEKAI_ITEMS_MAX 200
#endif /* !DSEKAI_ITEMS_MAX */

/*! \} */ /* dsekai_items */

/**
 * \addtogroup dsekai_tilemaps
 * \{
 */

#ifndef TILE_W
/*! \brief TILESET_TILE width in pixels. */
#  define TILE_W 16
#endif /* !TILE_W */

#ifndef TILE_H
/*! \brief TILESET_TILE height in pixels. */
#  define TILE_H 16
#endif /* TILE_H */

#ifndef TILEMAP_CROP_DEFS_MAX
/**
 * \brief Maximum count of TILEMAP::crop_defs.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#  define TILEMAP_CROP_DEFS_MAX 10
#endif /* !TILEMAP_CROP_DEFS_MAX */

#ifndef TILEMAP_ITEMS_MAX
/**
 * \brief Maximum count of TILEMAP::item_defs.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#  define TILEMAP_ITEMS_MAX 20
#endif /* !TILEMAP_ITEMS_MAX */

#ifndef TILEMAP_SPAWNS_MAX
/**
 * \brief Maximum count of TILEMAP::spawns.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#  define TILEMAP_SPAWNS_MAX 20
#endif /* !TILEMAP_SPAWNS_MAX */

#ifndef JSON_TOKENS_MAX
#define JSON_TOKENS_MAX 4096
#endif /* !JSON_TOKENS_MAX */

#ifndef JSON_PATH_SZ
#define JSON_PATH_SZ 255
#endif /* !JSON_PATH_SZ */

#ifndef TILEMAP_STRPOOL_SZ
#define TILEMAP_STRPOOL_SZ 1024
#endif /* !TILEMAP_STRPOOL_SZ */

#ifndef TILEMAP_NAME_MAX
/**
 * \brief Maximum size of TILEMAP::name.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#define TILEMAP_NAME_MAX 8
#endif /* !TILEMAP_NAME_MAX */

/*! \} */ /* dsekai_tilemaps */

/**
 * \addtogroup dsekai_crops
 * \{
 */

#ifndef DSEKAI_CROPS_MAX
/**
 * \brief Maximum count of DSEKAI_STATE::crops.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#  define DSEKAI_CROPS_MAX 40
#endif /* !DSEKAI_CROPS_MAX */

#ifndef CROP_NAME_MAX
/**
 * \brief Maximum size of CROP_DEF::name.
 * \note This may be overridden by passing a -D argument to the compiler.
 * \attention Changing this value may break compatibility with tilemaps or
 *            saves! Please use caution.
 */
#define CROP_NAME_MAX 8
#endif /* !CROP_NAME_MAX */

/*! \} */ /* dsekai_crops */

#endif /* !DSEKAI_CONFIG_H */

