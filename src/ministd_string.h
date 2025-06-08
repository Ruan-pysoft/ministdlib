#ifndef _RM_STD_STRING_H
#define _RM_STD_STRING_H

#include "ministd_types.h"

usz strlen(cstr);

typedef struct String {
	char ref base;
	char ref end;
	char ref ptr;
	int extbuf;
} String;

#define s_to_c(s) ((s)->base)
#define s_len(s) ((s)->ptr - (s)->base)
#define s_clone(s) s_copy((s)->base)

String ref s_new(void);
void s_free(String ref);
String ref s_newalloc(usz);
String ref s_array(str, usz);
String ref s_frombuf(ptr, usz);
String ref s_grow(String ref, usz);

void s_putc(String ref, char);
void s_terminate(String ref);
String ref s_reset(String ref);
String ref s_restart(String ref);
String ref s_append(String ref, str);
String ref s_nappend(String ref, str, usz);
String ref s_memappend(String ref, str, usz);
String ref s_copy(str);
String ref s_parse(String ref, String ref);

void s_tolower(String ref);

typedef struct StringFile StringFile;

StringFile ref sf_open(String ref);

#endif /* _RM_STD_STRING_H */
