#include "ministd_string.h"

#include "ministd_memory.h"

usz
strlen(const char ref cstr)
{
	usz len;

	for (len = 0; cstr[len] != 0; ++len);

	return len;
}
usz
strnlen(const char ref cstr, usz maxlen)
{
	usz len;

	for (len = 0; cstr[len] != 0 && len < maxlen; ++len);

	return len;
}

String own
s_new(void)
{
	return s_newalloc(128);
}
void
s_free(String own this)
{
	if (this->owned_buffer) free(this->base);
	free(this);
}
String own
s_newalloc(usz n)
{
	String own result;

	if (n == 0) return NULL;

	result = alloc(sizeof(String));
	if (result == NULL) return NULL;

	result->base = alloc(n);
	if (result->base == NULL) {
		free(result);
		return NULL;
	}

	result->end = result->base + n;
	result->ptr = result->base;
	*result->ptr = 0;

	result->owned_buffer = true;

	return result;
}
StringView own
sv_new(const char ref cstr, usz cstr_cap)
{
	StringView own result;
	usz len;

	len = strnlen(cstr, cstr_cap);
	if (len == cstr_cap) return NULL;

	result = alloc(sizeof(StringView));
	if (result == NULL) return NULL;

	result->base = cstr;
	result->end = result->base + cstr_cap;
	result->ptr = result->base + len;

	return result;
}
String own
s_frombuf(ptr buf, usz size)
{
	String own result;

	if (size == 0) return NULL;

	result = alloc(sizeof(String));
	if (result == NULL) return NULL;

	result->base = buf;
	result->end = (char*)buf + size;
	result->ptr = result->end;
	*result->ptr = 0;

	result->owned_buffer = false;

	return result;
}
String own
s_grow(String own this, usz growby)
{
	const usz cap = this->end - this->base;
	const usz len = s_len(this);

	if (!this->owned_buffer) {
		s_free(this);
		return NULL;
	}

	this->base = realloc(this->base, cap+growby);
	if (this->base == NULL) {
		s_free(this);
		return NULL;
	}

	this->end = this->base + cap+growby;
	this->ptr = this->base + len;

	return this;
}

void
s_putc(String ref this, char c)
{
	*(this->ptr++) = c;
	if (this->ptr == this->end) this = s_grow(this, s_len(this));
	if (this != NULL) *this->ptr = 0;
}
void
s_terminate(String ref this)
{
	*this->ptr = 0;
}
String own
s_reset(String own this)
{
	this->ptr = this->base;
	*this->ptr = 0;

	return this;
}
StringView own
s_restart(StringView own this)
{
	this->ptr = this->base;

	return this;
}
String own
s_append(String own this, const char ref str)
{
	return s_nappend(this, str, strlen(str));
}
String own
s_nappend(String own this, const char ref str, usz size)
{
	usz grow_by, i;
	const usz left = (this->end-this->base) - s_len(this);

	grow_by = size+1;
	if (grow_by < (this->end-this->base)) grow_by = this->end-this->base;

	if (left < size+1) this = s_grow(this, grow_by);
	if (this == NULL) return NULL;

	for (i = 0; i < size && str[i] != 0; ++i) {
		*(this->ptr++) = str[i];
	}
	*this->ptr = 0;

	return this;
}
String own
s_memappend(String own this, const char ref buf, usz size)
{
	usz grow_by, i;
	const usz left = (this->end-this->base) - s_len(this);

	grow_by = size+1;
	if (grow_by < (this->end-this->base)) grow_by = this->end-this->base;

	if (left < size+1) this = s_grow(this, grow_by);
	if (this == NULL) return NULL;

	for (i = 0; i < size; ++i) {
		this->ptr[i] = buf[i];
	}
	this->ptr += size;
	*this->ptr = 0;

	return this;
}
String own
s_copy(const char ref str)
{
	String own result;

	result = s_new();
	return s_append(result, str);
}
String own
s_parse(StringView ref from, String own to)
{
	const char ref start, ref end;
	usz toklen;

	start = from->ptr;
	while (*start != 0 && (*start == ' ' || *start == '\t'
		|| *start == '\n')) ++start;

	if (*start == '\'') {
		++start;
		end = start;
		while (*end != 0 && *end != '\'') ++end;
	} else if (*start == '"') {
		++start;
		end = start;
		while (*end != 0 && *end != '"') ++end;
	} else {
		end = start;
		while (*end != 0 && *end != ' ' && *end != '\t'
			&& *end != '\n') ++end;
	}

	toklen = end - start;
	if (toklen == 0) return to;

	from->ptr = end;

	if (to->ptr != to->base) s_putc(to, ' ');
	return s_memappend(to, start, end - start);
}

void
s_tolower(String ref this)
{
	char ref p;
	const char lower_bit = 'a' ^ 'A';

	for (p = this->base; p < this->ptr; ++p) {
		*p |= lower_bit;
	}
}
void
s_toupper(String ref this)
{
	char ref p;
	const char upper_mask = ~('a' ^ 'A');

	for (p = this->base; p < this->ptr; ++p) {
		*p &= upper_mask;
	}
}

#include "ministd_io.h"

struct StringFile {
	FILE ptrs;
	String ref s;
	usz content_len;
};

isz
sf_read(StringFile ref file, ptr buf, usz cap)
{
	char ref readtill;
	char ref bufptr;
	const char ref initial = file->s->ptr;

	readtill = file->s->base + file->content_len;
	if (readtill - file->s->ptr > cap) readtill = file->s->ptr+cap;

	/* TODO: use something like memcpy here */
	for (bufptr = buf; file->s->ptr < readtill; ++file->s->ptr, ++bufptr) {
		*bufptr = *file->s->ptr;
	}

	return file->s->ptr - initial;
}
isz
sf_write(StringFile ref file, const ptr buf, usz cap)
{
	file->s = s_memappend(file->s, (char ref)buf, cap);
	if (file->s == 0) {
		file->content_len = 0;
		return -1;
	}
	if (file->s->ptr - file->s->base > file->content_len) {
		file->content_len = file->s->ptr - file->s->base;
	}

	return cap;
}
void
sf_close(StringFile ref file)
{
	file->s = 0;
	file->content_len = 0;
}
isz
sf_misc(StringFile ref file, enum FILE_OP op)
{
	isz r;
	r = 0;

	switch (op) {
		case FO_FLUSH: {
		break; }
	}

	return r;
}
FILE string_file_ptrs = {
	(FILE_read_t)sf_read,   /* read */
	(FILE_write_t)sf_write, /* write */
	(FILE_close_t)sf_close, /* close */
	(FILE_run_t)sf_misc,    /* run */
};

StringFile own
sf_open(String ref string)
{
	StringFile ref result;

	result = alloc(sizeof(StringFile));
	if (result == 0) return 0;

	result->ptrs = string_file_ptrs;
	result->s = string;
	result->content_len = string->ptr - string->base;

	return result;
}
