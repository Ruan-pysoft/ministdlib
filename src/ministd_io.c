#include "ministd_io.h"

#include "ministd.h"

#include <asm/unistd.h>

FILE ref stdin;
FILE ref stdout;
FILE ref stderr;

struct RAW_FILE raw_stdin;
struct RAW_FILE raw_stdout;
struct RAW_FILE raw_stderr;

FILE raw_file_ptrs;

struct RAW_FILE {
	FILE ptrs;
	int fd;
};
isz raw_file_read(file, buf, cap)
	struct RAW_FILE ref file;
	void arr buf;
	usz cap;
{
	return fd_read(file->fd, buf, cap);
}
isz raw_file_write(file, buf, cap)
	struct RAW_FILE ref file;
	const void arr buf;
	usz cap;
{
	return fd_write(file->fd, buf, cap);
}
isz raw_file_misc(file, op)
	struct RAW_FILE ref file;
	enum FILE_OP op;
{
	isz r;
	r = 0;

	switch (op) {
		case FO_CLOSE: {
			fd_close(file->fd);
		break; }
	}

	return r;
}

isz fd_read(fd, buf, cap)
	void arr buf;
	usz cap;
{
	long lfd;
	lfd = fd;

	__asm__ volatile(
		"mov %[fd], %%rdi;" /* file discriptor */
		"mov %[buf], %%rsi;" /* buffer */
		"syscall;"
		: /* outputs */
		: "a" ((long)__NR_read), [fd] "r" (lfd), [buf] "r" (buf), "d" (cap) /* inputs */
		: "rdi", "rsi" /* clobber */
	);
}
isz fd_write(fd, buf, cap)
	const void arr buf;
	usz cap;
{
	long lfd;
	lfd = fd;

	__asm__ volatile(
		"mov %[fd], %%rdi;" /* file discriptor */
		"mov %[buf], %%rsi;" /* buffer */
		"syscall;"
		: /* outputs */
		: "a" ((long)__NR_write), [fd] "r" (lfd), [buf] "r" (buf), "d" (cap) /* inputs */
		: "rdi", "rsi" /* clobber */
	);
}
void fd_close(fd)
{
	long lfd;
	lfd = fd;

	__asm__ volatile(
		"mov %[fd], %%rdi;" /* file descriptor */
		"syscall;"
		: /* outputs */
		: "a" ((long)__NR_close), [fd] "r" (lfd) /* inputs */
		: "rdi" /* clobber */
	);
}

isz read(file, buf, cap)
	FILE ref file;
	void arr buf;
	usz cap;
{
	return file->read(file, buf, cap);
}
isz write(file, buf, cap)
	FILE ref file;
	const void arr buf;
	usz cap;
{
	return file->write(file, buf, cap);
}
void close(file)
	FILE ref file;
{
	file->run(file, FO_CLOSE);
}

int fputs(s, f)
	cstr s;
	FILE ref f;
{
	isz bytes_written;
	const usz len = strlen(s);

	bytes_written = write(f, s, len);
	if (bytes_written != len) {
		return EOF;
	} else {
		return bytes_written;
	}
}
int fputc(c, f)
	char c;
	FILE ref f;
{
	if (write(f, &c, 1) == 1) {
		return c;
	} else {
		return EOF;
	}
}
int puts(s)
	cstr s;
{
	int bytes_written;

	bytes_written = fputs(s, stdout);
	if (bytes_written >= 0 && fputc(10, stdout) == 10) {
		return bytes_written+1;
	} else {
		return EOF;
	}
}
int putc(c)
	char c;
{
	fputc(c, stdout);
}

void _minilib_io_init()
{
	raw_file_ptrs = (FILE) {
		.read = raw_file_read,
		.write = raw_file_write,
		.run = raw_file_misc,
	};

	raw_stdin = (struct RAW_FILE) {
		.ptrs = raw_file_ptrs,
		.fd = 0,
	};
	stdin = (FILE ref)&raw_stdin;

	raw_stdout = (struct RAW_FILE) {
		.ptrs = raw_file_ptrs,
		.fd = 1,
	};
	stdout = (FILE ref)&raw_stdout;

	raw_stderr = (struct RAW_FILE) {
		.ptrs = raw_file_ptrs,
		.fd = 2,
	};
	stderr = (FILE ref)&raw_stderr;
}
