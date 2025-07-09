#ifndef _RM_STD_IO_H
#define _RM_STD_IO_H

#include <ministd_types.h>
#include <ministd_error.h>

enum FILE_OP {
	FO_FLUSH
};

struct FILE;
typedef isz (*FILE_read_t)(struct FILE ref this, ptr buf, usz cap, err_t ref err_out);
typedef isz (*FILE_write_t)(struct FILE ref this, const ptr buf, usz cap, err_t ref err_out);
typedef void (*FILE_close_t)(struct FILE ref this, err_t ref err_out);
typedef isz (*FILE_run_t)(struct FILE ref this, enum FILE_OP op, err_t ref err_out);
typedef struct FILE {
	FILE_read_t read;
	FILE_write_t write;
	FILE_close_t close;
	FILE_run_t run;
	char ungot;
	bool has_ungot;
} FILE;

extern FILE ref stdin;
extern FILE ref stdout;
extern FILE ref stderr;

int fd_open(const char ref path, err_t ref err_out); /* TODO: flags */
isz fd_read(int fd, ptr buf, usz cap, err_t ref err_out);
isz fd_write(int fd, const ptr buf, usz cap, err_t ref err_out);
void fd_close(int fd, err_t ref err_out);

FILE own open(const char ref path, err_t ref err_out);
FILE own from_fd(int fd, err_t ref err_out);
isz read(FILE ref file, ptr buf, usz cap, err_t ref err_out);
isz write(FILE ref file, const ptr buf, usz cap, err_t ref err_out);
void close(FILE ref file, err_t ref err_out);
void flush(FILE ref file, err_t ref err_out);
void ungetc(FILE ref file, char c, err_t ref err_out);
int peekc(FILE ref file, err_t ref err_out);

isz fputs(const char ref str, FILE ref file, err_t ref err_out);
int fputc(char c, FILE ref file, err_t ref err_out);
isz puts(const char ref str, err_t ref err_out);
int putc(char c, err_t ref err_out);

bool fgets(char ref buf, usz cap, FILE ref file, err_t ref err_out);
int fgetc(FILE ref file, err_t ref err_out);
bool fgetline(char ref buf, usz cap, FILE ref file, err_t ref err_out);
bool gets(char ref buf, usz cap, err_t ref err_out);
char getc(err_t ref err_out);
bool getline(char ref buf, usz cap, err_t ref err_out);

typedef struct BufferedFile BufferedFile;

BufferedFile own bf_new(FILE own file, err_t ref err_out);
BufferedFile own bf_new_from(FILE own file, own_ptr read_buf, usz read_cap,
			     own_ptr write_buf, usz write_cap, err_t ref err_out);

#endif /* _RM_STD_IO_H */
