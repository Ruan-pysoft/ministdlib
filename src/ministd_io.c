#include "ministd_io.h"

#include "ministd_memory.h"
#include "ministd_syscall.h"
#include "ministd_string.h"

FILE ref stdin;
FILE ref stdout;
FILE ref stderr;

struct RAW_FILE {
	FILE ptrs;
	int fd;
};

static isz raw_file_read(struct RAW_FILE ref this, ptr buf, usz cap);
static isz raw_file_write(struct RAW_FILE ref this, const ptr buf, usz cap);
static void raw_file_close(struct RAW_FILE ref this);
static isz raw_file_misc(struct RAW_FILE ref this, enum FILE_OP op);
static FILE raw_file_ptrs = {
	(FILE_read_t)raw_file_read,   /* .read */
	(FILE_write_t)raw_file_write, /* .write */
	(FILE_close_t)raw_file_close, /* .close */
	(FILE_run_t)raw_file_misc,    /* .run */
};

static struct RAW_FILE raw_stdin;
static struct RAW_FILE raw_stdout;
static struct RAW_FILE raw_stderr;

static isz
raw_file_read(struct RAW_FILE ref this, ptr buf, usz cap)
{
	return fd_read(this->fd, buf, cap);
}
static isz
raw_file_write(struct RAW_FILE ref this, const ptr buf, usz cap)
{
	return fd_write(this->fd, buf, cap);
}
static void
raw_file_close(struct RAW_FILE ref this)
{
	fd_close(this->fd);
}
static isz
raw_file_misc(struct RAW_FILE ref this, enum FILE_OP op)
{
	isz r;
	r = 0;

	switch (op) {
		case FO_FLUSH: {
			/* do nothing */
		break; }
	}

	return r;
}

int
fd_open(const char ref path)
{
	int res;
	int flags;
	int mode;

	flags = 02; /* read/write */
	mode = 00777; /* permissions of created file (if file created) */

	_syscall3(__NR_open, res, path, flags, mode);

	return res;
}
isz
fd_read(int fd, ptr buf, usz cap)
{
	isz lfd, res;
	lfd = fd;

	_syscall3(__NR_read, res, lfd, buf, cap);

	return res;
}
isz
fd_write(int fd, const ptr buf, usz cap)
{
	isz lfd, res;
	lfd = fd;

	_syscall3(__NR_write, res, lfd, buf, cap);

	return res;
}
void
fd_close(int fd)
{
	isz lfd, res;
	lfd = fd;

	_syscall1(__NR_close, res, lfd);
}

FILE own
open(const char ref path)
{
	return from_fd(fd_open(path));
}
FILE own
from_fd(int fd)
{
	struct RAW_FILE own res;
	res = alloc(sizeof(struct RAW_FILE));
	res->ptrs = raw_file_ptrs;
	res->fd = fd;
	return (FILE own)res;
}
isz
read(FILE ref file, ptr buf, usz cap)
{
	return file->read(file, buf, cap);
}
isz
write(FILE ref file, const ptr buf, usz cap)
{
	return file->write(file, buf, cap);
}
void
close(FILE ref file)
{
	file->close(file);
}

int
fputs(const char ref str, FILE ref file)
{
	isz bytes_written;
	const usz len = strlen(str);

	bytes_written = write(file, (ptr)str, len);
	if (bytes_written != len) {
		return EOF;
	} else {
		return bytes_written;
	}
}
int
fputc(char c, FILE ref file)
{
	if (write(file, &c, 1) == 1) {
		return c;
	} else {
		return EOF;
	}
}
int
puts(const char ref str)
{
	int bytes_written;

	bytes_written = fputs(str, stdout);
	if (bytes_written >= 0 && fputc(10, stdout) == 10) {
		return bytes_written+1;
	} else {
		return EOF;
	}
}
int
putc(char c)
{
	return fputc(c, stdout);
}

void
ministd_io_init(void)
{
	raw_stdin.ptrs = raw_file_ptrs;
	raw_stdin.fd = 0;
	stdin = (FILE ref)&raw_stdin;

	raw_stdout.ptrs = raw_file_ptrs;
	raw_stdout.fd = 1;
	stdout = (FILE ref)&raw_stdout;

	raw_stderr.ptrs = raw_file_ptrs;
	raw_stderr.fd = 2;
	stderr = (FILE ref)&raw_stderr;
}
