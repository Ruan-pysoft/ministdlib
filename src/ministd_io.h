#ifndef _RM_STD_IO_H
#define _RM_STD_IO_H

#include "ministd_types.h"

#define EOF (-1)

enum FILE_OP {
	FO_CLOSE,
};

typedef struct FILE {
	isz (*read)(struct FILE ref, void arr, usz);
	isz (*write)(struct FILE ref, const void arr, usz);
	isz (*run)(struct FILE ref, enum FILE_OP);
} FILE;

extern FILE ref stdin;
extern FILE ref stdout;
extern FILE ref stderr;

isz fd_read(int, void arr, usz); /* file descriptor, buffer, buffer capacity */
isz fd_write(int, const void arr, usz); /* file descriptor, buffer, buffer capacity */
void fd_close(int); /* fd */

isz read(FILE ref, void arr, usz);
isz write(FILE ref, const void arr, usz);
void close(FILE ref);

int fputs(cstr, FILE ref);
int fputc(char, FILE ref);
int puts(cstr);
int putc(char);

void _minilib_io_init(void);

#endif /* _RM_STD_IO_H */
