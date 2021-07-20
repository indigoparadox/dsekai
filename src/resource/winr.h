
#ifndef WINR_H
#define WINR_H

typedef uint32_t RESOURCE_ID;
typedef HBITMAP RESOURCE_BITMAP_HANDLE;

#define resource_compare_id( a, b ) (a == b)
#define resource_assign_id( dest, source ) dest = source

#endif /* WINR_H */

