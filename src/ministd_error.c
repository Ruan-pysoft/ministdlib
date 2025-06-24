#include "ministd_error.h"

#include "ministd_io.h"

const char ref err_repr(err_t err)
{
	switch (err) {
		case ERR_OK: return "ERR_OK";

		case ERR_PERM: return "ERR_PERM";
		case ERR_NOENT: return "ERR_NOENT";
		case ERR_SRCH: return "ERR_SRCH";
		case ERR_INTR: return "ERR_INTR";
		case ERR_IO: return "ERR_IO";
		case ERR_NXIO: return "ERR_NXIO";
		case ERR_2BIG: return "ERR_2BIG";
		case ERR_NOEXEC: return "ERR_NOEXEC";
		case ERR_BADF: return "ERR_BADF";
		case ERR_CHILD: return "ERR_CHILD";
		case ERR_AGAIN: return "ERR_AGAIN";
		case ERR_NOMEM: return "ERR_NOMEM";
		case ERR_ACCES: return "ERR_ACCES";
		case ERR_FAULT: return "ERR_FAULT";
		case ERR_NOTBLK: return "ERR_NOTBLK";
		case ERR_BUSY: return "ERR_BUSY";
		case ERR_EXIST: return "ERR_EXIST";
		case ERR_XDEV: return "ERR_XDEV";
		case ERR_NODEV: return "ERR_NODEV";
		case ERR_NOTDIR: return "ERR_NOTDIR";
		case ERR_ISDIR: return "ERR_ISDIR";
		case ERR_INVAL: return "ERR_INVAL";
		case ERR_NFILE: return "ERR_NFILE";
		case ERR_MFILE: return "ERR_MFILE";
		case ERR_NOTTY: return "ERR_NOTTY";
		case ERR_TXTBSY: return "ERR_TXTBSY";
		case ERR_FBIG: return "ERR_FBIG";
		case ERR_NOSPC: return "ERR_NOSPC";
		case ERR_SPIPE: return "ERR_SPIPE";
		case ERR_ROFS: return "ERR_ROFS";
		case ERR_MLINK: return "ERR_MLINK";
		case ERR_PIPE: return "ERR_PIPE";
		case ERR_DOM: return "ERR_DOM";
		case ERR_RANGE: return "ERR_RANGE";

		case ERR_OVERFLOW: return "ERR_OVERFLOW";
		case ERR_EOF: return "ERR_EOF";
		case ERR_PARSE: return "ERR_PARSE";
	}

	return "ERR_[UNKNOWN]";
}
const char ref
err_str(err_t err)
{
	switch (err) {
		case ERR_OK: return "No error";

		case ERR_PERM: return "Operation not permitted";
		case ERR_NOENT: return "No such file or directory";
		case ERR_SRCH: return "No such process";
		case ERR_INTR: return "Interrupted system call";
		case ERR_IO: return "I/O error";
		case ERR_NXIO: return "No such device or address";
		case ERR_2BIG: return "Argument list too long";
		case ERR_NOEXEC: return "Exec format error";
		case ERR_BADF: return "Bad file number";
		case ERR_CHILD: return "No child processes";
		case ERR_AGAIN: return "Try again";
		case ERR_NOMEM: return "Out of memory";
		case ERR_ACCES: return "Permission denied";
		case ERR_FAULT: return "Bad address";
		case ERR_NOTBLK: return "Block device required";
		case ERR_BUSY: return "Device or resource busy";
		case ERR_EXIST: return "File exists";
		case ERR_XDEV: return "Cross-device link";
		case ERR_NODEV: return "No such device";
		case ERR_NOTDIR: return "Not a directory";
		case ERR_ISDIR: return "Is a directory";
		case ERR_INVAL: return "Invalid argument";
		case ERR_NFILE: return "File table overflow";
		case ERR_MFILE: return "Too many open files";
		case ERR_NOTTY: return "Not a typewriter";
		case ERR_TXTBSY: return "Text file busy";
		case ERR_FBIG: return "File too large";
		case ERR_NOSPC: return "No space left on device";
		case ERR_SPIPE: return "Illegal seek";
		case ERR_ROFS: return "Read-only file system";
		case ERR_MLINK: return "Too many links";
		case ERR_PIPE: return "Broken pipe";
		case ERR_DOM: return "Math argument out of domain of func";
		case ERR_RANGE: return "Math result not representable";

		case ERR_OVERFLOW: return "Operation would result in overflow";
		case ERR_EOF: return "Hit EOF while parsing";
		case ERR_PARSE: return "Parse error";
	}

	return "unknown error code";
}
void
perror(err_t err, const char ref s)
{
	if (s && *s) {
		fputs(s, stderr, NULL);
		fputs(": ", stderr, NULL);
	}
	fputs(err_str(err), stderr, NULL);
	fputc('\n', stderr, NULL);
}
