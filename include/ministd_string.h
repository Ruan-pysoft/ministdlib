#ifndef _RM_STD_STRING_H
#define _RM_STD_STRING_H

#include <_ministd_prelude.h>

usz strlen(const char ref cstr);
usz strnlen(const char ref cstr, usz maxlen);

typedef struct String String;

String own s_new(err_t ref err_out);
String own s_with_capacity(usz cap, err_t ref err_out);
String own s_from_buffer(const char ref buf, usz cap, err_t ref err_out);
String own s_from_cstring(const char ref str, err_t ref err_out);
String own s_copy(const String ref other, err_t ref err_out);
void s_free(String own s);

char own s_cstr(const String ref this, err_t ref err_out);
char own s_to_cstr(String own this, err_t ref err_out);

usz s_len(const String ref this);
usz s_capacity(const String ref this);
void s_reserve(String ref this, usz cap, err_t ref err_out);
void s_grow(String ref this, usz grow_by, err_t ref err_out);
void s_shrink(String ref this, usz shrink_by, err_t ref err_out);

const char ref s_buffer(const String ref this);
char ref s_mut_buffer(String ref this);
char s_at(const String ref this, usz idx, err_t ref err_out);

void s_clear(String ref this);
void s_set(String ref this, usz idx, char c, err_t ref err_out);
void s_push(String ref this, char c, err_t ref err_out);
void s_append(String ref this, const char ref data, usz len, err_t ref err_out);
void s_sappend(String ref this, const String ref other, err_t ref err_out);
void s_cappend(String ref this, const char ref cstr, err_t ref err_out);

isz s_fprint(const String ref this, FILE ref file, err_t ref err_out);
isz s_print(const String ref this, err_t ref err_out);

String own s_fscan(FILE ref file, err_t ref err_out);
void s_fscan_into(String ref this, FILE ref file, err_t ref err_out);
String own s_scan(err_t ref err_out);
void s_scan_into(String ref this, err_t ref err_out);

typedef struct StringView {
	const char ref buf;
	usz len;
} StringView;

StringView own sv_new(const char ref buf, usz len, err_t ref err_out);
StringView own sv_from_cstring(const char ref str, err_t ref err_out);
StringView own sv_from_string(const String ref str, usz idx, err_t ref err_out);
StringView own sv_copy(const StringView ref other, err_t ref err_out);

char own sv_cstr(const StringView ref this, err_t ref err_out);
char own sv_to_cstr(StringView own this, err_t ref err_out);
String own sv_string(const StringView ref this, err_t ref err_out);
String own sv_to_string(StringView own this, err_t ref err_out);

usz sv_len(const StringView ref this);
const char ref sv_buffer(const StringView ref this);
char sv_at(const StringView ref this, usz idx, err_t ref err_out);
void sv_move(StringView ref this, const char ref new_buf);
void sv_set_len(StringView ref this, usz new_len);
void sv_grow(StringView ref this, usz grow_by);
void sv_shrink(StringView ref this, usz shrink_by, err_t ref err_out);

isz sv_fprint(const StringView ref this, FILE ref file, err_t ref err_out);
isz sv_print(const StringView ref this, err_t ref err_out);

typedef struct StringFile StringFile;
typedef struct StringReadFile StringReadFile;

StringFile own sf_open(String ref string, err_t ref err_out);
StringReadFile own sf_open_readonly(const String ref string, err_t ref err_out);

typedef struct StringViewFile StringViewFile;

StringViewFile own svf_open(const StringView ref string_view, err_t ref err_out);

#endif /* _RM_STD_STRING_H */
