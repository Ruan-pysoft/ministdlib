#include <ministd_memory.h>

#include <ministd_error.h>
#include <ministd_syscall.h>

/* screw it, I'll do this manually later */
#define _SYS_MMAN_H
#include <bits/mman-linux.h>

own_ptr mmap(ptr addr, usz len, int prot, int flags, int fildes, usz off, err_t ref err_out);
own_ptr
mmap(ptr addr, usz len, int prot, int flags, int fildes, usz off, err_t ref err_out)
{
	own_ptr res;

	_syscall6(__NR_mmap, res, addr, len, prot, flags, fildes, off);

	if ((isz)res < 0) {
		ERR_WITH(-(isz)res, NULL);
	}

	return res;
}
void munmap(ptr addr, usz len, err_t ref err_out);
void
munmap(ptr addr, usz len, err_t ref err_out)
{
	long res;

	_syscall2(__NR_munmap, res, addr, len);

	if (res < 0) ERR_VOID(-res);
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
typedef union std_header {
	struct {
		usz size;
		int is_free;
	} s;
	ALIGN stub;
} std_header_t;

own_ptr
std_alloc(Allocator ref this, usz bytes, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz total_size;
	own_ptr block;
	std_header_t ref header;

	(void) this;

	if (!bytes) return NULL;

	total_size = sizeof(std_header_t) + bytes;
	block = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, &err);
	TRY_WITH(err, NULL);
	if (block == NULL) {
		return NULL;
	}

	header = block;
	header->s.size = bytes;
	header->s.is_free = 0;

	return (ptr)(header + 1);
}
own_ptr
std_realloc(Allocator ref this, own_ptr buf, usz bytes, err_t ref err_out)
{
	err_t err = ERR_OK;
	std_header_t ref header;
	own_ptr res;

	if (!buf) return std_alloc(this, bytes, err_out);
	if (!bytes) {
		std_free(this, buf);
		return NULL;
	}
	header = (std_header_t*)buf - 1;
	if (header->s.size >= bytes) return buf;

	res = std_alloc(this, bytes, &err);
	if (err != ERR_OK) {
		std_free(this, buf);
		ERR_WITH(err, NULL);
	}
	if (res) {
		memmove(res, buf, bytes < header->s.size ? bytes : header->s.size);
	}
	std_free(this, buf);

	return res;
}
void
std_free(Allocator ref this, own_ptr buf)
{
	std_header_t ref header;

	(void) this;

	if (!buf) return;

	header = (std_header_t ref)buf - 1;

	munmap((ptr)header, sizeof(std_header_t) + header->s.size, NULL);
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

own_ptr sbrk(isz n, err_t ref err_out);
own_ptr
sbrk(isz n, err_t ref err_out)
{
	ptr increment_to, curr_limit;
	own_ptr res;

	_syscall1(__NR_brk, curr_limit, 0); /* get current program break */

	if (n == 0) return curr_limit;

	increment_to = (char*)curr_limit + n;
	if (increment_to < curr_limit) { /* overflow */
		ERR_WITH(ERR_NOMEM, NULL);
	}

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

own_ptr sbrk_alloc(Allocator ref this, usz bytes, err_t ref err_out);
own_ptr sbrk_realloc(Allocator ref this, own_ptr buf, usz bytes, err_t ref err_out);
void sbrk_free(Allocator ref this, own_ptr buf);
Allocator sbrk_allocator = {
	(Allocator_alloc_t)sbrk_alloc,
	(Allocator_realloc_t)sbrk_realloc,
	(Allocator_free_t)sbrk_free,
};
typedef union sbrk_header {
	struct {
		usz size;
		int is_free;
		union sbrk_header ref next;
	} s;
	ALIGN stub;
} sbrk_header_t;
sbrk_header_t ref sbrk_head, ref sbrk_tail;

/* FIXME: I'm pretty sure there's various bugs lurking in the sbrk_* suite,
 * which I found & patched while translating it to a mmap version,
 * but they're still hanging out in here...
 */
sbrk_header_t ref sbrk_get_free_block(usz bytes);
own_ptr
sbrk_alloc(Allocator ref this, usz bytes, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz total_size;
	own_ptr block;
	sbrk_header_t ref header;

	(void) this;

	if (!bytes) return 0;

	header = sbrk_get_free_block(bytes);
	if (header) {
		header->s.is_free = 0;
		return (ptr)(header + 1);
	}

	total_size = sizeof(sbrk_header_t) + bytes;
	block = sbrk(total_size, &err);
	TRY_WITH(err, NULL);

	header = block;
	header->s.size = bytes;
	header->s.is_free = 0;
	header->s.next = 0;
	if (!sbrk_head) sbrk_head = header;
	if (sbrk_tail) sbrk_tail->s.next = header;
	sbrk_tail = header;

	return (ptr)(header + 1);
}
sbrk_header_t ref
sbrk_get_free_block(usz bytes)
{
	sbrk_header_t ref res;

	for (res = sbrk_head; res != 0; res = res->s.next) {
		if (res->s.is_free && res->s.size >= bytes) return res;
	}

	return res;
}
own_ptr
sbrk_realloc(Allocator ref this, own_ptr buf, usz bytes, err_t ref err_out)
{
	err_t err = ERR_OK;
	sbrk_header_t ref header;
	own_ptr ret;

	if (!buf || !bytes) return sbrk_alloc(this, bytes, err_out);
	header = (sbrk_header_t*)buf - 1;
	if (header->s.size >= bytes) return buf;
	ret = sbrk_alloc(this, bytes, &err);
	TRY_WITH(err, ret);
	if (ret) {
		usz i;

		for (i = 0; i < header->s.size; ++i) {
			((char*)ret)[i] = ((char*)buf)[i];
		}
	}
	sbrk_free(this, buf);

	return ret;
}
void
sbrk_free(Allocator ref this, own_ptr buf)
{
	sbrk_header_t ref header, ref tmp;
	ptr programbreak;

	(void) this;

	if (!buf) return;
	header = ((sbrk_header_t*)buf) - 1;

	programbreak = sbrk(0, NULL);
	if (programbreak == NULL) {
		perror(ERR_NOMEM, "Something went wrong when calling free");
		return;
	}
	if ((char*)buf + header->s.size == programbreak) {
		if (sbrk_head == sbrk_tail) {
			sbrk_head = sbrk_tail = 0;
		} else {
			int counter = 0;

			tmp = sbrk_head;
			while (tmp && counter < (1<<16)) {
				if (tmp->s.next == sbrk_tail) {
					tmp->s.next = 0;
					sbrk_tail = tmp;
				}
				tmp = tmp->s.next;

				/* quick-and-dirty infinite loop protection */
				/* (but you'll probably SEGFAULT before an infinite loop) */
				++counter;
			}
		}
		if (sbrk(0 - sizeof(sbrk_header_t) - header->s.size, NULL) == NULL) {
			perror(ERR_NOMEM, "Something went wrong when calling free");
			return;
		}
		return;
	}
	header->s.is_free = 1;
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
