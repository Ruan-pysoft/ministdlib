#include "ministd_time.h"

#include "ministd_error.h"
#include "ministd_syscall.h"

long
nanosleep(const struct timespec ref sleep_time, struct timespec ref time_left,
	  err_t ref err_out)
{
	isz res;

	_syscall2(__NR_nanosleep, res, sleep_time, time_left);

	if (res < 0) {
		ERR_WITH(-res, -1);
	}
	return res;
}
