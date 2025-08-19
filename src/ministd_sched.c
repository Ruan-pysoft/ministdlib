#include <ministd_sched.h>

#include <ministd_syscall.h>

usz
_raw_clone(struct clone_args *cl_args, err_t ref err_out)
{
	isz res;

	_syscall_clone3(res, cl_args);

	if (res < 0) {
		ERR_WITH(-res, 0);
	}

	return res;
}
