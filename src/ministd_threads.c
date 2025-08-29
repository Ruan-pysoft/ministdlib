#include <ministd_threads.h>

#include <ministd_atomic.h>

mutex_t own
mutex_new(err_t ref err_out)
{
	return atomic_new_i(0, err_out);
}
bool
mutex_try_lock(mutex_t ref this)
{
	return atomic_swap_i(this, 1, MO_STRICT) == 0;
}
void
mutex_lock(mutex_t ref this)
{
	while (!mutex_try_lock(this));
}
void
mutex_unlock(mutex_t ref this)
{
	atomic_swap_i(this, 0, MO_STRICT);
}
