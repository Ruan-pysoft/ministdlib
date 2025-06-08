#ifndef _RM_STD_MEMORY_H
#define _RM_STD_MEMORY_H

#include "ministd_types.h"

ptr alloc(usz);
ptr nalloc(usz, usz);
ptr realloc(ptr, usz);
ptr nrealloc(ptr, usz, usz);
void free(ptr);

void memzero(ptr, usz);
void nmemzero(ptr, usz, usz);

typedef struct Allocator {
	ptr (*alloc)(struct Allocator ref, usz);
	ptr (*realloc)(struct Allocator ref, ptr, usz);
	void (*free)(struct Allocator ref, ptr);
} Allocator;

ptr a_alloc(Allocator ref, usz);
ptr a_nalloc(Allocator ref, usz, usz);
ptr a_realloc(Allocator ref, ptr, usz);
ptr a_nrealloc(Allocator ref, ptr, usz, usz);
void a_free(Allocator ref, ptr);

extern Allocator ref stack_allocator;

Allocator ref a_newarena(ptr, usz);

#endif /* _RM_STD_MEMORY_H */
