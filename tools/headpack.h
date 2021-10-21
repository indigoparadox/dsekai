
#ifndef HEADPACK_H
#define HEADPACK_H

#include <stdio.h>

int path_bin_or_txt( const char* path );

int path_to_define( const char* path, FILE* header );

void encode_generic_file( char* path, int id, FILE* header );

#endif /* !HEADPACK_H */

