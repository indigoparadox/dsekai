
#ifndef RESOURCE_FILE_H
#define RESOURCE_FILE_H

typedef char RESOURCE_ID[64];
typedef MEMORY_HANDLE RESOURCE_BITMAP_HANDLE;
typedef MEMORY_HANDLE RESOURCE_JSON_HANDLE;

#define resource_compare_id( a, b ) (NULL != a && NULL != b  && 0 == memory_strncmp_ptr( a, b, 64 ))
#define resource_assign_id( dest, source ) if( NULL != source ) { memory_strncpy_ptr( dest, source, 64 ); }

#endif /* RESOURCE_FILE_H */

