
#ifndef HEADPACK_H
#define HEADPACK_H

/*! \file headpack.h
 *  \brief Functions comprising the headpack resource tool.
 */

#include <stdio.h>

#define HEADPACK_INCLUDE_GUARD "RESEMB_H"

#define HEADPACK_INCLUDES_TABLE( f ) f( ../../src/config.h ) f( ../../src/itstruct.h ) f( ../../src/cropdef.h ) f( ../../src/tmstruct.h )

#define HEADPACK_FNAME_MAX 255

#define HEADPACK_STATE_IN_FMT_ARG 1
#define HEADPACK_STATE_OUT_FMT_ARG 2

#define HEADPACK_DEFS_MAX 255

#define HEADPACK_TYPE_MAX 8

/*! \brief Constant indicating path is to a bitmap (or binary) file. */
#define PATH_TYPE_BIN 0
/*! \brief Constant indicating path is to a tilemap (text) file. */
#define PATH_TYPE_TXT 1

typedef int (*headpack_writer)( const char*, FILE* );
typedef int (*headpack_indexer)( const char*[], int, FILE* );

struct HEADPACK_DEF {
   char prefix;
   headpack_writer writer;
   headpack_indexer indexer;
   char type[HEADPACK_TYPE_MAX];
};

int headpack_register(
   char prefix, headpack_writer writer, headpack_indexer indexer,
   const char* type );

/**
 * \brief Determine if a path points to a bitmap or tilemap.
 * \param path Path to examine the type of, by file extension.
 * \return ::PATH_TYPE_TXT for tilemaps, ::PATH_TYPE_BIN for others.
 */
int path_bin_or_txt( const char* path );

/**
 * \brief Given a file path, strip the directory and extension and write the
 *        resulting basename to the provided header.
 * \param path Path to extract and write the basename from.
 * \param header ::MEMORY_PTR to the header file to write to.
 * \return Number of bytes written.
 */
int path_to_define( const char* path, FILE* header );

/**
 * \brief Write the contents of the given buffer into the provided header as a
 *        hex-encoded constant to be compiled into the deskai engine.
 * \param id Index of the resource to encode in the header.
 * \param header ::MEMORY_PTR to the header file to write to.
 * \return Number of bytes written.
 */
int encode_binary_buffer(
   unsigned char* buffer_in, int buffer_in_sz, const char* res_path,
   int id, FILE* header, int in_fmt, int out_fmt );

struct HEADPACK_DEF* headpack_get_def( const char* filename );

int write_header(
   FILE* header, int paths_in_sz, const char* paths_in[],
   int in_fmt, int out_fmt );

#ifdef HEADPACK_C
struct HEADPACK_DEF g_headpack_defs[HEADPACK_DEFS_MAX];
int g_headpack_defs_sz = 0;
#else
extern struct HEADPACK_DEF g_headpack_defs[];
extern int g_headpack_defs_sz;
#endif /* HEADPACK_C */

#endif /* !HEADPACK_H */

