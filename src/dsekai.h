
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

/*! \brief Indicates that an animation is being used for TILEMAP::weather. */
#define ANIMATE_FLAG_WEATHER  0x0200
/*! \brief Indicates that an animation was created by a ::SCRIPT. */
#define ANIMATE_FLAG_SCRIPT   0x0100

 /* === Includes === */

#include "config.h"
#include <unilayer.h>

struct DSEKAI_STATE;

#include "itstruct.h"
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
#include "control.h"
#ifdef NETWORK_IRC
#include "irc.h"
#endif /* NETWORK_IRC */
#include "menu.h"

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

