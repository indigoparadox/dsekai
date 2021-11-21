
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
#define PATTERN_W 16
#define PATTERN_H 16

#ifndef ITEM_INVENTORY_MAX
#define ITEM_INVENTORY_MAX 20
#endif /* !ITEM_INVENTORY_MAX */

#define SCREEN_MAP_W (SCREEN_W)
#define SCREEN_MAP_H (160 - 32)

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
#define DSEKAI_MOBILES_MAX 20
#endif /* !MOBILES_MAX */

#ifndef DSEKAI_ITEMS_MAX
/*! \brief Maximum number of items existing in the world at any time. */
#define DSEKAI_ITEMS_MAX 40
#endif /* !DSEKAI_ITEMS_MAX */

/*! \} */

/*! \} */

#endif /* !DSEKAI_CONFIG_H */

