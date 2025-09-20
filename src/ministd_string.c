#include "ministd_error.h"
#include <ministd_string.h>

#include <ministd_io.h>
#include <ministd_memory.h>

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

#define DEFAULT_STRING_SIZE 128

struct String {
	char own buf;
	usz len;
	usz cap;
};

String own
s_new(err_t ref err_out)
{
	return s_with_capacity(DEFAULT_STRING_SIZE, err_out);
}
String own
s_with_capacity(usz cap, err_t ref err_out)
{
	err_t err = ERR_OK;
	String own res;

	res = alloc(sizeof(*res), &err);
	TRY_WITH(err, NULL);

	res->buf = alloc(cap, &err);
	if (err != ERR_OK) {
		free(res);

		ERR_WITH(err, NULL);
	}
	res->cap = cap;
	res->len = 0;

	return res;
}
String own
s_from_buffer(const char ref buf, usz cap, err_t ref err_out)
{
	err_t err = ERR_OK;
	String own res;

	res = s_with_capacity(cap, &err);
	TRY_WITH(err, NULL);

	memmove(res->buf, buf, cap);
	res->len = cap;

	return res;
}
String own
s_from_cstring(const char ref str, err_t ref err_out)
{
	return s_from_buffer(str, strlen(str), err_out);
}
void
s_free(String own s)
{
	free(s->buf);
	free(s);
}

char own
s_cstr(const String ref this, err_t ref err_out)
{
	err_t err = ERR_OK;
	char own res;

	res = alloc(this->len + 1, &err);
	TRY_WITH(err, NULL);

	memmove(res, this->buf, this->len);
	res[this->len] = 0;

	return res;
}

usz
s_len(const String ref this)
{
	return this->len;
}
usz
s_capacity(const String ref this)
{
	return this->cap;
}
void
s_reserve(String ref this, usz cap, err_t ref err_out)
{
	err_t err = ERR_OK;

	if (cap == this->cap) return;

	this->buf = realloc(this->buf, cap, &err);
	TRY_VOID(err);
	this->cap = cap;

	if (this->len > cap) this->len = cap;
}
void
s_grow(String ref this, usz grow_by, err_t ref err_out)
{
	s_reserve(this, this->cap + grow_by, err_out);
}

void
s_clear(String ref this)
{
	this->len = 0;
}
void
s_push(String ref this, char c, err_t ref err_out)
{
	err_t err = ERR_OK;

	if (this->len == this->cap) {
		s_grow(this, this->len, &err);
		TRY_VOID(err);
	}

	this->buf[this->len++] = c;
}
void
s_append(String ref this, const char ref data, usz len, err_t ref err_out)
{
	err_t err = ERR_OK;
	const usz free_bytes = this->cap - this->len;
	const usz over_capacity = len > free_bytes ? len - free_bytes : 0;
	usz grow_by = over_capacity != 0 ? this->cap : 0;

	while (grow_by < over_capacity) grow_by *= 2;

	s_grow(this, grow_by, &err);
	TRY_VOID(err);

	memmove(&this->buf[this->len], data, len);
	this->len += len;
}
void
s_sappend(String ref this, const String ref other, err_t ref err_out)
{
	s_append(this, other->buf, other->len, err_out);
}

isz
s_fprint(const String ref this, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz written = 0;

	while (written < this->len) {
		usz bytes_written;

		bytes_written = write(
			file, (ptr)(this->buf + written), this->len - written,
			&err
		);
		TRY_WITH(err, 0);
		if (bytes_written == 0) ERR_WITH(ERR_IO, 0);

		written += bytes_written;
	}

	return written;
}
isz
s_print(const String ref this, err_t ref err_out)
{ return s_fprint(this, stdout, err_out); }

String own
s_fscan(FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	String own res;

	res = s_new(&err);
	TRY_WITH(err, NULL);

	s_fscan_into(res, file, err_out);
	return res;
}
void
s_fscan_into(String ref this, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;

	if (this->len == this->cap) {
		s_grow(this, this->cap, &err);
		TRY_VOID(err);
	}

	if (fgets(&this->buf[this->len], this->cap - this->len, file, &err)) {
		this->len += strnlen(
			&this->buf[this->len], this->cap - this->len
		);
		return;
	}

	for (;;) {
		const usz oldcap = this->cap;

		TRY_VOID(err);
		this->len = this->cap;

		s_grow(this, this->cap, &err);
		TRY_VOID(err);

		if (fgets(&this->buf[oldcap], oldcap, file, &err)) {
			this->len += strnlen(&this->buf[oldcap], oldcap);
			return;
		}
	}
}
String own
s_scan(err_t ref err_out)
{ return s_fscan(stdin, err_out); }
void
s_scan_into(String ref this, err_t ref err_out)
{ s_fscan_into(this, stdin, err_out); }

struct StringFile {
	FILE ptrs;
	String ref str;
	usz read_pos;
};
struct StringReadFile {
	FILE ptrs;
	const String ref str;
	usz read_pos;
};

static usz srf_read(StringReadFile ref this, ptr buf, usz cap,
		    err_t ref err_out);
static usz sf_write(StringFile ref this, const ptr buf, usz cap,
		    err_t ref err_out);
static usz srf_write(StringReadFile ref this, const ptr buf, usz cap,
		     err_t ref err_out);
static void srf_close(StringReadFile ref this, err_t ref err_out);
static usz srf_run(StringReadFile ref this, enum FILE_OP op, err_t ref err_out);
static const FILE sf_ptrs = {
	(FILE_read_t)srf_read,
	(FILE_write_t)sf_write,
	(FILE_close_t)srf_close,
	(FILE_run_t)srf_run,
	0, false,
};
static const FILE srf_ptrs = {
	(FILE_read_t)srf_read,
	(FILE_write_t)srf_write,
	(FILE_close_t)srf_close,
	(FILE_run_t)srf_run,
	0, false,
};

StringFile own
sf_open(String ref string, err_t ref err_out)
{
	err_t err = ERR_OK;
	StringFile own res;

	res = alloc(sizeof(*res), &err);
	TRY_WITH(err, NULL);

	res->ptrs = sf_ptrs;
	res->str = string;
	res->read_pos = 0;

	return res;
}
StringReadFile own
sf_open_readonly(const String ref string, err_t ref err_out)
{
	err_t err = ERR_OK;
	StringReadFile own res;

	res = alloc(sizeof(*res), &err);
	TRY_WITH(err, NULL);

	res->ptrs = srf_ptrs;
	res->str = string;
	res->read_pos = 0;

	return res;
}

static usz
srf_read(StringReadFile ref this, ptr buf, usz cap, err_t ref err_out)
{
	const usz readable = this->str->cap - this->read_pos;
	const usz to_read = readable < cap ? readable : cap;

	(void) err_out;

	memmove(buf, &this->str->buf[this->read_pos], to_read);
	this->read_pos += to_read;

	return to_read;
}
static usz
sf_write(StringFile ref this, const ptr buf, usz cap, err_t ref err_out)
{
	err_t err = ERR_OK;

	s_append(this->str, buf, cap, &err);
	TRY_WITH(err, 0);

	return cap;
}
static usz
srf_write(StringReadFile ref this, const ptr buf, usz cap, err_t ref err_out)
{
	(void) this;
	(void) buf;
	(void) cap;

	ERR_WITH(ERR_BADF, 0);
}
static void
srf_close(StringReadFile ref this, err_t ref err_out)
{
	(void) this;
	(void) err_out;
}
static usz
srf_run(StringReadFile ref this, enum FILE_OP op, err_t ref err_out)
{
	usz r = 0;

	(void) this;

	switch (op) {
		case FO_FLUSH: break;
		case FO_HASFD: {
			r = false;
		break; }
		case FO_GETFD: {
			ERR_WITH(ERR_BADF, 0);
		break; }
	}

	return r;
}

#ifdef TEST
#include <_ministd_tests.h>

static const char ref hello_world = "Hello, world";

static void
s_append_cstring(String ref this, const char ref str, err_t ref err_out)
{
	s_append(this, str, strlen(str), err_out);
}
TEST_FN(string) {
	err_t err = ERR_OK;
	String own greeting;
	String own test;

	greeting = s_new(&err);
	ASSERT(err == ERR_OK, "while creating new string");
	s_append_cstring(greeting, "Hello, ", &err);
	ASSERT(err == ERR_OK, "while appending string");
	s_append_cstring(greeting, "world", &err);
	ASSERT(err == ERR_OK, "while appending string");
	s_push(greeting, '!', &err);
	s_push(greeting, '\n', &err);
	ASSERT(err == ERR_OK, "while appending character");

	s_fprint(greeting, outf, NULL);

	ASSERT_EQ_MSG(uz, strlen(hello_world), 12, "#1 (strlen)");
	ASSERT_EQ_MSG(uz, strnlen(hello_world, 10), 10, "#2 (strnlen)");
	ASSERT_EQ_MSG(uz, strnlen(hello_world, 20), 12, "#3 (strnlen)");
	test = s_with_capacity(1024, &err);
	ASSERT(err == ERR_OK, "while allocating new string");
	ASSERT_EQ_MSG(uz, s_capacity(test), 1024, "#4 (s_newalloc)");
	s_free(test);

	s_free(greeting);
}
TEST_OUTPUT(string) =
	"Hello, world!\n"
;
#endif
