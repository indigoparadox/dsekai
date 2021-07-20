
#ifndef RESOURCE_HEADER_H
#define RESOURCE_HEADER_H

typedef uint32_t RESOURCE_ID;
typedef MEMORY_HANDLE RESOURCE_BITMAP_HANDLE;
typedef MEMORY_HANDLE RESOURCE_JSON_HANDLE;

#define resource_compare_id( a, b ) (a == b)
#define resource_assign_id( dest, source ) dest = source

#endif /* RESOURCE_HEADER_H */

