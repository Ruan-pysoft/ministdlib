#ifndef _RM_STD_FMT_H
#define _RM_STD_FMT_H

#include "ministd_io.h"
#include "ministd_error.h"
#include "ministd_types.h"

#define FPRINT_RAW_DECL(type, suffix) \
	isz fprint##suffix(type suffix, FILE ref file, err_t ref err_out); \
	isz fprint##suffix##b(type suffix, FILE ref file, err_t ref err_out); \
	isz fprint##suffix##x(type suffix, FILE ref file, err_t ref err_out); \
	isz print##suffix(type suffix, err_t ref err_out); \
	isz print##suffix##b(type suffix, err_t ref err_out); \
	isz print##suffix##x(type suffix, err_t ref err_out)

#define FPRINT_DECL(type, suffix) \
	FPRINT_RAW_DECL(type, suffix); \
	FPRINT_RAW_DECL(unsigned type, u##suffix)

isz fprintc(char c, FILE ref file, err_t ref err_out);
isz fprints(const char ref s, FILE ref file, err_t ref err_out);
isz printc(char c, err_t ref err_out);
isz prints(const char ref s, err_t ref err_out);

FPRINT_DECL(long, l);

FPRINT_RAW_DECL(isz, z);
FPRINT_RAW_DECL(usz, uz);

FPRINT_DECL(int, i);

FPRINT_DECL(short, h);

isz fprintp(const ptr p, FILE ref file, err_t ref err_out);
isz printp(const ptr p, err_t ref err_out);

#undef FPRINT_DECL
#undef FPRINT_RAW_DECL

char fscanc(FILE ref file, err_t ref err_out);
char own fscans(FILE ref file, err_t ref err_out);
long fscanl(FILE ref file, err_t ref err_out);
unsigned long fscanul(FILE ref file, err_t ref err_out);
isz fscanz(FILE ref file, err_t ref err_out);
usz fscanuz(FILE ref file, err_t ref err_out);
int fscani(FILE ref file, err_t ref err_out);
unsigned int fscanu(FILE ref file, err_t ref err_out);
short fscanh(FILE ref file, err_t ref err_out);
unsigned short fscanuh(FILE ref file, err_t ref err_out);

#define scanc(err_out)	fscanc(stdin, err_out)
#define scans(err_out)	fscans(stdin, err_out)
#define scanl(err_out)	fscanl(stdin, err_out)
#define scanul(err_out)	fscanul(stdin, err_out)
#define scanz(err_out)	fscanz(stdin, err_out)
#define scanuz(err_out)	fscanuz(stdin, err_out)
#define scani(err_out)	fscani(stdin, err_out)
#define scanu(err_out)	fscanu(stdin, err_out)
#define scanh(err_out)	fscanh(stdin, err_out)
#define scanuh(err_out)	fscanuh(stdin, err_out)

#endif /* _RM_STD_FMT_H */
