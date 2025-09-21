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
String own
s_copy(const String ref other, err_t ref err_out)
{
	return s_from_buffer(other->buf, other->len, err_out);
}
void
s_free(String own s)
{
	if (s->buf) free(s->buf);
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
char own
s_to_cstr(String own this, err_t ref err_out)
{
	err_t err = ERR_OK;
	char own res;

	s_push(this, '\0', &err);
	TRY_WITH(err, NULL);

	res = this->buf;
	this->buf = NULL;
	s_free(this);

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
s_shrink(String ref this, usz shrink_by, err_t ref err_out)
{
	if (shrink_by > this->len) ERR_VOID(ERR_OVERFLOW);

	this->len -= shrink_by;
}

const char ref
s_buffer(const String ref this)
{
	return this->buf;
}
char ref
s_mut_buffer(String ref this)
{
	return this->buf;
}
char
s_at(const String ref this, usz idx, err_t ref err_out)
{
	if (idx >= this->len) ERR_WITH(ERR_BOUNDS, '\0');
	return this->buf[idx];
}

void
s_clear(String ref this)
{
	this->len = 0;
}
void
s_set(String ref this, usz idx, char c, err_t ref err_out)
{
	if (idx >= this->len) ERR_VOID(ERR_BOUNDS);
	this->buf[idx] = c;
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
void
s_cappend(String ref this, const char ref cstr, err_t ref err_out)
{
	s_append(this, cstr, strlen(cstr), err_out);
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

StringView own
sv_new(const char ref buf, usz len, err_t ref err_out)
{
	err_t err = ERR_OK;
	StringView own res;

	res = alloc(sizeof(*res), &err);
	TRY_WITH(err, NULL);

	res->buf = buf;
	res->len = len;

	return res;
}
StringView own
sv_from_cstring(const char ref str, err_t ref err_out)
{
	return sv_new(str, strlen(str), err_out);
}
StringView own
sv_from_string(const String ref str, usz idx, err_t ref err_out)
{
	if (idx > s_len(str)) ERR_WITH(ERR_OVERFLOW, NULL);
	return sv_new(&s_buffer(str)[idx], s_len(str) - idx, err_out);
}
StringView own
sv_copy(const StringView ref other, err_t ref err_out)
{
	return sv_new(other->buf, other->len, err_out);
}

char own
sv_cstr(const StringView ref this, err_t ref err_out)
{
	err_t err = ERR_OK;
	char own res;

	res = alloc(this->len + 1, &err);
	TRY_WITH(err, NULL);

	memmove(res, this->buf, this->len);
	res[this->len] = 0;

	return res;
}
char own
sv_to_cstr(StringView own this, err_t ref err_out)
{
	char own res = sv_cstr(this, err_out);
	free(this);
	return res;
}
String own
sv_string(const StringView ref this, err_t ref err_out)
{
	return s_from_buffer(this->buf, this->len, err_out);
}
String own
sv_to_string(StringView own this, err_t ref err_out)
{
	String own res = sv_string(this, err_out);
	free(this);
	return res;
}

usz
sv_len(const StringView ref this)
{
	return this->len;
}
const char ref
sv_buffer(const StringView ref this)
{
	return this->buf;
}
char
sv_at(const StringView ref this, usz idx, err_t ref err_out)
{
	if (idx >= this->len) ERR_WITH(ERR_BOUNDS, '\0');
	return this->buf[idx];
}
void
sv_move(StringView ref this, const char ref new_buf)
{
	this->buf = new_buf;
}
void
sv_set_len(StringView ref this, usz new_len)
{
	this->len = new_len;
}
void
sv_grow(StringView ref this, usz grow_by)
{
	this->len += grow_by;
}
void
sv_shrink(StringView ref this, usz shrink_by, err_t ref err_out)
{
	if (shrink_by > this->len) ERR_VOID(ERR_OVERFLOW);
	this->len -= shrink_by;
}

isz
sv_fprint(const StringView ref this, FILE ref file, err_t ref err_out)
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
sv_print(const StringView ref this, err_t ref err_out)
{ return sv_fprint(this, stdout, err_out); }

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

struct StringViewFile {
	FILE ptrs;
	const StringView ref str;
	usz read_pos;
};

static usz svf_read(StringViewFile ref this, ptr buf, usz cap,
		    err_t ref err_out);
static usz svf_write(StringViewFile ref this, const ptr buf, usz cap,
		    err_t ref err_out);
static void svf_close(StringViewFile ref this, err_t ref err_out);
static usz svf_run(StringViewFile ref this, enum FILE_OP op, err_t ref err_out);
static const FILE svf_ptrs = {
	(FILE_read_t)svf_read,
	(FILE_write_t)svf_write,
	(FILE_close_t)svf_close,
	(FILE_run_t)svf_run,
	0, false,
};

StringViewFile own
svf_open(const StringView ref string_view, err_t ref err_out)
{
	err_t err = ERR_OK;
	StringViewFile own res;

	res = alloc(sizeof(*res), &err);
	TRY_WITH(err, NULL);

	res->ptrs = svf_ptrs;
	res->str = string_view;
	res->read_pos = 0;

	return res;
}

static usz
svf_read(StringViewFile ref this, ptr buf, usz cap, err_t ref err_out)
{
	const usz readable = this->str->len - this->read_pos;
	const usz to_read = readable < cap ? readable : cap;

	(void) err_out;

	memmove(buf, &this->str->buf[this->read_pos], to_read);
	this->read_pos += to_read;

	return to_read;
}
static usz
svf_write(StringViewFile ref this, const ptr buf, usz cap, err_t ref err_out)
{
	(void) this;
	(void) buf;
	(void) cap;

	ERR_WITH(ERR_BADF, 0);
}
static void
svf_close(StringViewFile ref this, err_t ref err_out)
{
	(void) this;
	(void) err_out;
}
static usz
svf_run(StringViewFile ref this, enum FILE_OP op, err_t ref err_out)
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

TEST_FN(string) {
	err_t err = ERR_OK;
	String own greeting;
	String own test;

	greeting = s_new(&err);
	ASSERT(err == ERR_OK, "while creating new string");
	s_cappend(greeting, "Hello, ", &err);
	ASSERT(err == ERR_OK, "while appending string");
	s_cappend(greeting, "world", &err);
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
