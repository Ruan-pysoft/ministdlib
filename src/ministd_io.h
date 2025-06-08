#ifndef _RM_STD_IO_H
#define _RM_STD_IO_H

#include "ministd_types.h"

#define EOF (-1)

enum FILE_OP {
	FO_CLOSE,
};

typedef isz (*FILE_read_t)(struct FILE ref this, ptr buf, usz cap);
typedef isz (*FILE_write_t)(struct FILE ref this, const ptr buf, usz cap);
typedef isz (*FILE_run_t)(struct FILE ref this, enum FILE_OP op);
typedef struct FILE {
	FILE_read_t read;
	FILE_write_t write;
	FILE_run_t run;
} FILE;

extern FILE ref stdin;
extern FILE ref stdout;
extern FILE ref stderr;

isz fd_read(int fd, ptr buf, usz cap);
isz fd_write(int fd, const ptr buf, usz cap);
void fd_close(int fd);

isz read(FILE ref file, ptr buf, usz cap);
isz write(FILE ref file, const ptr buf, usz cap);
void close(FILE ref file);

int fputs(const char ref str, FILE ref file);
int fputc(char c, FILE ref file);
int puts(const char ref str);
int putc(char c);

#endif /* _RM_STD_IO_H */
