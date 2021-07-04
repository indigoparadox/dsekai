
#include "../memory.h"

struct MEMORY_BLOCK_HEADER {
   uint16_t offset;
   uint16_t sz;
   uint16_t allocated;
   struct MEMORY_BLOCK_HEADER* prev;
   struct MEMORY_BLOCK_HEADER* next;
};

static uint8_t g_fake_heap[FAKE_HEAP_SIZE];
static uint16_t g_fake_heap_top = 0;
static struct MEMORY_BLOCK_HEADER* g_block_first = NULL;
static const uint16_t gc_sentinal = 1212;

#define MEMORY_BLOCK_REAL_SZ( data_sz ) \
   (sizeof( struct MEMORY_BLOCK_HEADER ) + (data_sz) + sizeof( gc_sentinal ))

void* memory_alloc( uint16_t sz, uint16_t count ) {
   struct MEMORY_BLOCK_HEADER* iter = NULL,
      * last_block = NULL;
   uint16_t new_real_sz = 0,
      new_addr = 0,
      new_top = 0,
      new_sz = 0;

   /* TODO: Detect overflow. */
   new_sz = sz * count;

   if( NULL == g_block_first ) {
      g_block_first = (struct MEMORY_BLOCK_HEADER*)g_fake_heap;
      g_block_first->prev = NULL;
      g_block_first->next = NULL;
      g_block_first->offset = 0;
      g_block_first->allocated = 1;
      iter = g_block_first;
   } else {
      iter = g_block_first;
      while( NULL != iter->next ) {
         iter = iter->next;
      }

      new_addr = iter->offset + MEMORY_BLOCK_REAL_SZ( iter->sz );
      new_top = new_addr + MEMORY_BLOCK_REAL_SZ( new_sz );
      if( new_top >= FAKE_HEAP_SIZE ) {
         /* Not enough memory. */
         iter = NULL;
      } else {
         iter->next = (struct MEMORY_BLOCK_HEADER*)&(g_fake_heap[new_addr]);
         last_block = iter;
         iter = iter->next;
         iter->prev = last_block;
         iter->offset = new_addr;

         /* TODO: Place/check sentinal. */
      }
   }

   if( NULL != iter ) {
      iter->sz = new_sz;
      iter->allocated = 1;
      g_fake_heap_top = iter->offset + MEMORY_BLOCK_REAL_SZ( iter->sz );
   }

   return iter;
}

void memory_free( void** ptr_ptr ) {
   if( NULL == ptr_ptr ) {
      return;
   }

   if( NULL == *ptr_ptr ) {
      return;
   }

   /* TODO */

   *ptr_ptr = NULL;
}

void* memory_realloc( void* ptr, uint16_t sz ) {
   /* TODO */
   return NULL;
}

