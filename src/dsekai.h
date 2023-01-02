
#ifndef DSEKAI_H
#define DSEKAI_H

/*! \file dsekai.h
 *  \brief Engine state data and main header, used in every engine file.
 *
 *  This file should be used to call all other engine headers, to ensure
 *  dependencies are kept and in order.
 *
 *  Data types and constants that are used in multiple engine headers should
 *  be placed in this file.
 */

#define DSEKAI_ERROR_ALLOCATE -128

/**
 * \relates ANIMATION
 * \brief Indicates that an animation is behind tiles.
 */
#define ANIMATE_FLAG_BG  0x0400
/**
 * \relates ANIMATION
 * \brief Indicates that an animation is in front of tiles.
 */
#define ANIMATE_FLAG_FG  0x0800
/**
 * \relates ANIMATION
 * \brief Indicates that an animation is being used for TILEMAP::weather.
 */
#define ANIMATE_FLAG_WEATHER  0x0200
/**
 * \relates ANIMATION
 * \brief Indicates that an animation was created by a ::SCRIPT.
 */
#define ANIMATE_FLAG_SCRIPT   0x0100

#ifdef PLATFORM_PALM
#define SECTION_CROP __attribute__ ((section( "cropfns" )))
#define SECTION_ITEM __attribute__ ((section( "itemfns" )))
#define SECTION_MOBILE __attribute__ ((section( "mobfns" )))
#define SECTION_SCRIPT __attribute__ ((section( "scrfns" )))
#define SECTION_TILEMAP __attribute__ ((section( "tmapfns" )))
#else
#define SECTION_CROP
#define SECTION_ITEM
#define SECTION_MOBILE
#define SECTION_SCRIPT
#define SECTION_TILEMAP
#endif /* PLATFORM_PALM */

 /* === Includes === */

#include <unilayer.h>
#include "config.h"

/**
 * \brief Convenience macro for creating a dialog ::WINDOW with a sprite and
 *        text string specified from TILEMAP::strpool.
 * \image html windowsp.png
 * \param dialog Index of the string to display from TILEMAP::strpool.
 * \param sprite RESOURCE_NAME of the GRAPHICS_BITMAP to display in this window.
 */
#define window_prefab_dialog( dialog, sprite, dir_flag, fg, bg ) window_push( WINDOW_ID_SCRIPT_SPEAK, 0, WINDOW_TYPE_WINDOW, WINDOW_FLAG_MODAL, SCREEN_MAP_X, WINDOW_PLACEMENT_CENTER, SCREEN_MAP_W, WINDOW_SIZE_AUTO, fg, bg, 0, 0, NULL ); window_push( WINDOW_ID_SCRIPT_SPEAK + 1, WINDOW_ID_SCRIPT_SPEAK, WINDOW_TYPE_LABEL, 0, WINDOW_PLACEMENT_CENTER, 30, WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER, fg, bg, GRAPHICS_STRING_FLAG_FONT_SCRIPT, 0, dialog ); window_push( WINDOW_ID_SCRIPT_SPEAK + 2, WINDOW_ID_SCRIPT_SPEAK, WINDOW_TYPE_SPRITE, dir_flag | WINDOW_FLAG_SPRITE_BORDER_SINGLE, WINDOW_PLACEMENT_CENTER, 6, WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO, fg, bg, 0, sprite, NULL ); 

#define window_prefab_system_dialog( dialog, fg, bg ) window_push( WINDOW_ID_SCRIPT_SPEAK, 0, WINDOW_TYPE_WINDOW, WINDOW_FLAG_MODAL, SCREEN_MAP_X, WINDOW_PLACEMENT_CENTER, SCREEN_MAP_W, WINDOW_SIZE_AUTO, fg, bg, 0, 0, NULL ); window_push( WINDOW_ID_SCRIPT_SPEAK + 1, WINDOW_ID_SCRIPT_SPEAK, WINDOW_TYPE_LABEL, 0, WINDOW_PLACEMENT_CENTER, 30, WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER, fg, bg, 0, 0, dialog ); window_push( WINDOW_ID_SCRIPT_SPEAK + 2, WINDOW_ID_SCRIPT_SPEAK, WINDOW_TYPE_SPRITE, MOBILE_DIR_SOUTH | WINDOW_FLAG_SPRITE_BORDER_SINGLE, WINDOW_PLACEMENT_CENTER, 6, WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO, fg, bg, 0, state->player.sprite_cache_id, NULL ); 

struct DSEKAI_STATE;

#include "winids.h"

#include "itstruct.h"
#include "cropdef.h"
#include "tmstruct.h"

#include "strpool.h"

#include "script.h"
#include "item.h"
#include "tilemap.h"
#include "mobile.h"
#ifdef NETWORK_IRC
#include "irc.h"
#endif /* NETWORK_IRC */
#include "menu.h"
#include "crop.h"
#include "pathfind.h"

#ifdef TILEMAP_FMT_JSON
#include "tmjson.h"
#endif /* TILEMAP_FMT_JSON */

#ifdef TILEMAP_FMT_ASN
#include "tmasn.h"
#endif /* TILEMAP_FMT_ASN */

#ifdef TILEMAP_FMT_BIN
#include "tmbin.h"
#endif /* TILEMAP_FMT_BIN */

#include "serial.h"

#include "engines.h"

#endif /* !DSEKAI_H */

