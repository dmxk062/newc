#pragma once
#include "types.h"

#define ALLOC_PAGE_MIN_SIZE (1024 * 1024 * 8) // allocate 8MB at once at least
#define ALLOC_GC_THRESHOLD 24
#define ALLOC_SPLIT_THRESHOLD 32 // avoid creating uselessly small pages

typedef struct AllocAllocation {
    bool free;
    u64 usable_size;
    u64 tag;

    struct AllocAllocation* next;
    struct AllocAllocation* prev;
} AllocAllocation;

typedef struct AllocPage {
    u64 size;
    u64 usable_size;
    u64 free_size;
    u64 num_allocations;

    bool empty;

    void* start;
    void* end;

    struct AllocPage* next;
    struct AllocPage* prev;
} AllocPage;

typedef struct {
    bool did_initialize;
    u64 num_pages;
    u64 num_allocations;
    u64 num_empty_pages;

    AllocPage* first;
    AllocPage* last;
} AllocHead;

