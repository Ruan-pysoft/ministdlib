#ifndef _RM_STD_STRING_H
#define _RM_STD_STRING_H

#include <_ministd_prelude.h>

usz strlen(const char ref cstr);
usz strnlen(const char ref cstr, usz maxlen);

typedef struct String String;

String own s_new(err_t ref err_out);
String own s_with_capacity(usz cap, err_t ref err_out);
void s_free(String own s);

char own s_cstr(const String ref this, err_t ref err_out);

usz s_len(const String ref this);
usz s_capacity(const String ref this);
void s_reserve(String ref this, usz cap, err_t ref err_out);
void s_grow(String ref this, usz grow_by, err_t ref err_out);

void s_clear(String ref this);
void s_push(String ref this, char c, err_t ref err_out);
void s_append(String ref this, const char ref data, usz len, err_t ref err_out);
void s_sappend(String ref this, const String ref other, err_t ref err_out);

isz s_fprint(const String ref this, FILE ref file, err_t ref err_out);
isz s_print(const String ref this, err_t ref err_out);

typedef struct StringFile StringFile;
typedef struct StringReadFile StringReadFile;

StringFile own sf_open(String ref string, err_t ref err_out);
StringReadFile own sf_open_readonly(const String ref string, err_t ref err_out);

#endif /* _RM_STD_STRING_H */
