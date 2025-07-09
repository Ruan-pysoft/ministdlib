#include <ministd_io.h>

#include <ministd.h>
#include <ministd_error.h>
#include <ministd_memory.h>
#include <ministd_memory.h>
#include <ministd_string.h>
#include <ministd_syscall.h>

FILE ref stdin;
FILE ref stdout;
FILE ref stderr;

struct RAW_FILE {
	FILE ptrs;
	int fd;
};

static usz raw_file_read(struct RAW_FILE ref this, ptr buf, usz cap, err_t ref err_out);
static usz raw_file_write(struct RAW_FILE ref this, const ptr buf, usz cap, err_t ref err_out);
static void raw_file_close(struct RAW_FILE ref this, err_t ref err_out);
static usz raw_file_misc(struct RAW_FILE ref this, enum FILE_OP op, err_t ref err_out);
static FILE raw_file_ptrs = {
	(FILE_read_t)raw_file_read,   /* .read */
	(FILE_write_t)raw_file_write, /* .write */
	(FILE_close_t)raw_file_close, /* .close */
	(FILE_run_t)raw_file_misc,    /* .run */
	0,                            /* .ungot */
	false,                        /* .has_ungot */
};

static struct RAW_FILE raw_stdin;
static struct RAW_FILE raw_stdout;
static struct RAW_FILE raw_stderr;

static usz
raw_file_read(struct RAW_FILE ref this, ptr buf, usz cap, err_t ref err_out)
{
	return fd_read(this->fd, buf, cap, err_out);
}
static usz
raw_file_write(struct RAW_FILE ref this, const ptr buf, usz cap, err_t ref err_out)
{
	return fd_write(this->fd, buf, cap, err_out);
}
static void
raw_file_close(struct RAW_FILE ref this, err_t ref err_out)
{
	fd_close(this->fd, err_out);
}
static usz
raw_file_misc(struct RAW_FILE ref this, enum FILE_OP op, err_t ref err_out)
{
	usz r = 0;

	(void) this;
	(void) err_out;

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
usz
fd_read(int fd, ptr buf, usz cap, err_t ref err_out)
{
	isz lfd, res;
	lfd = fd;

	_syscall3(__NR_read, res, lfd, buf, cap);

	if (res < 0) {
		ERR_WITH(-res, 0);
	}

	return (usz)res;
}
usz
fd_write(int fd, const ptr buf, usz cap, err_t ref err_out)
{
	isz lfd, res;
	lfd = fd;

	_syscall3(__NR_write, res, lfd, buf, cap);

	if (res < 0) {
		ERR_WITH(-res, 0);
	}

	return (usz)res;
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
usz
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
usz
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
flush(FILE ref file, err_t ref err_out)
{
	file->run(file, FO_FLUSH, err_out);
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

usz
fputs(const char ref str, FILE ref file, err_t ref err_out)
{
	const usz len = strlen(str);
	err_t err = ERR_OK;
	usz written = 0;

	while (written < len) {
		isz bytes_written;

		bytes_written = write(file, (ptr)(str + written), len - written, &err);
		TRY_WITH(err, 0);
		if (bytes_written == 0) ERR_WITH(ERR_IO, 0);

		written += bytes_written;
	}

	return written;
}
int
fputc(char c, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz bytes_written;

	bytes_written = write(file, &c, 1, &err);
	TRY_WITH(err, 0);
	if (bytes_written == 1) {
		return c;
	} else {
		ERR_WITH(ERR_IO, 0);
	}
}
usz
puts(const char ref str, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz bytes_written;

	bytes_written = fputs(str, stdout, &err);
	TRY_WITH(err, 0);
	if (fputc(10, stdout, &err) == 10) {
		TRY_WITH(err, 0);
		return bytes_written+1;
	} else {
		TRY_WITH(err, 0);
		ERR_WITH(ERR_IO, 0);
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
	usz len = 0;
	int c;

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
	usz len = 0;
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

struct BufferedFile {
	FILE ptrs;
	FILE ref raw;
	own_ptr read_buf;
	ptr read_buf_pos;
	ptr read_buf_end;
	usz read_cap;
	own_ptr write_buf;
	ptr write_buf_pos;
	ptr write_buf_end;
	usz write_cap;
};

static BufferedFile own buffered_stdin;
static BufferedFile own buffered_stdout;

usz bf_read(BufferedFile ref file, ptr buf, usz cap, err_t ref err_out);
usz bf_write(BufferedFile ref file, const ptr buf, usz cap, err_t ref err_out);
void bf_close(BufferedFile ref file, err_t ref err_out);
usz bf_misc(BufferedFile ref file, enum FILE_OP op, err_t ref err_out);
FILE buffered_file_ptrs = {
	(FILE_read_t)bf_read,   /* read */
	(FILE_write_t)bf_write, /* write */
	(FILE_close_t)bf_close, /* close */
	(FILE_run_t)bf_misc,    /* run */
	0,                      /* ungot */
	false,                  /* has_ungot */
};

#define DEFAULT_BUFSIZE (2 * 1024)

BufferedFile own
bf_new(FILE own file, err_t ref err_out)
{
	err_t err = ERR_OK;
	own_ptr read_buf;
	own_ptr write_buf;

	read_buf = alloc(DEFAULT_BUFSIZE, &err);
	TRY_WITH(err, NULL);
	write_buf = alloc(DEFAULT_BUFSIZE, &err);
	if (err != ERR_OK) {
		free(read_buf);
		ERR_WITH(err, NULL);
	}

	return bf_new_from(
		file, read_buf, DEFAULT_BUFSIZE,
		write_buf, DEFAULT_BUFSIZE, err_out
	);
}
BufferedFile own
bf_new_from(FILE own file, own_ptr read_buf, usz read_cap,
	    own_ptr write_buf, usz write_cap, err_t ref err_out)
{
	err_t err = ERR_OK;
	BufferedFile own res;

	res = alloc(sizeof(BufferedFile), &err);
	TRY_WITH(err, NULL);

	res->ptrs = buffered_file_ptrs;
	res->raw = file;

	res->read_buf = read_buf;
	res->read_buf_pos = read_buf;
	res->read_buf_end = read_buf;
	res->read_cap = read_cap;

	res->write_buf = write_buf;
	res->write_buf_pos = write_buf;
	res->write_buf_end = write_buf;
	res->write_cap = write_cap;

	return res;
}

usz
bf_read(BufferedFile ref file, ptr buf, usz cap, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz to_read;

	if (file->read_buf_pos == file->read_buf_end && cap > file->read_cap) {
		return read(file->raw, buf, cap, err_out);
	}

	if (file->read_buf_pos == file->read_buf_end) {
		usz bytes_read;

		bytes_read = read(file->raw, file->read_buf, file->read_cap, &err);
		TRY_WITH(err, 0);

		if (bytes_read == 0) return 0;

		file->read_buf_pos = file->read_buf;
		file->read_buf_end = (char*)file->read_buf + bytes_read;
	}

	to_read = (char ref)file->read_buf_end - (char ref)file->read_buf_pos;
	if (cap < to_read) to_read = cap;

	memmove(buf, file->read_buf_pos, to_read);
	file->read_buf_pos = (char ref)file->read_buf_pos + to_read;

	return to_read;
}
void
bf_flush(BufferedFile ref file, err_t ref err_out)
{
	err_t err = ERR_OK;

	while (file->write_buf_end > file->write_buf_pos) {
		isz bytes_written;

		bytes_written = write(
			file->raw,
			file->write_buf_pos,
			(char*)file->write_buf_end - (char*)file->write_buf_pos,
			&err
		);
		TRY_VOID(err);

		if (bytes_written == 0) return;

		file->write_buf_pos = (char ref)file->write_buf_pos + bytes_written;
	}

	file->write_buf_pos = file->write_buf;
	file->write_buf_end = file->write_buf;
}
usz
bf_write(BufferedFile ref file, const ptr buf, usz cap, err_t ref err_out)
{
	err_t err = ERR_OK;

	if ((char ref)file->write_buf_end + cap > (char ref)file->write_buf + file->write_cap) {
		bf_flush(file, &err);
		TRY_WITH(err, -1);

		if (file->write_buf_pos < file->write_buf_end) {
			return 0;
		}

		if (cap > file->write_cap) {
			return write(file->raw, buf, cap, err_out);
		} else {
			file->write_buf_pos = file->write_buf;
			file->write_buf_end = (char ref)file->write_buf + cap;
			memmove(file->write_buf, buf, cap);
			return cap;
		}
	} else {
		memmove(file->write_buf_end, buf, cap);
		file->write_buf_end = (char ref)file->write_buf_end + cap;
		return cap;
	}
}
void
bf_close(BufferedFile ref file, err_t ref err_out)
{
	err_t err = ERR_OK;

	bf_flush(file, &err);
	TRY_VOID(err);
	free(file->read_buf);
	free(file->write_buf);
	close(file->raw, err_out);
}
usz
bf_misc(BufferedFile ref file, enum FILE_OP op, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz r = 0;

	switch (op) {
		case FO_FLUSH: {
			bf_flush(file, &err);
			TRY_WITH(err, 0);
		break; }
	}

	return r;
}

static void
ministd_io_cleanup(void)
{
	err_t err = ERR_OK;

	bf_close(buffered_stdin, &err);
	if (err != ERR_OK) {
		perror(err, "Error while closing stdin");
		err = ERR_OK;
	}
	free(buffered_stdin);

	bf_close(buffered_stdout, &err);
	if (err != ERR_OK) {
		perror(err, "Error while closing stdout");
		err = ERR_OK;
	}
	free(buffered_stdout);

	/*
	 * don't really need to close stdin/out/err I don't think,
	 * honestly just doing it bc of the buffers,
	 * so I'm not closing stderr.
	 * Also, closing stderr would make reporting errors
	 * during other cleanup rather difficult...
	 */
}
static usz
stdin_read(struct BufferedFile ref this, ptr buf, usz cap, err_t ref err_out)
{
	/* tie stdin to stdout -- TODO: Allow this with buffered files in general? */
	err_t err = ERR_OK;

	flush(stdout, &err);
	TRY_WITH(err, 0);

	return bf_read(this, buf, cap, err_out);
}
static usz
stderr_write(struct RAW_FILE ref this, ptr buf, usz cap, err_t ref err_out)
{
	/* tie stderr to stdout */
	err_t err = ERR_OK;
	usz res;

	flush(stdout, &err);
	/* no TRY_WITH
	 * write to stderr regardless of if flushing stdout was successful.
	 */

	res = raw_file_write(this, buf, cap, err_out);
	/* manual TRY_WITH here, if flush failed and write didn't */
	if (err != ERR_OK && err_out != NULL && *err_out != ERR_OK) {
		ERR_WITH(err, -1);
	}

	return res;
}
void
ministd_io_init(void)
{
	err_t err = ERR_OK;

	raw_stderr.ptrs = raw_file_ptrs;
	raw_stderr.ptrs.write = (FILE_write_t)stderr_write;
	raw_stderr.fd = 2;
	stderr = (FILE ref)&raw_stderr;

	raw_stdin.ptrs = raw_file_ptrs;
	raw_stdin.fd = 0;
	buffered_stdin = bf_new((FILE*)&raw_stdin, &err);
	if (err != ERR_OK) {
		perror(err, "Error while initialising stdin");
		exit(127);
	}
	buffered_stdin->ptrs.read = (FILE_read_t)stdin_read;
	stdin = (FILE ref)buffered_stdin;

	raw_stdout.ptrs = raw_file_ptrs;
	raw_stdout.fd = 1;
	buffered_stdout = bf_new((FILE*)&raw_stdout, &err);
	if (err != ERR_OK) {
		perror(err, "Error while initialising stdout");
		exit(127);
	}
	stdout = (FILE ref)buffered_stdout;

	atexit(ministd_io_cleanup);
}
