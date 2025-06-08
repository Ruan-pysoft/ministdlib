#include "ministd_string.h"

#include "ministd_memory.h"

usz strlen(s)
	cstr s;
{
	int len;

	for (len = 0; s[len] != 0; ++len);

	return len;
}

String ref s_new()
{
	return s_newalloc(128);
}
void s_free(s)
	String ref s;
{
	if (!s->extbuf) free(s->base);
	free(s);
}
String ref s_newalloc(n)
	usz n;
{
	String ref result;
	result = alloc(sizeof(String));
	if (result == 0) return 0;

	result->base = alloc(n);
	if (result->base == 0) {
		free(result);
		return 0;
	}

	result->end = result->base + n;
	result->ptr = result->base;
	*result->ptr = 0;

	result->extbuf = 0;

	return result;
}
String ref s_array(p, n)
	str p;
	usz n;
{
	String ref result;
	result = alloc(sizeof(String));
	if (result == 0) return 0;

	result->base = (str)p;
	result->end = result->base + strlen(p);
	result->ptr = result->end;

	result->extbuf = 1;

	return result;
}
String ref s_frombuf(b, n)
	ptr b;
usz n;
{
	String ref result;
	result = alloc(sizeof(String));
	if (result == 0) return 0;

	result->base = b;
	result->end = b + n;
	result->ptr = result->end;
	*result->ptr = 0;

	result->extbuf = 1;

	return result;
}
String ref s_grow(s, n)
	String ref s;
	usz n;
{
	const usz cap = s->end - s->base;
	const usz len = s_len(s);

	if (s->extbuf) {
		s_free(s);
		return 0;
	}

	s->base = realloc(s->base, cap+n);
	if (s->base == 0) {
		s_free(s);
		return 0;
	}

	s->end = s->base + cap+n;
	s->ptr = s->base + len;

	return s;
}

void s_putc(s, c)
	String ref s;
	char c;
{
	*(s->ptr++) = c;
	if (s->ptr == s->end) s = s_grow(s, s_len(s));
	*s->ptr = 0;
}
void s_terminate(s)
	String ref s;
{
	*s->ptr = 0;
}
String ref s_reset(s)
	String ref s;
{
	s->ptr = s->base;
	*s->ptr = 0;

	return s;
}
String ref s_restart(s)
	String ref s;
{
	s->ptr = s->base;
}
String ref s_append(s, p)
	String ref s;
	str p;
{
	return s_nappend(s, p, strlen(p));
}
String ref s_nappend(s, p, n)
	String ref s;
	str p;
	usz n;
{
	usz grow_by, i;
	const usz left = (s->end-s->base) - s_len(s);

	grow_by = n+1;
	if (grow_by < (s->end-s->base)) grow_by = s->end-s->base;

	if (left < n+1) s = s_grow(s, grow_by);
	if (s == 0) return s;

	for (i = 0; i < n && p[i] != 0; ++i) {
		*(s->ptr++) = p[i];
	}
	*s->ptr = 0;

	return s;
}
String ref s_memappend(s, p, n)
	String ref s;
	str p;
	usz n;
{
	usz grow_by, i;
	const usz left = (s->end-s->base) - s_len(s);

	grow_by = n+1;
	if (grow_by < (s->end-s->base)) grow_by = s->end-s->base;

	if (left < n+1) s = s_grow(s, grow_by);
	if (s == 0) return s;

	for (i = 0; i < n; ++i) {
		s->ptr[i] = p[i];
	}
	s->ptr += n;
	*s->ptr = 0;

	return s;
}
String ref s_copy(p)
	str p;
{
	String ref result;

	result = s_new();
	return s_append(result, p);
}
String ref s_parse(s1, s2)
	String ref s1;
	String ref s2;
{
	char ref start, ref end;
	usz toklen;

	start = s1->ptr;
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
	if (toklen == 0) return s2;

	s1->ptr = end;

	if (s2->ptr != s2->base) s_putc(s2, ' ');
	return s_memappend(s2, start, end - start);
}
void s_tolower(s)
	String ref s;
{
	char ref p;
	const char lower_bit = 'a' ^ 'A';

	for (p = s->base; p < s->ptr; ++p) {
		*p |= lower_bit;
	}
}

#include "ministd_io.h"

struct StringFile {
	FILE ptrs;
	String ref s;
	usz content_len;
};

isz sf_read(file, buf, cap)
	StringFile ref file;
	void arr buf;
	usz cap;
{
	char ref readtill;
	char ref bufptr;
	const char ref initial = file->s->ptr;

	readtill = file->s->base + file->content_len;
	if (readtill - file->s->ptr > cap) readtill = file->s->ptr+cap;

	for (bufptr; file->s->ptr < readtill; ++file->s->ptr, ++bufptr) {
		*bufptr = *file->s->ptr;
	}

	return file->s->ptr - initial;
}
isz sf_write(file, buf, cap)
	StringFile ref file;
	const void arr buf;
	usz cap;
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
isz sf_misc(file, op)
	StringFile ref file;
	enum FILE_OP op;
{
	isz r;
	r = 0;

	switch (op) {
		case FO_CLOSE: {
			file->s = 0;
			file->content_len = 0;
		break; }
	}

	return r;
}
FILE string_file_ptrs = (FILE) {
	.read = sf_read,
	.write = sf_write,
	.run = sf_misc,
};

StringFile ref sf_open(s)
	String ref s;
{
	StringFile ref result;

	result = alloc(sizeof(StringFile));
	if (result == 0) return 0;

	result->ptrs = string_file_ptrs;
	result->s = s;
	result->content_len = s->ptr - s->base;

	return result;
}
