#include "ministd_memory.h"

#include "ministd_syscall.h"

ptr sbrk(isz n);
ptr
sbrk(isz n)
{
	ptr res;
	_syscall1(__NR_sbrk, res, n);
	return res;
}

ptr mmap_alloc(Allocator ref this, usz n);
ptr mmap_realloc(Allocator ref this, ptr p, usz n);
void mmap_free(Allocator ref this, ptr p);
static Allocator mmap_allocator = (Allocator) {
	.alloc = mmap_alloc,
	.realloc = mmap_realloc,
	.free = mmap_free,
};
typedef char ALIGN[16];
typedef union header {
	struct {
		usz size;
		int is_free;
		union header ref next;
	} s;
	ALIGN stub;
} mmap_header_t;
mmap_header_t ref head, ref tail;

mmap_header_t ref mmap_get_free_block(usz n);
ptr
mmap_alloc(Allocator ref this, usz n)
{
	usz total_size;
	ptr block;
	mmap_header_t ref header;

	if (!n) return 0;

	header = mmap_get_free_block(n);
	if (header) {
		header->s.is_free = 0;
		return (ptr)(header + 1);
	}

	total_size = sizeof(mmap_header_t) + n;
	block = sbrk(total_size);
	if (block == (ptr) -1) {
		return 0;
	}

	header = block;
	header->s.size = n;
	header->s.is_free = 0;
	header->s.next = 0;
	if (!head) head = header;
	if (tail) tail->s.next = header;
	tail = header;

	return (ptr)(header + 1);
}
mmap_header_t ref
mmap_get_free_block(usz n)
{
	mmap_header_t ref res;

	for (res = head; res != 0; res = res->s.next) {
		if (res->s.is_free && res->s.size >= n) return res;
	}

	return res;
}
ptr
mmap_realloc(Allocator ref this, ptr p, usz n)
{
	mmap_header_t ref header;
	ptr ret;
	if (!p || !n) return mmap_alloc(this, n);
	header = (mmap_header_t*)p - 1;
	if (header->s.size >= n) return p;
	ret = mmap_alloc(this, n);
	if (ret) {
		usz i;

		for (i = 0; i < header->s.size; ++i) {
			((char*)ret)[i] = ((char*)p)[i];
		}
		mmap_free(this, p);
	}
	return ret;
}
void
mmap_free(Allocator ref this, ptr p)
{
	mmap_header_t *header, *tmp;
	ptr programbreak;

	if (!p) return;
	header = ((mmap_header_t*)p) - 1;

	programbreak = sbrk(0);
	if ((char*)p + header->s.size == programbreak) {
		if (head == tail) {
			head = tail = 0;
		} else {
			tmp = head;
			while (tmp) {
				if (tmp->s.next == tail) {
					tmp->s.next = 0;
					tail = tmp;
				}
			}
		}
		sbrk(0 - sizeof(mmap_header_t) - header->s.size);
		return;
	}
	header->s.is_free = 1;
}

ptr
alloc(usz n)
{
	return a_alloc(&mmap_allocator, n);
}
ptr
nalloc(usz size, usz n)
{
	return a_nalloc(&mmap_allocator, size, n);
}
ptr
realloc(ptr p, usz n)
{
	return a_realloc(&mmap_allocator, p, n);
}
ptr
nrealloc(ptr p, usz size, usz n)
{
	return a_nrealloc(&mmap_allocator, p, size, n);
}
void
free(ptr p)
{
	return a_free(&mmap_allocator, p);
}

void
memzero(ptr p, usz n)
{
	usz i;

	for (i = 0; i < n; ++i) {
		((char*)p)[i] = 0;
	}
}
void nmemzero(ptr p, usz size, usz n)
{
	usz total_size;

	if (!size || !n) return;
	total_size = size * n;
	if (total_size / n != size) return;
	return memzero(p, total_size);
}

ptr
a_alloc(Allocator ref this, usz n)
{
	return this->alloc(this, n);
}
ptr
a_nalloc(Allocator ref this, usz size, usz n)
{
	usz total_size;

	if (!size || !n) return 0;
	total_size = size * n;
	if (total_size / n != size) return 0;
	return a_alloc(this, total_size);
}
ptr
a_realloc(Allocator ref this, ptr p, usz n)
{
	return this->realloc(this, p, n);
}
ptr
a_nrealloc(Allocator ref this, ptr p, usz size, usz n)
{
	usz total_size;

	if (!size || !n) {
		free(p);
		return 0;
	}
	total_size = size * n;
	if (total_size / n != size) {
		free(p);
		return 0;
	}
	return a_realloc(this, p, total_size);
}
void
a_free(Allocator ref this, ptr p)
{
	return this->free(this, p);
}

ptr stack_alloc(Allocator ref this, usz n);
ptr stack_realloc(Allocator ref this, ptr p, usz n);
void stack_free(Allocator ref this, ptr p);
static Allocator stack_allocator_raw = (Allocator) {
	.alloc = stack_alloc,
	.realloc = stack_realloc,
	.free = stack_free,
};
Allocator ref stack_allocator = &stack_allocator_raw;
ptr
stack_alloc(Allocator ref this, usz n)
{
	ptr res;

	__asm__ volatile(
		"mov %%rsp, %[res];"
		"sub %[n], %%rsp;"
		: [res] "=g" (res)
		: [n] "g" (n)
		: "rsp"
	);
}
ptr
stack_realloc(Allocator ref this, ptr p, usz n)
{
	return 0;
}
void
stack_free(Allocator ref this, ptr p)
{
	return;
}

ptr arena_alloc(Allocator ref this, usz n);
ptr arena_realloc(Allocator ref this, ptr p, usz n);
void arena_free(Allocator ref this, ptr p);
static Allocator arena_vtable = (Allocator) {
	.alloc = arena_alloc,
	.realloc = arena_realloc,
	.free = arena_free,
};
struct ArenaAllocator {
	Allocator vtable;
	ptr buf;
	usz bufsize;
};
ptr
arena_alloc(Allocator ref this, usz n)
{
	return 0;
}
ptr
arena_realloc(Allocator ref this, ptr p, usz n)
{
	return 0;
}
void
arena_free(Allocator ref this, ptr p)
{
	return;
}

Allocator ref a_newarena(ptr buf, usz bufsize)
{
	return 0;
}
