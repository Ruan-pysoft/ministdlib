#ifndef _RM_STD_FMT_H
#define _RM_STD_FMT_H

#include "ministd_io.h"
#include "ministd_error.h"
#include "ministd_types.h"

isz fprintc(char c, FILE ref file, err_t ref err_out);
isz fprints(const char ref s, FILE ref file, err_t ref err_out);
isz fprintl(long l, FILE ref file, err_t ref err_out);
isz fprintul(unsigned long ul, FILE ref file, err_t ref err_out);
isz fprintlb(long l, FILE ref file, err_t ref err_out);
isz fprintulb(unsigned long ul, FILE ref file, err_t ref err_out);
isz fprintlx(long l, FILE ref file, err_t ref err_out);
isz fprintulx(unsigned long ul, FILE ref file, err_t ref err_out);

#define printc(c, err_out)	fprintc(c,   stdout, err_out)
#define prints(s, err_out)	fprints(s,   stdout, err_out)
#define printl(l, err_out)	fprintl(l,   stdout, err_out)
#define printul(ul, err_out)	fprintul(l,  stdout, err_out)
#define printlb(l, err_out)	fprintlb(l,  stdout, err_out)
#define printulb(ul, err_out)	fprintulb(l, stdout, err_out)
#define printlx(l, err_out)	fprintlx(l,  stdout, err_out)
#define printulx(ul, err_out)	fprintulx(l, stdout, err_out)

#define fprintz(z, file, err_out)	fprintl(z,    file, err_out)
#define fprintuz(uz, file, err_out)	fprintul(uz,  file, err_out)
#define fprintzb(z, file, err_out)	fprintlb(z,   file, err_out)
#define fprintuzb(uz, file, err_out)	fprintulb(uz, file, err_out)
#define fprintzx(z, file, err_out)	fprintlx(z,   file, err_out)
#define fprintuzx(uz, file, err_out)	fprintulx(uz, file, err_out)
#define printz(z, err_out)		fprintl(z,    stdout, err_out)
#define printuz(uz, err_out)		fprintul(uz,  stdout, err_out)
#define printzb(z, err_out)		fprintlb(z,   stdout, err_out)
#define printuzb(uz, err_out)		fprintulb(uz, stdout, err_out)
#define printzx(z, err_out)		fprintlx(z,   stdout, err_out)
#define printuzx(uz, err_out)		fprintulx(uz, stdout, err_out)

#define fprinti(i, file, err_out)	fprintl(i,   file, err_out)
#define fprintu(u, file, err_out)	fprintul(u,  file, err_out)
#define fprintib(i, file, err_out)	fprintlb(i,  file, err_out)
#define fprintub(u, file, err_out)	fprintulb(u, file, err_out)
#define fprintix(i, file, err_out)	fprintlx(i,  file, err_out)
#define fprintux(u, file, err_out)	fprintulx(u, file, err_out)
#define printi(i, err_out)		fprintl(i,   stdout, err_out)
#define printu(u, err_out)		fprintul(u,  stdout, err_out)
#define printib(i, err_out)		fprintlb(i,  stdout, err_out)
#define printub(u, err_out)		fprintulb(u, stdout, err_out)
#define printix(i, err_out)		fprintlx(i,  stdout, err_out)
#define printux(u, err_out)		fprintulx(u, stdout, err_out)

#define fprinth(h, file, err_out)	fprintl(h,    file, err_out)
#define fprintuh(uh, file, err_out)	fprintul(uh,  file, err_out)
#define fprinthb(h, file, err_out)	fprintlb(h,   file, err_out)
#define fprintuhb(uh, file, err_out)	fprintulb(uh, file, err_out)
#define fprinthx(h, file, err_out)	fprintlx(h,   file, err_out)
#define fprintuhx(uh, file, err_out)	fprintulx(uh, file, err_out)
#define printh(h, err_out)		fprintl(h,    stdout, err_out)
#define printuh(uh, err_out)		fprintul(uh,  stdout, err_out)
#define printhb(h, err_out)		fprintlb(h,   stdout, err_out)
#define printuhb(uh, err_out)		fprintulb(uh, stdout, err_out)
#define printhx(h, err_out)		fprintlx(h,   stdout, err_out)
#define printuhx(uh, err_out)		fprintulx(uh, stdout, err_out)

#define fprintp(p, file, err_out)	fprintuzx((usz)p, file, err_out)
#define printp(p, file, err_out)	printuzx((usz)p, err_out)

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
