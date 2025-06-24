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

static isz raw_file_read(struct RAW_FILE ref this, ptr buf, usz cap, err_t ref err_out);
static isz raw_file_write(struct RAW_FILE ref this, const ptr buf, usz cap, err_t ref err_out);
static void raw_file_close(struct RAW_FILE ref this, err_t ref err_out);
static isz raw_file_misc(struct RAW_FILE ref this, enum FILE_OP op, err_t ref err_out);
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
raw_file_read(struct RAW_FILE ref this, ptr buf, usz cap, err_t ref err_out)
{
	return fd_read(this->fd, buf, cap, err_out);
}
static isz
raw_file_write(struct RAW_FILE ref this, const ptr buf, usz cap, err_t ref err_out)
{
	return fd_write(this->fd, buf, cap, err_out);
}
static void
raw_file_close(struct RAW_FILE ref this, err_t ref err_out)
{
	fd_close(this->fd, err_out);
}
static isz
raw_file_misc(struct RAW_FILE ref this, enum FILE_OP op, err_t ref err_out)
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
fd_open(const char ref path, err_t ref err_out)
{
	int res;
	int flags;
	int mode;

	flags = 02; /* read/write */
	mode = 00777; /* permissions of created file (if file created) */

	_syscall3(__NR_open, res, path, flags, mode);

	if (res < 0) {
		ERR_WITH(-res, -1);
	}

	return res;
}
isz
fd_read(int fd, ptr buf, usz cap, err_t ref err_out)
{
	isz lfd, res;
	lfd = fd;

	_syscall3(__NR_read, res, lfd, buf, cap);

	if (res < 0) {
		ERR_WITH(-res, -1);
	}

	return res;
}
isz
fd_write(int fd, const ptr buf, usz cap, err_t ref err_out)
{
	isz lfd, res;
	lfd = fd;

	_syscall3(__NR_write, res, lfd, buf, cap);

	if (res < 0) {
		ERR_WITH(-res, -1);
	}

	return res;
}
void
fd_close(int fd, err_t ref err_out)
{
	isz lfd, res;
	lfd = fd;

	_syscall1(__NR_close, res, lfd);

	SET_ERR(-res);
}

FILE own
open(const char ref path, err_t ref err_out)
{
	err_t err = ERR_OK;
	int fd;

	fd = fd_open(path, &err);
	TRY_WITH(err, NULL);

	return from_fd(fd, err_out);
}
FILE own
from_fd(int fd, err_t ref err_out)
{
	err_t err = ERR_OK;
	struct RAW_FILE own res;

	res = alloc(sizeof(struct RAW_FILE), &err);
	TRY_WITH(err, NULL);
	res->ptrs = raw_file_ptrs;
	res->fd = fd;

	return (FILE own)res;
}
isz
read(FILE ref file, ptr buf, usz cap, err_t ref err_out)
{
	if (!cap) return 0;
	if (file->has_ungot) {
		file->has_ungot = false;
		*(char*)buf = file->ungot;
		return 1+read(file, (char*)buf+1, cap-1, err_out);
	}
	return file->read(file, buf, cap, err_out);
}
isz
write(FILE ref file, const ptr buf, usz cap, err_t ref err_out)
{
	return file->write(file, buf, cap, err_out);
}
void
close(FILE ref file, err_t ref err_out)
{
	file->close(file, err_out);
}
void
ungetc(FILE ref file, char c, err_t ref err_out)
{
	if (file->has_ungot) {
		ERR_VOID(ERR_PERM);
	}
	file->has_ungot = true;
	file->ungot = c;
}
int
peekc(FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	unsigned char c;

	if (read(file, &c, 1, &err) == 0) {
		TRY_WITH(err, -1);
		return -1;
	}
	TRY_WITH(err, -1);
	ungetc(file, c, &err);
	TRY_WITH(err, -1);

	return c;
}

isz
fputs(const char ref str, FILE ref file, err_t ref err_out)
{
	const usz len = strlen(str);

	return write(file, (ptr)str, len, err_out);
}
int
fputc(char c, FILE ref file, err_t ref err_out)
{
	isz bytes_written;

	bytes_written = write(file, &c, 1, err_out);
	if (bytes_written == 1) {
		return c;
	} else if (bytes_written >= 0) {
		ERR_WITH(ERR_IO, -1);
	} else {
		return -1;
	}
}
isz
puts(const char ref str, err_t ref err_out)
{
	err_t err = ERR_OK;
	isz bytes_written;

	bytes_written = fputs(str, stdout, &err);
	TRY_WITH(err, -1);
	if (bytes_written >= 0 && fputc(10, stdout, err_out) == 10) {
		return bytes_written+1;
	} else if (bytes_written >= 0) {
		ERR_WITH(ERR_IO, -1);
	} else {
		return -1;
	}
}
int
putc(char c, err_t ref err_out)
{
	return fputc(c, stdout, err_out);
}

bool
fgets(char ref buf, usz cap, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	isz len = 0;
	int c;
	bool whole_word = false;

	if (buf == NULL || cap == 0) return 0;
	if (cap == 1) {
		*buf = 0;
		return false;
	}

	*buf = 0;
	while ((c = peekc(file, &err)) <= ' ' && c != -1) {
		TRY_WITH(err, false);
		fgetc(file, &err);
		TRY_WITH(err, false);
	}
	if (c == -1) {
		TRY_WITH(err, false);
		return true;
	}

	len = 0;
	for (;;) {
		c = fgetc(file, &err);
		TRY_WITH(err, false);

		buf[len] = c;
		buf[len+1] = 0;
		++len;

		c = peekc(file, &err);
		TRY_WITH(err, false);
		if (c == -1 || c <= ' ') {
			return true;
		}

		if (len == cap-1) {
			return false;
		}
	}
}
int
fgetc(FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	unsigned char res;
	isz bytes_read;

	bytes_read = read(file, &res, 1, &err);
	TRY_WITH(err, -1);
	if (bytes_read < 0) ERR_WITH(ERR_IO, -1);
	if (bytes_read != 1) return -1;

	return res;
}
bool
fgetline(char ref buf, usz cap, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	isz len = 0;
	isz bytes_read;

	if (buf == NULL || cap == 0) return 0;
	if (cap == 1) {
		*buf = 0;
		return false;
	}

	*buf = 0;
	while (len < cap-1) {
		buf[len+1] = 0;
		bytes_read = read(file, buf+len, 1, &err);
		if (err) {
			buf[len] = 0;
			ERR_WITH(err, false);
		}
		if (bytes_read == 0) {
			buf[len] = 0;
			return true;
		}
		++len;
		if (buf[len-1] == '\n') break;
	}
	return len && buf[len-1] == '\n';
}
bool
gets(char ref buf, usz cap, err_t ref err_out)
{
	return fgets(buf, cap, stdin, err_out);
}
char
getc(err_t ref err_out)
{
	return fgetc(stdin, err_out);
}
bool
getline(char ref buf, usz cap, err_t ref err_out)
{
	return fgetline(buf, cap, stdin, err_out);
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
