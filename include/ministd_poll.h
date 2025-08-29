#ifndef _RM_STD_POLL_H
#define _RM_STD_POLL_H

#include <_ministd_prelude.h>

#include <asm-generic/poll.h>

enum poll_flag {
	PF_IN = POLLIN,
	PF_PRI = POLLPRI,
	PF_OUT = POLLOUT,
	PF_ERR = POLLERR,
	PF_HUP = POLLHUP,
	PF_NVAL = POLLNVAL,

	PF_RDNORM = POLLRDNORM,
	PF_RDBAND = POLLRDBAND,
	PF_WRNORM = POLLWRNORM,
	PF_WRBAND = POLLWRBAND,
	PF_MSG = POLLMSG,
	PF_REMOVE = POLLREMOVE,
	PF_RDHUP = POLLRDHUP
};

struct pollfd pollfd_new(int fd, enum poll_flag flags);
struct pollfd pollfd_new_from_file(const FILE ref file, enum poll_flag flags,
				   err_t ref err_out);
bool pollfd_has_events(const struct pollfd ref this, enum poll_flag events);

usz poll(struct pollfd ref polls, usz len, int timeout, err_t ref err_out);

typedef struct poll_list_t {
	struct pollfd own polls;
	usz cap;
	usz len;
} poll_list_t;

poll_list_t poll_list_new(err_t ref err_out);
void poll_list_free(poll_list_t ref this);
void poll_list_add(poll_list_t ref this, struct pollfd poll, err_t ref err_out);
void poll_list_add_fd(poll_list_t ref this, int fd, enum poll_flag flags,
		      err_t ref err_out);
void poll_list_add_file(poll_list_t ref this, const FILE ref file,
			enum poll_flag flags, err_t ref err_out);
void poll_list_remove(poll_list_t ref this, usz idx, err_t ref err_out);
usz poll_list_poll(poll_list_t ref this, int timeout, err_t ref err_out);
struct pollfd ref poll_list_first(poll_list_t ref this);
struct pollfd ref poll_list_next(poll_list_t ref this,
				 const struct pollfd ref poll,
				 err_t ref err_out);

#undef POLLIN
#undef POLLPRI
#undef POLLOUT
#undef POLLERR
#undef POLLHUP
#undef POLLNVAL

#undef POLLRDNORM
#undef POLLRDBAND
#undef POLLWRNORM
#undef POLLWRBAND
#undef POLLMSG
#undef POLLREMOVE
#undef POLLRDHUP

#undef POLLFREE
#undef POLL_BUSY_LOOP

#endif /* _RM_STD_POLL_H */
