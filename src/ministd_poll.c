#include "ministd_error.h"
#include <ministd_poll.h>

#include <ministd_memory.h>
#include <ministd_syscall.h>

struct pollfd
pollfd_new(int fd, enum poll_flag flags)
{
	struct pollfd res;
	res.fd = fd;
	res.events = (short)flags;
	res.revents = 0;
	return res;
}
struct pollfd
pollfd_new_from_file(const FILE ref file, enum poll_flag flags,
		     err_t ref err_out)
{
	err_t err = ERR_OK;
	int fd = run_op((FILE ref)file, FO_GETFD, &err);
	const struct pollfd nullpoll = { 0, 0, 0 };
	TRY_WITH(err, nullpoll);

	return pollfd_new(fd, flags);
}
bool
pollfd_has_events(const struct pollfd ref this, enum poll_flag events)
{
	return !!(this->revents & events);
}

usz
poll(struct pollfd ref polls, usz len, int timeout, err_t ref err_out)
{
	isz res, ltimeout = timeout;

	_syscall3(__NR_poll, res, polls, len, ltimeout);

	if (res < 0) {
		ERR_WITH(-res, 0);
	} else {
		return (usz)res;
	}
}

poll_list_t
poll_list_new(err_t ref err_out)
{
	err_t err = ERR_OK;
	poll_list_t res = { NULL, 0, 0 };

	res.polls = alloc(sizeof(*res.polls) * 16, &err);
	TRY_WITH(err, res);
	res.cap = 16;

	return res;
}
void
poll_list_free(poll_list_t ref this)
{
	free(this->polls);
}
void
poll_list_add(poll_list_t ref this, struct pollfd poll, err_t ref err_out)
{
	err_t err = ERR_OK;

	if (this->len == this->cap) {
		this->polls = realloc(
			this->polls,
			sizeof(*this->polls) * this->cap * 2,
			&err
		);
		TRY_VOID(err);
		this->cap *= 2;
	}

	this->polls[this->len++] = poll;
}
void
poll_list_add_fd(poll_list_t ref this, int fd, enum poll_flag flags,
		 err_t ref err_out)
{
	poll_list_add(this, pollfd_new(fd, flags), err_out);
}
void
poll_list_add_file(poll_list_t ref this, const FILE ref file,
		   enum poll_flag flags, err_t ref err_out)
{
	err_t err = ERR_OK;

	struct pollfd poll = pollfd_new_from_file(file, flags, &err);
	TRY_VOID(err);
	poll_list_add(this, poll, err_out);
}
void
poll_list_remove(poll_list_t ref this, usz idx, err_t ref err_out)
{
	usz i;

	if (idx >= this->len) ERR_VOID(ERR_INVAL);

	--this->len;
	for (i = idx; i < this->len; ++i) {
		this->polls[i] = this->polls[i+1];
	}
}
usz
poll_list_poll(poll_list_t ref this, int timeout, err_t ref err_out)
{
	return poll(this->polls, this->len, timeout, err_out);
}
struct pollfd ref
poll_list_first(poll_list_t ref this)
{
	usz i;

	for (i = 0; i < this->len; ++i) {
		if (this->polls[i].revents != 0) return &this->polls[i];
	}

	return NULL;
}
struct pollfd ref
poll_list_next(poll_list_t ref this, const struct pollfd ref poll,
	       err_t ref err_out)
{
	usz i;

	if (poll < this->polls) ERR_WITH(ERR_INVAL, NULL);
	if (poll >= this->polls + this->len) ERR_WITH(ERR_INVAL, NULL);

	for (i = poll - this->polls + 1; i < this->len; ++i) {
		if (this->polls[i].revents != 0) return &this->polls[i];
	}

	return NULL;
}
