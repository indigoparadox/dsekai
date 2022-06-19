
#ifndef DSEKAI_CONFIG_H
#define DSEKAI_CONFIG_H

/**
 * \addtogroup dsekai_engines
 *
 * \{
 */

/*! \file config.h
 *  \brief Configuration settings defined at compile time.
 */

/*! \addtogroup dsekai_engines_config DSekai Engines Configuration
 *  \brief Compile-time options defining engine behaviors.
 *
 * \{
 */

#define SPRITE_H 16
#define SPRITE_W 16

#ifndef ITEM_INVENTORY_MAX
#define ITEM_INVENTORY_MAX 20
#endif /* !ITEM_INVENTORY_MAX */

#ifndef SCREEN_MAP_W
/*! \brief Width of the overview or viewport area in pixels. */
#define SCREEN_MAP_W (160)
#endif /* !SCREEN_MAP_W */

#ifndef SCREEN_MAP_H
/*! \brief Height of the overview or viewport area in pixels. */
#define SCREEN_MAP_H (160 - 32)
#endif /* !SCREEN_MAP_H */

#ifndef SCREEN_MAP_X
/*! \brief Left offset of the overview or viewport area in pixels. */
#define SCREEN_MAP_X ((SCREEN_W / 2) - (SCREEN_MAP_W / 2))
#endif /* !SCREEN_MAP_X */

#ifndef SCREEN_MAP_Y
/*! \brief Top offset of the overview or viewport area in pixels. */
#define SCREEN_MAP_Y (0)
#endif /* !SCREEN_MAP_Y */

#define WINDOW_STRINGS_MAX 5
#define WINDOW_STRING_LEN_MAX 64

#ifndef STATUS_WINDOW_W
/*! \brief Width in pixels of the status ::WINDOW. */
#define STATUS_WINDOW_W SCREEN_W
#endif /* !STATUS_WINDOW_W */

#ifndef STATUS_WINDOW_H
/*! \brief Height in pixels of the status ::WINDOW. */
#define STATUS_WINDOW_H 32
#endif /* !STATUS_WINDOW_H */

#ifndef ANI_SPRITE_COUNTDOWN_MAX
/*! \brief Number of loops between DSEKAI_STATE::ani_sprite_x changes. */
#define ANI_SPRITE_COUNTDOWN_MAX 10
#endif /* !ANI_SPRITE_COUNTDOWN_MAX */

#ifndef DSEKAI_MOBILES_MAX
/*! \brief Maximum count of ::MOBILE existing on a loaded ::TILEMAP. */
#  define DSEKAI_MOBILES_MAX 20
#endif /* !MOBILES_MAX */

#ifndef DSEKAI_ITEMS_MAX
/*! \brief Maximum number of items existing in the world at any time. */
#  ifdef PLATFORM_DOS
#     define DSEKAI_ITEMS_MAX 50
#  else
#     define DSEKAI_ITEMS_MAX 100
#  endif /* PLATFORM_DOS */
#endif /* !DSEKAI_ITEMS_MAX */

#ifndef TILEMAP_CROP_DEFS_MAX
#  ifdef PLATFORM_DOS
#     define TILEMAP_CROP_DEFS_MAX 5
#  else
#     define TILEMAP_CROP_DEFS_MAX 10
#  endif /* PLATFORM_DOS */
#endif /* !TILEMAP_CROP_DEFS_MAX */

#ifndef TILEMAP_ITEMS_MAX
#  define TILEMAP_ITEMS_MAX 20
#endif /* !TILEMAP_ITEMS_MAX */

#ifndef TILEMAP_SPAWNS_MAX
#  ifdef PLATFORM_DOS
#     define TILEMAP_SPAWNS_MAX 10
#  else
/*! \brief Maximum number of TILEMAP::spawns. */
#     define TILEMAP_SPAWNS_MAX 20
#  endif /* PLATFORM_DOS */
#endif /* !TILEMAP_SPAWNS_MAX */

#ifndef DSEKAI_CROPS_MAX
#  ifdef PLATFORM_DOS
#     define DSEKAI_CROPS_MAX 15
#  else
/*! \brief Maximum number of crops growing in the world accross all maps. */
#     define DSEKAI_CROPS_MAX 40
#  endif /* PLATFORM_DOS */
#endif /* !DSEKAI_CROPS_MAX */

#ifndef CROP_NAME_MAX
#define CROP_NAME_MAX 8
#endif /* !CROP_NAME_MAX */

#ifndef JSON_TOKENS_MAX
#define JSON_TOKENS_MAX 4096
#endif /* !JSON_TOKENS_MAX */

#ifndef JSON_PATH_SZ
#define JSON_PATH_SZ 255
#endif /* !JSON_PATH_SZ */

#ifndef TILEMAP_NAME_MAX
/*! \brief TILEMAP::name maxium length. */
#define TILEMAP_NAME_MAX 8
#endif /* !TILEMAP_NAME_MAX */

#if 0
#ifndef DIALOG_TEXT_SZ
#define DIALOG_TEXT_SZ 128
#endif /* !DIALOG_TEXT_SZ */
typedef char DIALOG_TEXT[DIALOG_TEXT_SZ];
#endif

/*! \} */

/*! \} */

#endif /* !DSEKAI_CONFIG_H */

