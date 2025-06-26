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
void millisleep(unsigned int millis, err_t ref err_out)
{
	err_t err = ERR_OK;
	struct timespec time;

	time.tv_sec = millis / 1000;
	time.tv_nsec = (millis % 1000) * 1000 * 1000;

	for (;;) {
		if (nanosleep(&time, &time, &err) == 0) break;
		if (err == ERR_INTR) {
			err = ERR_OK;
		}
		TRY_VOID(err);
	}
}
void sleep(float secs, err_t ref err_out)
{
	err_t err = ERR_OK;
	struct timespec time;

	time.tv_sec = (long)secs;
	time.tv_nsec = (long)((secs - time.tv_sec) * 1000 * 1000 * 1000);
	if (time.tv_nsec == 1000 * 1000 * 1000) {
		++time.tv_sec;
		time.tv_nsec = 0;
	}

	for (;;) {
		if (nanosleep(&time, &time, &err) == 0) break;
		if (err == ERR_INTR) {
			err = ERR_OK;
		}
		TRY_VOID(err);
	}
}
