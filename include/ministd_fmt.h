#ifndef _RM_STD_FMT_H
#define _RM_STD_FMT_H

#include <_ministd_prelude.h>

extern FILE ref stdin;
extern FILE ref stdout;
extern FILE ref stderr;

#define LIST_OF_FMTTYPES_NUMERIC \
	X(h, short) \
	X(uh, unsigned short) \
	X(i, int) \
	X(ui, unsigned int) \
	X(l, long) \
	X(ul, unsigned long) \
	X(z, isz) \
	X(uz, usz)
#define LIST_OF_FMTTYPES_SPECIAL_PRINT \
	X(s, const char ref) \
	X(c, char) \
	X(p, const ptr)
#define LIST_OF_FMTTYPES_SPECIAL_SCAN \
	X(s, char ref) \
	X(c, char)

/* basic fprint* and print* function declarations */
#define X(suff, type) \
	isz fprint ## suff(type suff, FILE ref file, err_t ref err_out); \
	isz print ## suff(type suff, err_t ref err_out);
LIST_OF_FMTTYPES_NUMERIC
LIST_OF_FMTTYPES_SPECIAL_PRINT
#undef X

/* function declarations for hexadecimal and binary printing */
#define X(suff, type) \
	isz fprint ## suff ## b(type suff, FILE ref file, err_t ref err_out); \
	isz fprint ## suff ## x(type suff, FILE ref file, err_t ref err_out); \
	isz print ## suff ## b(type suff, err_t ref err_out); \
	isz print ## suff ## x(type suff, err_t ref err_out);
LIST_OF_FMTTYPES_NUMERIC
#undef X

/* function declarations for fscan* and scan* functions */
#define X(suff, type) \
	type fscan ## suff(FILE ref file, err_t ref err_out); \
	type scan ## suff(err_t ref err_out);
LIST_OF_FMTTYPES_NUMERIC
LIST_OF_FMTTYPES_SPECIAL_SCAN
#undef X

#endif /* _RM_STD_FMT_H */
