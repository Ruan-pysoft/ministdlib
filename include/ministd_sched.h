#ifndef _RM_STD_SCHED_H
#define _RM_STD_SCHED_H

#include <linux/sched.h>

#include <ministd_error.h>
#include <ministd_io.h>
#include <ministd_types.h>

#define _syscall_clone3(res, cl_args) \
		_syscall2(__NR_clone3, res, cl_args, sizeof(*cl_args));

usz _raw_clone(struct clone_args *cl_args, err_t ref err_out);

#endif /* _RM_STD_SCHED_H */
