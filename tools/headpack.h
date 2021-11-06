
#ifndef HEADPACK_H
#define HEADPACK_H

/*! \file headpack.h
 *  \brief Functions comprising the headpack resource tool.
 */

#include <stdio.h>

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
 * \brief Write the file at the given path into the provided header as a
 *        hex-encoded constant to be compiled into the deskai engine.
 * \param path Path to the file to open and hex encode.
 * \param id Index of the resource to encode in the header.
 * \param header ::MEMORY_PTR to the header file to write to.
 * \return Number of bytes written.
 */
int encode_generic_file( char* path, int id, FILE* header );

/**
 * \brief Write the given ::TILEMAP to a C header file on disk.
 * \param t ::TILEMAP to write to header.
 * \param header_file FILE pointer for header file on disk.
 * \return
 */
int map2h( struct TILEMAP* t, FILE* header_file );

#endif /* !HEADPACK_H */

