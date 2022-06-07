
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

struct DSEKAI_STATE;

#include "winids.h"

#include "itstruct.h"
#include "cropdef.h"
#include "tmstruct.h"

#include "strpool.h"

#ifdef RESOURCE_FILE
#include "json.h"
#include "asn.h"
#endif /* RESOURCE_FILE */
#include "script.h"
#include "item.h"
#include "tilemap.h"
#include "mobile.h"
#include "window.h"
#ifdef NETWORK_IRC
#include "irc.h"
#endif /* NETWORK_IRC */
#include "menu.h"
#include "crop.h"

#ifdef TILEMAP_FMT_JSON
#include "tmjson.h"
#endif /* TILEMAP_FMT_JSON */

#ifdef TILEMAP_FMT_ASN
#include "tmasn.h"
#endif /* TILEMAP_FMT_ASN */

#ifdef TILEMAP_FMT_BIN
#include "tmbin.h"
#endif /* TILEMAP_FMT_BIN */

#include "engines.h"

#endif /* !DSEKAI_H */

