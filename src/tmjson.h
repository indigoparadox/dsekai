
#ifndef TMJSON_H
#define TMJSON_H

/*! \file tmjson.h
 *  \brief Constants and functions for populating a TILEMAP from JSON.
 */

#include "dsekai.h"

/*! \brief JSON path to script definition. */
#define TILEMAP_JPATH_SCRIPT "/properties/[name=script_%d]/value"
/*! \brief JSON path to string definition. */
#define TILEMAP_JPATH_STRING "/properties/[name=string_%d]/value"
/*! \brief JSON path to tileset source in map data. */
#define TILEMAP_JPATH_TS_SRC     "/tilesets/0/source"
/*! \brief JSON path to terrain tile TILEMAP::tileset index in map data. */
#define TILEMAP_JPATH_TILE       "/layers/[name=terrain]/data/%d"
/*! \brief JSON path to map name. */
#define TILEMAP_JPATH_PROP_NAME  "/properties/[name=name]/value"
#define TILEMAP_JPATH_PROP_ENGINE "/properties/[name=engine]/value"
/*! \brief JSON path to mobile tile X. */
#define TILEMAP_JPATH_MOB_X      "/layers/[name=mobiles]/objects/%d/x"
/*! \brief JSON path to mobile tile Y. */
#define TILEMAP_JPATH_MOB_Y      "/layers/[name=mobiles]/objects/%d/y"
/*! \brief JSON path to mobile name. */
#define TILEMAP_JPATH_MOB_NAME   "/layers/[name=mobiles]/objects/%d/name"
/*! \brief JSON path to TILEMAP_SPAWN::type. */
#define TILEMAP_JPATH_MOB_TYPE   "/layers/[name=mobiles]/objects/%d/type"
/*! \brief JSON path to mobile script index. */
#define TILEMAP_JPATH_MOB_SCRIPT "/layers/[name=mobiles]/objects/%d/properties/[name=script]/value"
/*! \brief JSON path to TILESET_TILE::image. */
#define TILEMAP_JPATH_TS_TILE "/tiles/%d/image"
/*! \brief JSON path to TILESET_TILE::flags. */
#define TILEMAP_JPATH_TS_FLAGS "/tiles/%d/properties/[name=flags]/value"

/**
 * \brief Load tilemap specified by id into TILEMAP struct t.
 * \param id A RESOURCE_ID indicating the tilemap to load from disk.
 * \param t ::MEMORY_PTR to an empty TILEMAP struct to be loaded into.
 * \return
 */
int16_t tilemap_json_load( RESOURCE_ID id, struct TILEMAP* t );

#endif /* !TMJSON_H */

