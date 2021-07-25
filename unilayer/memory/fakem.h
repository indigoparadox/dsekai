
#ifndef MEMORY_FAKEM_H
#define MEMORY_FAKEM_H

struct FAKE_MEMORY_HANDLE;
typedef struct FAKE_MEMORY_HANDLE* MEMORY_HANDLE;

#ifdef ANCIENT_C

typedef char* MEMORY_PTR;
typedef const char* CONST_MEMORY_PTR;
#  ifdef PLATFORM_DOS
typedef char far * MEMORY_FAR_PTR;
typedef const char far * CONST_MEMORY_FAR_PTR;
#  else
typedef char* MEMORY_FAR_PTR;
typedef const char* CONST_MEMORY_FAR_PTR;
#  endif

#else

typedef void* MEMORY_PTR;
typedef const void* CONST_MEMORY_PTR;
#  ifdef PLATFORM_DOS
typedef void far * MEMORY_FAR_PTR;
typedef const void far * CONST_MEMORY_FAR_PTR;
#  else
typedef void* MEMORY_FAR_PTR;
typedef const void* CONST_MEMORY_FAR_PTR;
#  endif

#endif /* ANCIENT_C */


#endif /* !MEMORY_FAKEM_H */

