#ifndef _RM_STD_MEMORY_H
#define _RM_STD_MEMORY_H

#include "ministd_types.h"
#include "ministd_error.h"

own_ptr alloc(usz bytes, err_t ref err_out);
own_ptr nalloc(usz size, usz n, err_t ref err_out);
own_ptr realloc(own_ptr buf, usz bytes, err_t ref err_out);
own_ptr nrealloc(own_ptr buf, usz size, usz n, err_t ref err_out);
void free(own_ptr buf);

void memzero(ptr buf, usz bytes);
void nmemzero(ptr buf, usz size, usz n, err_t ref err_out);
void memmove(void ref dest, const void ref src, usz n);
void nmemmove(void ref dest, const void ref src, usz size, usz n, err_t ref err_out);

struct Allocator;
typedef own_ptr (*Allocator_alloc_t)(struct Allocator ref this, usz bytes, err_t ref err_out);
typedef own_ptr (*Allocator_realloc_t)(struct Allocator ref this, own_ptr buf, usz bytes, err_t ref err_out);
typedef void (*Allocator_free_t)(struct Allocator ref this, own_ptr buf);
typedef struct Allocator {
	Allocator_alloc_t alloc;
	Allocator_realloc_t realloc;
	Allocator_free_t free;
} Allocator;

own_ptr a_alloc(Allocator ref alloc, usz bytes, err_t ref err_out);
own_ptr a_nalloc(Allocator ref alloc, usz size, usz n, err_t ref err_out);
own_ptr a_realloc(Allocator ref alloc, own_ptr buf, usz bytes, err_t ref err_out);
own_ptr a_nrealloc(Allocator ref alloc, own_ptr buf, usz size, usz n, err_t ref err_out);
void a_free(Allocator ref alloc, ptr buf);

#endif /* _RM_STD_MEMORY_H */
