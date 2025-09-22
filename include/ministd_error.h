#ifndef _RM_STD_ERROR_H
#define _RM_STD_ERROR_H

#include <_ministd_prelude.h>

/* Error type & error handling convenience functions & macros */

/* honestly, idk if this is cheating,
   but in my defence, this isn't libc but kernel headers */
#include <linux/errno.h>

/* convenience macros to work with the err_out argument */
#define SET_ERR(err) do { if (err_out != NULL) *err_out = err; } while (0)
#define ERR_WITH(err, val) do { if (err_out != NULL) *err_out = err; return val; } while (0)
#define ERR_VOID(err) do { if (err_out != NULL) *err_out = err; return; } while (0)
#define TRY_WITH(err, val) do { if (err) ERR_WITH(err, val); } while (0)
#define TRY_VOID(err) do { if (err) ERR_VOID(err); } while (0)

typedef enum err_t {
	ERR_OK		= 0,

	ERR_PERM	= EPERM,	/* 1 -- Operation not permitted  */
	ERR_NOENT	= ENOENT,	/* 2 -- No such file or directory  */
	ERR_SRCH	= ESRCH,	/* 3 -- No such process  */
	ERR_INTR	= EINTR,	/* 4 -- Interrupted system call  */
	ERR_IO		= EIO,		/* 5 -- I/O error  */
	ERR_NXIO	= ENXIO,	/* 6 -- No such device or address  */
	ERR_2BIG	= E2BIG,	/* 7 -- Argument list too long  */
	ERR_NOEXEC	= ENOEXEC,	/* 8 -- Exec format error  */
	ERR_BADF	= EBADF,	/* 9 -- Bad file number  */
	ERR_CHILD	= ECHILD,	/* 10 -- No child processes  */
	ERR_AGAIN	= EAGAIN,	/* 11 -- Try again  */
	ERR_NOMEM	= ENOMEM,	/* 12 -- Out of memory  */
	ERR_ACCES	= EACCES,	/* 13 -- Permission denied  */
	ERR_FAULT	= EFAULT,	/* 14 -- Bad address  */
	ERR_NOTBLK	= ENOTBLK,	/* 15 -- Block device required  */
	ERR_BUSY	= EBUSY,	/* 16 -- Device or resource busy  */
	ERR_EXIST	= EEXIST,	/* 17 -- File exists  */
	ERR_XDEV	= EXDEV,	/* 18 -- Cross-device link  */
	ERR_NODEV	= ENODEV,	/* 19 -- No such device  */
	ERR_NOTDIR	= ENOTDIR,	/* 20 -- Not a directory  */
	ERR_ISDIR	= EISDIR,	/* 21 -- Is a directory  */
	ERR_INVAL	= EINVAL,	/* 22 -- Invalid argument  */
	ERR_NFILE	= ENFILE,	/* 23 -- File table overflow  */
	ERR_MFILE	= EMFILE,	/* 24 -- Too many open files  */
	ERR_NOTTY	= ENOTTY,	/* 25 -- Not a typewriter  */
	ERR_TXTBSY	= ETXTBSY,	/* 26 -- Text file busy  */
	ERR_FBIG	= EFBIG,	/* 27 -- File too large  */
	ERR_NOSPC	= ENOSPC,	/* 28 -- No space left on device  */
	ERR_SPIPE	= ESPIPE,	/* 29 -- Illegal seek  */
	ERR_ROFS	= EROFS,	/* 30 -- Read-only file system  */
	ERR_MLINK	= EMLINK,	/* 31 -- Too many links  */
	ERR_PIPE	= EPIPE,	/* 32 -- Broken pipe  */
	ERR_DOM		= EDOM,		/* 33 -- Math argument out of domain of func  */
	ERR_RANGE	= ERANGE,	/* 34 -- Math result not representable  */

	/* custom errnos */
	ERR_OVERFLOW,			/* operation would cause overflow */
	ERR_EOF,			/* Encountered EOF */
	ERR_PARSE,			/* parse error */
	ERR_ASSERT,			/* assertion error */
	ERR_BOUNDS			/* bounds check failed */

} err_t;

/* error display functions */
const char ref err_repr(err_t err);
const char ref err_str(err_t err);
void perror(err_t err, const char ref s);

#endif /* _RM_STD_ERROR_H */
