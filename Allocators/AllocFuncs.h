#pragma once
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "stdlib.h"
#include <stddef.h>

/* ================================================== */
/*                    enums & types                   */
/* ================================================== */
#define ALLOCATOR_LIST_API static inline

// defines custom malloc, calloc,  realloc, and free ptrs
typedef struct AllocatorFns_t {
    void* (*fn_malloc)(size_t bytes);
    void* (*fn_calloc)(size_t, size_t);
    void* (*fn_realloc)(void*, size_t);
    void (*fn_free)(void*);
} AllocatorFns_t;

ALLOCATOR_LIST_API void AllocatorFns_Init(AllocatorFns_t* allocator, void* (*user_malloc)(size_t),
                                          void* (*user_calloc)(size_t, size_t),
                                          void* (*user_realloc)(void*, size_t),
                                          void (*user_free)(void*)) {
    if (!allocator) return;

    allocator->fn_malloc = user_malloc ? user_malloc : NULL;
    allocator->fn_calloc = user_calloc ? user_calloc : NULL;
    allocator->fn_realloc = user_realloc ? user_realloc : NULL;
    allocator->fn_free = user_free ? user_free : NULL;
}

ALLOCATOR_LIST_API void AllocatorFns_Populate(AllocatorFns_t* pAllocatorsDest,
                                              AllocatorFns_t* pAllocatorsSrc) {
    if (pAllocatorsSrc) {
        // Copy over any provided allocator functions
        pAllocatorsDest->fn_malloc = pAllocatorsSrc->fn_malloc ? pAllocatorsSrc->fn_malloc : malloc;
        pAllocatorsDest->fn_calloc = pAllocatorsSrc->fn_calloc ? pAllocatorsSrc->fn_calloc : calloc;
        pAllocatorsDest->fn_realloc =
            pAllocatorsSrc->fn_realloc ? pAllocatorsSrc->fn_realloc : realloc;
        pAllocatorsDest->fn_free = pAllocatorsSrc->fn_free ? pAllocatorsSrc->fn_free : free;
    } else {
        // Otherwise assign the stdlib ones
        pAllocatorsDest->fn_malloc = malloc;
        pAllocatorsDest->fn_calloc = calloc;
        pAllocatorsDest->fn_realloc = realloc;
        pAllocatorsDest->fn_free = free;
    }
}
