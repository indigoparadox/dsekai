
#ifndef MAP2H_H
#define MAP2H_H

/*! \file map2h.h
 *  \brief Functions for writing a parsed map to a header file.
 */

/**
 * \brief Write the given ::TILEMAP to a C header file on disk.
 * \param t ::TILEMAP to write to header.
 * \param header_file FILE pointer for header file on disk.
 * \param map_idx
 * \return
 */
int map2h( struct TILEMAP* t, FILE* header_file, int map_idx );

#endif /* !MAP2H_H */

