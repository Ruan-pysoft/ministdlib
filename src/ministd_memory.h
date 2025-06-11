#ifndef _RM_STD_MEMORY_H
#define _RM_STD_MEMORY_H

#include "ministd_types.h"

own_ptr alloc(usz bytes);
own_ptr nalloc(usz size, usz n);
own_ptr realloc(own_ptr buf, usz bytes);
own_ptr nrealloc(own_ptr buf, usz size, usz n);
void free(own_ptr buf);

void memzero(ptr buf, usz bytes);
void nmemzero(ptr buf, usz size, usz n);

struct Allocator;
typedef own_ptr (*Allocator_alloc_t)(struct Allocator ref this, usz bytes);
typedef own_ptr (*Allocator_realloc_t)(struct Allocator ref this, own_ptr buf, usz bytes);
typedef void (*Allocator_free_t)(struct Allocator ref this, own_ptr buf);
typedef struct Allocator {
	Allocator_alloc_t alloc;
	Allocator_realloc_t realloc;
	Allocator_free_t free;
} Allocator;

own_ptr a_alloc(Allocator ref alloc, usz bytes);
own_ptr a_nalloc(Allocator ref alloc, usz size, usz n);
own_ptr a_realloc(Allocator ref alloc, own_ptr buf, usz bytes);
own_ptr a_nrealloc(Allocator ref alloc, own_ptr buf, usz size, usz n);
void a_free(Allocator ref alloc, ptr buf);

#endif /* _RM_STD_MEMORY_H */
