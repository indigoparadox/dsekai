
#ifndef TMJSON_H
#define TMJSON_H

/**
 * \addtogroup dsekai_tilemaps
 *
 * \{
 */

/*! \file tmjson.h
 *  \brief Constants and functions for populating a TILEMAP from \ref json.
 */

#include "dsekai.h"

/**
 * \addtogroup dsekai_tilemaps_json_format_sect Tilemaps JSON Format
 * 
 * \{
 * \page dsekai_tilemaps_json_format
 *
 * \}
 */

/*! \brief Maximum possible length of an individual dialog string. */
#define TILEMAP_JSON_STRBUF_SZ 128

/**
 * \addtogroup dsekai_tilemaps_json_paths Tilemaps JSON Paths
 * \brief Paths to map properties in a Tiled \ref json file.
 *
 * All paths here use \ref json_paths syntax.
 *
 * Some paths use %%d conversion specifiers as placeholders for the index of a
 * discrete value being iterated over.
 *
 * \{
 */

/**
 * \brief JSON path to script definition.
 *
 * Scripts are stored in map properties as "script_n", where n represents the
 * index of the script as referred to by MOBILE::script_id.
 *
 * Scripts are stored as sequences of a single character (shorthand for a
 * statement) followed by a number (an argument). They must be parsed by
 * ::script_parse_str.
 */
#define TILEMAP_JPATH_SCRIPT "/properties/[name=script_%d]/value"
/*! \brief JSON path to string definition. */
#define TILEMAP_JPATH_STRING "/properties/[name=string_%d]/value"
/*! \brief JSON path to TILEMAP::gid. */
#define TILEMAP_JPATH_GID "/properties/[name=gid]/value"
/*! \brief JSON path to tileset source in map data. */
#define TILEMAP_JPATH_TS_SRC     "/tilesets/0/source"
/*! \brief JSON path to terrain tile TILEMAP::tileset index in map data. */
#define TILEMAP_JPATH_TILE       "/layers/[name=terrain]/data/%d"
/*! \brief JSON path to map name. */
#define TILEMAP_JPATH_PROP_NAME  "/properties/[name=name]/value"
/*! \brief JSON path to map engine type. */
#define TILEMAP_JPATH_PROP_ENGINE "/properties/[name=engine]/value"
/*! \brief JSON path to weather property. */
#define TILEMAP_JPATH_PROP_WEATHER "/properties/[name=weather]/value"
/*! \brief JSON path to editable property. */
#define TILEMAP_JPATH_PROP_EDITABLE "/properties/[name=editable]/value"
/*! \brief JSON path to mobile tile X. */
#define TILEMAP_JPATH_MOB_X      "/layers/[name=mobiles]/objects/%d/x"
/*! \brief JSON path to mobile tile Y. */
#define TILEMAP_JPATH_MOB_Y      "/layers/[name=mobiles]/objects/%d/y"
/*! \brief JSON path to mobile name. */
#define TILEMAP_JPATH_MOB_NAME   "/layers/[name=mobiles]/objects/%d/name"
/*! \brief JSON path to TILEMAP_SPAWN::type. */
#define TILEMAP_JPATH_MOB_SPRITE   "/layers/[name=mobiles]/objects/%d/class"
/*! \brief JSON path to TILEMAP_SPAWN::gid. */
#define TILEMAP_JPATH_MOB_GID   "/layers/[name=mobiles]/objects/%d/properties/[name=gid]/value"
/*! \brief JSON path to mobile script index. */
#define TILEMAP_JPATH_MOB_SCRIPT "/layers/[name=mobiles]/objects/%d/properties/[name=script]/value"
/*! \brief JSON path to mobile type. */
#define TILEMAP_JPATH_MOB_TYPE_FLAG "/layers/[name=mobiles]/objects/%d/properties/[name=type]/value"
#define TILEMAP_JPATH_MOB_PLAYER_FLAG "/layers/[name=mobiles]/objects/%d/properties/[name=player]/value"
#define TILEMAP_JPATH_MOB_ASCII   "/layers/[name=mobiles]/objects/%d/properties/[name=ascii]/value"
/*! \brief JSON path to TILESET_TILE::image. */
#define TILEMAP_JPATH_TS_TILE "/tiles/%d/image"
/*! \brief JSON path to TILESET_TILE::flags. */
#define TILEMAP_JPATH_TS_FLAGS "/tiles/%d/properties/[name=flags]/value"
#define TILEMAP_JPATH_TS_ASCII "/tiles/%d/properties/[name=ascii]/value"
#define TILEMAP_JPATH_ITEM_GID "/layers/[name=items]/objects/%d/properties/[name=gid]/value"
#define TILEMAP_JPATH_ITEM_NAME "/layers/[name=items]/objects/%d/name"
#define TILEMAP_JPATH_ITEM_TYPE "/layers/[name=items]/objects/%d/class"
#define TILEMAP_JPATH_ITEM_SPRITE "/layers/[name=items]/objects/%d/properties/[name=sprite]/value"
#define TILEMAP_JPATH_ITEM_DATA "/layers/[name=items]/objects/%d/properties/[name=data]/value"
#define TILEMAP_JPATH_ITEM_CRAFTABLE "/layers/[name=items]/objects/%d/properties/[name=craftable]/value"
#define TILEMAP_JPATH_CROP_DEF_NAME "/layers/[name=crops]/objects/%d/name"
#define TILEMAP_JPATH_CROP_DEF_GID "/layers/[name=crops]/objects/%d/properties/[name=gid]/value"
#define TILEMAP_JPATH_CROP_DEF_CYCLE "/layers/[name=crops]/objects/%d/properties/[name=cycle]/value"
#define TILEMAP_JPATH_CROP_DEF_REGROWS "/layers/[name=crops]/objects/%d/properties/[name=regrows]/value"
#define TILEMAP_JPATH_CROP_DEF_SPRITE "/layers/[name=crops]/objects/%d/properties/[name=sprite]/value"
#define TILEMAP_JPATH_CROP_DEF_PRODUCE_GID "/layers/[name=crops]/objects/%d/properties/[name=produce]/value"

/*! \} */

/**
 * \brief Load tilemap specified by id into TILEMAP struct t.
 * \param id A RESOURCE_ID indicating the tilemap to load from disk.
 * \param t ::MEMORY_PTR to an empty TILEMAP struct to be loaded into.
 * \return
 */
int16_t tilemap_json_load( const RESOURCE_ID id, struct TILEMAP* t );

/*! \} */

#endif /* !TMJSON_H */

