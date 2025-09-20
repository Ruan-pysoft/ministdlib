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

#ifdef TEST
#include <_ministd_tests.h>
#include <ministd_memory.h>
#include <ministd_time.h>
#include <ministd_sched.h>

#define N_THREADS 10

bool has_executed[N_THREADS] = {0};
volatile usz num_executed = 0;

struct thread_fn_arg {
	mutex_t ref lock;
	int thread_id;
};

static int
TEST_FN_NAME(mutex_helper)(void ref void_arg) {
	struct thread_fn_arg ref arg = void_arg;

	mutex_lock(arg->lock);
	fprints("Acquired lock in thread ", stderr, NULL);
	fprinti(arg->thread_id, stderr, NULL);
	fprints("!\n", stderr, NULL);

	++num_executed;
	has_executed[arg->thread_id] = true;

	sleep(0.1, NULL);
	mutex_unlock(arg->lock);

	return 0;
}

TEST_FN(mutex) {
	err_t err = ERR_OK;
	mutex_t own lock = mutex_new(&err);
	struct clone_args cl_args = { 0 };
	usz i;
	TRY_VOID(err);

	cl_args.flags = CLONE_VM | CLONE_THREAD | CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_SIGHAND | CLONE_SYSVSEM;

	for (i = 0; i < N_THREADS; ++i) {
		own_ptr stack = alloc(256, NULL);
		struct thread_fn_arg own arg = alloc(sizeof(struct thread_fn_arg), NULL);
		cl_args.stack = (usz)stack;
		cl_args.stack_size = 256;
		arg->lock = lock;
		arg->thread_id = i;

		OUT(s, "Launching thread #");
		OUT(uz, i);
		OUT(s, "...\n");

		clone(TEST_FN_NAME(mutex_helper), arg, &cl_args, NULL);
	}

	while (num_executed < N_THREADS) {}
	
	OUT(s, "All threads have executed! Checking...\n");

	for (i = 0; i < N_THREADS; ++i) {
		ASSERT(has_executed[i], "thread didn't execute");
	}

	OUT(s, "All threads have executed.\n");
}
TEST_OUTPUT(mutex) =
	"Launching thread #0...\n"
	"Launching thread #1...\n"
	"Launching thread #2...\n"
	"Launching thread #3...\n"
	"Launching thread #4...\n"
	"Launching thread #5...\n"
	"Launching thread #6...\n"
	"Launching thread #7...\n"
	"Launching thread #8...\n"
	"Launching thread #9...\n"
	"All threads have executed! Checking...\n"
	"All threads have executed.\n"
;
#endif
