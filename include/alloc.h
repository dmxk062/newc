#pragma once
#include <errno.h>
#include <types.h>

/*
 * Force Allocator
 * Exits program on allocation failure
 */
void* alloc(usize size);

/*
 * Try Allocator
 * Returns NULL on failure
 */
void* talloc(usize size);

errno try_free(void* ptr);
void free(void* ptr);

bool ptr_is_alloc(void* ptr);
