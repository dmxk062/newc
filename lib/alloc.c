#include <errno.h>
#include <mmap.h>
#include <private/alloc.h>
#include <process.h>
#include <types.h>

static AllocHead Head = {
    .did_initialize = false,
    0,
};

static AllocPage* AllocPage_new(u64 size) {
    Result(void_p) new_mapping = mmap(NULL, size, Prot_Read | Prot_Write,
                                     Map_Anonymous | Map_Private, 0, 0);

    if (!new_mapping.ok) {
        return NULL;
    }

    AllocPage* page = new_mapping.val;
    *page = (AllocPage){
        .size = size,
        .usable_size = size - sizeof(AllocPage),
        .free_size = size - sizeof(AllocPage),
        .start = (void*)page + sizeof(AllocPage),
        .end = (void*)page + size,
        .num_allocations = 1,
    };

    AllocAllocation* first_alloc = page->start;
    *first_alloc = (AllocAllocation){
        .free = true,
        .usable_size = page->usable_size - sizeof(AllocAllocation),
        .next = NULL,
        .prev = NULL,
        .tag = 0,
    };

    return page;
}

static u64 AllocPage_get_max_avail_size(AllocPage* page,
                                        AllocAllocation** dest) {
    AllocAllocation* alloc = page->start;
    u64 size = 0;

    while (alloc) {
        if (alloc->free && alloc->usable_size > size) {
            size = alloc->usable_size;
            *dest = alloc;
        }
        alloc = alloc->next;
    }

    return size;
}

static inline u64 get_page_size(u64 size) {
    u64 val = ((size + sizeof(AllocPage) + sizeof(AllocAllocation) +
                ALLOC_PAGE_MIN_SIZE - 1) /
               ALLOC_PAGE_MIN_SIZE) *
              ALLOC_PAGE_MIN_SIZE;
    return val;
}

static errno AllocHead_init(u64 first_page_size) {
    AllocPage* first = AllocPage_new(first_page_size);
    if (!first) {
        return SE_OutOfMem;
    }

    Head.first = first;
    Head.last = first;
    Head.num_pages = 1;
    Head.did_initialize = true;
    return 0;
}

static AllocAllocation*
AllocAllocation_split(AllocPage* page, AllocAllocation* old_alloc, u64 _size) {
    u64 size = (_size + 7) & ~7; // align
    // useless to split here
    if (old_alloc->usable_size == size ||
        old_alloc->usable_size == size + sizeof(AllocAllocation) ||
        old_alloc->usable_size < size + ALLOC_SPLIT_THRESHOLD) {
        return old_alloc;
    }

    u64 alloc_size = size + sizeof(AllocAllocation);
    AllocAllocation* new_alloc = (void*)old_alloc + alloc_size;

    new_alloc->prev = old_alloc;
    new_alloc->next = old_alloc->next;
    if (old_alloc->next) {
        (old_alloc->next)->prev = new_alloc;
    }
    old_alloc->next = new_alloc;

    new_alloc->usable_size = (old_alloc->usable_size - alloc_size);
    old_alloc->usable_size = size;
    new_alloc->free = true;
    new_alloc->tag = 0;

    page->num_allocations++;

    return old_alloc;
}

static void* AllocPage_alloc(AllocPage* page, AllocAllocation* alloc,
                             u64 size) {
    AllocAllocation* new_alloc = AllocAllocation_split(page, alloc, size);
    page->usable_size -= (size + sizeof(AllocAllocation));

    if (page->empty) {
        Head.num_empty_pages--;
    }
    page->empty = false;
    new_alloc->free = false;
    return new_alloc;
}

static AllocAllocation* AllocAllocation_allocate(u64 size) {
    u64 target_size = get_page_size(size);

    if (!Head.did_initialize) {
        errno err = AllocHead_init(target_size);
        if (err) {
            return NULL;
        }
    }

    AllocPage* page = Head.first;
    AllocAllocation* to_split = NULL;

    while (page) {
        if (page->free_size >= size) {
            u64 max_size = AllocPage_get_max_avail_size(page, &to_split);
            if (max_size >= size) {
                break;
            }
        }
        page = page->next;
    }

    if (!to_split) {
        AllocPage* new_page = AllocPage_new(target_size);
        if (!new_page) {
            return NULL;
        }
        Head.last->next = new_page;
        new_page->prev = Head.last;
        Head.last = new_page;
        Head.num_pages++;

        to_split = new_page->start;
        page = new_page;
    }

    Head.num_allocations++;
    return AllocPage_alloc(page, to_split, size);
}

void* alloc(usize size) {
    AllocAllocation* alloc = AllocAllocation_allocate(size);
    if (!alloc) {
        Exit(127);
    }

    return (u8*)alloc + sizeof(AllocAllocation);
}

void* talloc(usize size) {
    AllocAllocation* alloc = AllocAllocation_allocate(size);
    if (!alloc) {
        return NULL;
    }

    return (u8*)alloc + sizeof(AllocAllocation);
}

static errno AllocAllocation_find(void* ptr, AllocPage** dest_page,
                                  AllocAllocation** dest_alloc) {
    AllocPage* page = Head.first;
    bool found = false;

    while (page) {
        if (page->start < ptr && page->end > ptr) {
            found = true;
            break;
        }
        page = page->next;
    }

    if (!found) {
        return -1;
    }
    if (dest_page) {
        *dest_page = page;
    }

    found = false;
    AllocAllocation* alloc = page->start;
    while (alloc) {
        if (ptr >= alloc && ptr < (u8*)alloc + alloc->usable_size) {
            found = true;
            break;
        }
        alloc = alloc->next;
    }

    if (!found) {
        return -2;
    }
    if (dest_alloc) {
        *dest_alloc = alloc;
    }

    return 0;
}

void collect_garbage() {
    AllocPage *page = Head.first, *next = Head.first;

    while (page) {
        next = page->next;
        if (page->empty && page != Head.first) {
            (page->prev)->next = page->next;
            if (page->next) {
                (page->next)->prev = page->prev;
            }
            mmunmap(page, page->size);
            Head.num_empty_pages--;
            Head.num_pages--;
        }

        page = next;
    }
}

errno try_free(void* ptr) {
    AllocPage* page;
    AllocAllocation* to_free;

    errno err = AllocAllocation_find(ptr, &page, &to_free);
    if (err) {
        return -1;
    }

    to_free->free = true;
    to_free->tag = 0;
    page->free_size += to_free->usable_size + sizeof(AllocAllocation);

    if (to_free->prev && (to_free->prev)->free) {
        AllocAllocation* prev = to_free->prev;
        prev->usable_size += to_free->usable_size = sizeof(AllocAllocation);

        prev->next = to_free->next;
        if (to_free->next) {
            (to_free->next)->prev = prev;
        }
        page->num_allocations--;
    }

    if (to_free->next && (to_free->next)->free) {
        AllocAllocation* next = to_free->next;
        to_free->usable_size += next->usable_size + sizeof(AllocAllocation);
        to_free->next = next->next;
        if (next->next) {
            (next->next)->prev = to_free;
        }
        page->num_allocations--;
    }

    if (page->num_allocations == 1 && ((AllocAllocation*)page->start)->free) {
        Head.num_empty_pages++;
        page->empty = true;
    }

    if (Head.num_empty_pages >= ALLOC_GC_THRESHOLD) {
        collect_garbage();
    }

    Head.num_allocations--;
    return SE_Success;
}

void free(void* ptr) {
    try_free(ptr);
}

bool ptr_is_alloc(void* ptr) {
    return !AllocAllocation_find(ptr, NULL, NULL);
}
