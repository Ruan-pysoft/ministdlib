#include "ministd_io.h"

#include "ministd_syscall.h"
#include "ministd_string.h"

#include <asm/unistd.h>

FILE ref stdin;
FILE ref stdout;
FILE ref stderr;

struct RAW_FILE {
	FILE ptrs;
	int fd;
};

static isz raw_file_read(struct RAW_FILE ref this, ptr buf, usz cap);
static isz raw_file_write(struct RAW_FILE ref this, const ptr buf, usz cap);
static isz raw_file_misc(struct RAW_FILE ref this, enum FILE_OP op);
static FILE raw_file_ptrs = (FILE) {
	.read = (FILE_read_t)raw_file_read,
	.write = (FILE_write_t)raw_file_write,
	.run = (FILE_run_t)raw_file_misc,
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
static isz
raw_file_misc(struct RAW_FILE ref this, enum FILE_OP op)
{
	isz r;
	r = 0;

	switch (op) {
		case FO_CLOSE: {
			fd_close(this->fd);
		break; }
	}

	return r;
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
	file->run(file, FO_CLOSE);
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
	fputc(c, stdout);
}

void
ministd_io_init(void)
{
	raw_file_ptrs = (FILE) {
		.read = (FILE_read_t)raw_file_read,
		.write = (FILE_write_t)raw_file_write,
		.run = (FILE_run_t)raw_file_misc,
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
