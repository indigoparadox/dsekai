
#ifndef MEMORY_FAKEM_H
#define MEMORY_FAKEM_H

struct FAKE_MEMORY_HANDLE;
typedef struct FAKE_MEMORY_HANDLE* MEMORY_HANDLE;

#ifdef ANCIENT_C
typedef char* MEMORY_PTR;
#else
#  ifdef PLATFORM_DOS
typedef void far * MEMORY_PTR;
#  else
typedef void* MEMORY_PTR;
#  endif
#endif /* ANCIENT_C */


#endif /* !MEMORY_FAKEM_H */

