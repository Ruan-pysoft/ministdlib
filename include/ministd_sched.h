#ifndef _RM_STD_SCHED_H
#define _RM_STD_SCHED_H

#include <_ministd_prelude.h>

#include <linux/sched.h>

#define _syscall_clone3(res, cl_args) \
		_syscall2(__NR_clone3, res, cl_args, sizeof(*cl_args));


usz clone(int (ref fn)(ptr), ptr arg, struct clone_args ref cl_args,
	  err_t ref err_out);

#endif /* _RM_STD_SCHED_H */
