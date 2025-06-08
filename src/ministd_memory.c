#include "ministd_memory.h"

#include "ministd_syscall.h"

own_ptr sbrk(isz n);
own_ptr
sbrk(isz n)
{
	own_ptr res;
	_syscall1(__NR_sbrk, res, n);
	return res;
}

own_ptr std_alloc(Allocator ref this, usz bytes);
own_ptr std_realloc(Allocator ref this, own_ptr buf, usz bytes);
void std_free(Allocator ref this, own_ptr buf);
static Allocator std_allocator = (Allocator) {
	.alloc = (Allocator_alloc_t)std_alloc,
	.realloc = (Allocator_realloc_t)std_realloc,
	.free = (Allocator_free_t)std_free,
};
typedef char ALIGN[16];
typedef union header {
	struct {
		usz size;
		int is_free;
		union header ref next;
	} s;
	ALIGN stub;
} std_header_t;
std_header_t ref head, ref tail;

std_header_t ref std_get_free_block(usz bytes);
own_ptr
std_alloc(Allocator ref this, usz bytes)
{
	usz total_size;
	own_ptr block;
	std_header_t ref header;

	if (!bytes) return 0;

	header = std_get_free_block(bytes);
	if (header) {
		header->s.is_free = 0;
		return (ptr)(header + 1);
	}

	total_size = sizeof(std_header_t) + bytes;
	block = sbrk(total_size);
	if (block == (ptr) -1) {
		return 0;
	}

	header = block;
	header->s.size = bytes;
	header->s.is_free = 0;
	header->s.next = 0;
	if (!head) head = header;
	if (tail) tail->s.next = header;
	tail = header;

	return (ptr)(header + 1);
}
std_header_t ref
std_get_free_block(usz bytes)
{
	std_header_t ref res;

	for (res = head; res != 0; res = res->s.next) {
		if (res->s.is_free && res->s.size >= bytes) return res;
	}

	return res;
}
own_ptr
std_realloc(Allocator ref this, own_ptr buf, usz bytes)
{
	std_header_t ref header;
	own_ptr ret;
	if (!buf || !bytes) return std_alloc(this, bytes);
	header = (std_header_t*)buf - 1;
	if (header->s.size >= bytes) return buf;
	ret = std_alloc(this, bytes);
	if (ret) {
		usz i;

		for (i = 0; i < header->s.size; ++i) {
			((char*)ret)[i] = ((char*)buf)[i];
		}
	}
	std_free(this, buf);
	return ret;
}
void
std_free(Allocator ref this, own_ptr buf)
{
	std_header_t ref header, ref tmp;
	ptr programbreak;

	if (!buf) return;
	header = ((std_header_t*)buf) - 1;

	programbreak = sbrk(0);
	if ((char*)buf + header->s.size == programbreak) {
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
		sbrk(0 - sizeof(std_header_t) - header->s.size);
		return;
	}
	header->s.is_free = 1;
}

own_ptr
alloc(usz bytes)
{
	return a_alloc(&std_allocator, bytes);
}
own_ptr
nalloc(usz size, usz n)
{
	return a_nalloc(&std_allocator, size, n);
}
own_ptr
realloc(own_ptr buf, usz bytes)
{
	return a_realloc(&std_allocator, buf, bytes);
}
own_ptr
nrealloc(own_ptr buf, usz size, usz n)
{
	return a_nrealloc(&std_allocator, buf, size, n);
}
void
free(own_ptr buf)
{
	return a_free(&std_allocator, buf);
}

void
memzero(own_ptr buf, usz bytes)
{
	usz i;

	for (i = 0; i < bytes; ++i) {
		((char*)buf)[i] = 0;
	}
}
void
nmemzero(own_ptr buf, usz size, usz n)
{
	usz total_size;

	if (!size || !n) return;
	total_size = size * n;
	if (total_size / n != size) return;
	return memzero(buf, total_size);
}

own_ptr
a_alloc(Allocator ref this, usz bytes)
{
	return this->alloc(this, bytes);
}
own_ptr
a_nalloc(Allocator ref this, usz size, usz n)
{
	usz total_size;

	if (!size || !n) return 0;
	total_size = size * n;
	if (total_size / n != size) return 0;
	return a_alloc(this, total_size);
}
own_ptr
a_realloc(Allocator ref this, own_ptr buf, usz bytes)
{
	return this->realloc(this, buf, bytes);
}
own_ptr
a_nrealloc(Allocator ref this, own_ptr buf, usz size, usz n)
{
	usz total_size;

	if (!size || !n) {
		free(buf);
		return 0;
	}
	total_size = size * n;
	if (total_size / n != size) {
		free(buf);
		return 0;
	}
	return a_realloc(this, buf, total_size);
}
void
a_free(Allocator ref this, own_ptr buf)
{
	return this->free(this, buf);
}

own_ptr stack_alloc(Allocator ref this, usz bytes);
static Allocator stack_allocator_raw = (Allocator) {
	.alloc = (Allocator_alloc_t)stack_alloc,
	.realloc = NULL,
	.free = NULL,
};
Allocator ref stack_allocator = &stack_allocator_raw;
own_ptr
stack_alloc(Allocator ref this, usz bytes)
{
	own_ptr res;

	__asm__ volatile(
		"mov %%rsp, %[res];"
		"sub %[n], %%rsp;"
		: [res] "=g" (res)
		: [n] "g" (bytes)
		: "rsp"
	);

	return res;
}
