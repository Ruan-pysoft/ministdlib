#include <linux/sched.h>
#include <ministd.h>
#include <ministd_threads.h>
#include <ministd_fmt.h>
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

int thread_fn(void ref void_arg) {
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

const char ref MSG_LOWER = "hello, world\n";
const char lowercase_bit = 'a'^'A';

void
main(err_t ref err_out)
{
	mutex_t own lock = mutex_new(NULL);
	struct clone_args cl_args = { 0 };
	usz i;

	(void) err_out;

	cl_args.flags = CLONE_VM | CLONE_THREAD | CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_SIGHAND | CLONE_SYSVSEM;

	for (i = 0; i < N_THREADS; ++i) {
		own_ptr stack = alloc(256, NULL);
		struct thread_fn_arg own arg = alloc(sizeof(struct thread_fn_arg), NULL);
		cl_args.stack = (usz)stack;
		cl_args.stack_size = 256;
		arg->lock = lock;
		arg->thread_id = i;

		prints("Launching thread #", NULL);
		printuz(i, NULL);
		prints("...\n", NULL);

		clone(thread_fn, arg, &cl_args, NULL);
	}

	while (num_executed < N_THREADS) {}
	
	prints("All threads have executed! Checking...\n", NULL);

	for (i = 0; i < N_THREADS; ++i) {
		prints("Thread ", NULL);
		printuz(i, NULL);
		prints(" has executed? ", NULL);
		printi(has_executed[i], NULL);
		printc('\n', NULL);
	}
}
