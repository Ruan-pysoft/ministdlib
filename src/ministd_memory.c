#include "ministd_memory.h"

#include "ministd_error.h"
#include "ministd_syscall.h"

own_ptr sbrk(isz n, err_t ref err_out);
own_ptr
sbrk(isz n, err_t ref err_out)
{
	ptr increment_to;
	own_ptr res;

	_syscall1(__NR_brk, increment_to, 0); /* get current program break */

	increment_to = (char*)increment_to + n;

	_syscall1(__NR_brk, res, increment_to); /* increment program break */

	if (res == (ptr)-1) {
		/* NOTE: this is my best guess as to what the syscall does
		 * based off of the libc manpages,
		 * but I'm unsure;
		 * TODO: verify */
		ERR_WITH(ERR_NOMEM, NULL);
	}

	return (char*)res - n; /* want old value, not new value */
}

own_ptr std_alloc(Allocator ref this, usz bytes, err_t ref err_out);
own_ptr std_realloc(Allocator ref this, own_ptr buf, usz bytes, err_t ref err_out);
void std_free(Allocator ref this, own_ptr buf);
static Allocator std_allocator = {
	(Allocator_alloc_t)std_alloc,     /* alloc */
	(Allocator_realloc_t)std_realloc, /* realloc */
	(Allocator_free_t)std_free,       /* free */
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
std_alloc(Allocator ref this, usz bytes, err_t ref err_out)
{
	err_t err = ERR_OK;
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
	block = sbrk(total_size, &err);
	TRY_WITH(err, NULL);
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
std_realloc(Allocator ref this, own_ptr buf, usz bytes, err_t ref err_out)
{
	err_t err = ERR_OK;
	std_header_t ref header;
	own_ptr ret;

	if (!buf || !bytes) return std_alloc(this, bytes, err_out);
	header = (std_header_t*)buf - 1;
	if (header->s.size >= bytes) return buf;
	ret = std_alloc(this, bytes, &err);
	TRY_WITH(err, ret);
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

	programbreak = sbrk(0, NULL);
	if (programbreak == NULL) {
		perror(ERR_NOMEM, "Something went wrong when calling free");
		return;
	}
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
				tmp = tmp->s.next;
			}
		}
		if (sbrk(0 - sizeof(std_header_t) - header->s.size, NULL) == NULL) {
			perror(ERR_NOMEM, "Something went wrong when calling free");
			return;
		}
		return;
	}
	header->s.is_free = 1;
}

own_ptr
alloc(usz bytes, err_t ref err_out)
{
	return a_alloc(&std_allocator, bytes, err_out);
}
own_ptr
nalloc(usz size, usz n, err_t ref err_out)
{
	return a_nalloc(&std_allocator, size, n, err_out);
}
own_ptr
realloc(own_ptr buf, usz bytes, err_t ref err_out)
{
	return a_realloc(&std_allocator, buf, bytes, err_out);
}
own_ptr
nrealloc(own_ptr buf, usz size, usz n, err_t ref err_out)
{
	return a_nrealloc(&std_allocator, buf, size, n, err_out);
}
void
free(own_ptr buf)
{
	a_free(&std_allocator, buf);
}

void
memzero(ptr buf, usz bytes)
{
	usz i;

	for (i = 0; i < bytes; ++i) {
		((char ref)buf)[i] = 0;
	}
}
void
nmemzero(ptr buf, usz size, usz n, err_t ref err_out)
{
	usz total_size;

	if (!size || !n) return;
	total_size = size * n;
	if (total_size / n != size) {
		ERR_VOID(ERR_OVERFLOW);
	}
	memzero(buf, total_size);
}
void
memmove(void ref dest, const void ref src, usz n)
{
	usz i;
	for (i = 0; i < n; ++i) {
		((char ref)dest)[i] = ((char ref)src)[i];
	}
}
void
nmemmove(void ref dest, const void ref src, usz size, usz n, err_t ref err_out)
{
	usz total_size;

	if (!size || !n) return;
	total_size = size * n;
	if (total_size / n != size) {
		ERR_VOID(ERR_OVERFLOW);
	}
	memmove(dest, src, total_size);
}

own_ptr
a_alloc(Allocator ref this, usz bytes, err_t ref err_out)
{
	return this->alloc(this, bytes, err_out);
}
own_ptr
a_nalloc(Allocator ref this, usz size, usz n, err_t ref err_out)
{
	usz total_size;

	if (!size || !n) return NULL;
	total_size = size * n;
	if (total_size / n != size) {
		ERR_WITH(ERR_OVERFLOW, NULL);
	}
	return a_alloc(this, total_size, err_out);
}
own_ptr
a_realloc(Allocator ref this, own_ptr buf, usz bytes, err_t ref err_out)
{
	return this->realloc(this, buf, bytes, err_out);
}
own_ptr
a_nrealloc(Allocator ref this, own_ptr buf, usz size, usz n, err_t ref err_out)
{
	usz total_size;

	if (!size || !n) {
		free(buf);
		return NULL;
	}
	total_size = size * n;
	if (total_size / n != size) {
		free(buf);
		ERR_WITH(ERR_OVERFLOW, NULL);
	}
	return a_realloc(this, buf, total_size, err_out);
}
void
a_free(Allocator ref this, own_ptr buf)
{
	this->free(this, buf);
}
