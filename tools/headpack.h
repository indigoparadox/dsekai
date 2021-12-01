
#ifndef HEADPACK_H
#define HEADPACK_H

/*! \file headpack.h
 *  \brief Functions comprising the headpack resource tool.
 */

#include <stdio.h>

#define HEADPACK_INCLUDE_GUARD "RESEMB_H"

#define HEADPACK_INCLUDES_TABLE( f ) f( ../../src/config.h ) f( ../../src/itstruct.h ) f( ../../src/tmstruct.h )

#define HEADPACK_FNAME_MAX 255

#define HEADPACK_STATE_IN_FMT_ARG 1
#define HEADPACK_STATE_OUT_FMT_ARG 2

struct TILEMAP;

/*! \brief Constant indicating path is to a bitmap (or binary) file. */
#define PATH_TYPE_BIN 0
/*! \brief Constant indicating path is to a tilemap (text) file. */
#define PATH_TYPE_TXT 1

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
   unsigned char* buffer_in, int buffer_in_sz, char* res_path,
   int id, FILE* header, int in_fmt, int out_fmt );

/**
 * \brief Write the given ::TILEMAP to a C header file on disk.
 * \param t ::TILEMAP to write to header.
 * \param header_file FILE pointer for header file on disk.
 * \return
 */
int map2h( struct TILEMAP* t, FILE* header_file );

int write_header(
   FILE* header, int paths_in_sz, char* paths_in[], int in_fmt, int out_fmt );

#endif /* !HEADPACK_H */

