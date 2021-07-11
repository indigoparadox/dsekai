
#ifndef MEMORY_WIN16M_H
#define MEMORY_WIN16M_H

typedef HLOCAL MEMORY_HANDLE;

#ifdef ANCIENT_C
typedef char* MEMORY_PTR;
typedef const char* CONST_MEMORY_PTR;
#else
typedef void* MEMORY_PTR;
typedef const void* CONST_MEMORY_PTR;
#endif /* ANCIENT_C */


#endif /* !MEMORY_WIN16M_H */

