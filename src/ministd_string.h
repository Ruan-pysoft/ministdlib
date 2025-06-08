#ifndef _RM_STD_STRING_H
#define _RM_STD_STRING_H

#include "ministd_types.h"

usz strlen(const char ref cstr);
usz strnlen(const char ref cstr, usz maxlen);

/* Guarantees:
 * size of array base >= end-base
 * ptr < end
 * null byte exists before or at end of array base
 */
typedef struct StringView {
	const char ref base; /* view into external string */
	const char ref end;
	const char ref ptr;
} StringView;
/* Guarantees:
 * size of array base >= end-base
 * ptr < end
 * null byte exists before or at end of array base
 */
typedef struct String {
	char *base; /* ownership determined by owned_buffer */
	char ref end;
	char ref ptr;
	bool owned_buffer;
} String;

#define s_to_c(s) ((s)->base)
#define s_len(s) ((s)->ptr - (s)->base)
#define s_clone(s) s_copy((s)->base)
#define s_as_sv(s) ((StringView ref)s)
#define sv_to_c(sv) s_to_c(sv)
#define sv_len(sv) s_len(sv)
#define sv_clone(sv) s_clone(sv)

String own s_new(void);
void s_free(String own this);
String own s_newalloc(usz size);
StringView own sv_new(const char ref cstr, usz cstr_cap);
String own s_frombuf(ptr buf, usz size);
/* WARNING: frees string if called on non-owned buffer */
String own s_grow(String own this, usz growby);

/* TODO: change all the functions that take & return String own
 * to functions that take String ref and returns error enum
 */

void s_putc(String ref this, char c);
void s_terminate(String ref this);
String own s_reset(String own this);
StringView own s_restart(StringView own this);
String own s_append(String own this, const char ref str);
String own s_nappend(String own this, const char ref str, usz size);
String own s_memappend(String own this, const char ref buf, usz size);
String own s_copy(const char ref str);
String own s_parse(StringView ref from, String own to);

void s_tolower(String ref this);
void s_toupper(String ref this);

typedef struct StringFile StringFile;

StringFile own sf_open(String ref string);

#endif /* _RM_STD_STRING_H */
