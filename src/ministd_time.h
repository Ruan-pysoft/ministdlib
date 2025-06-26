#ifndef _RM_STD_TIME_H
#define _RM_STD_TIME_H

#include "ministd.h"

struct timespec {
	long tv_sec;
	long tv_nsec;
};

long nanosleep(const struct timespec ref sleep_time,
	       struct timespec ref time_left, err_t ref err_out);

#endif /* _RM_STD_TIME_H */
