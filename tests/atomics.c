#include <linux/sched.h>
#include <ministd.h>
#include <ministd_atomic.h>
#include <ministd_fmt.h>
#include <ministd_memory.h>
#include <ministd_time.h>
#include <ministd_sched.h>

#define N_ITERS 999
#define N_THREADS 63

int thread_fn(void ref arg) {
	struct AtomicI ref counter = arg;
	usz i;

	for (i = 0; i < N_ITERS; ++i) {
		atomic_fetch_add_i(counter, 1, MO_STRICT);
	}

	return 0;
}

const char ref MSG_LOWER = "hello, world\n";
const char lowercase_bit = 'a'^'A';

#define print(sort, val) fprint ## sort(val, stdout, NULL)

int main(void) {
	struct AtomicC own ch = atomic_new_c(0, NULL);
	struct AtomicI own counter = atomic_new_i(0, NULL);
	struct clone_args cl_args = { 0 };
	usz i;
	char c;
	int num;

	for (i = 0; true; ++i) {
		atomic_store_c(ch, MSG_LOWER[i], MO_STRICT);

		if (MSG_LOWER[i] >= 'a' && MSG_LOWER[i] <= 'z') {
			c = atomic_fetch_and_c(ch, ~lowercase_bit, MO_STRICT);
		} else {
			c = atomic_load_c(ch, MO_STRICT);
		}
		if (c != MSG_LOWER[i]) {
			print(s, "Wrong value stored in atomic!\n");
			print(s, "Expected '");
			print(c, MSG_LOWER[i]);
			print(s, "', but found '");
			print(c, c);
			print(s, "'.\n");
			return 1;
		}

		c = atomic_load_c(ch, MO_STRICT);
		putc(c, NULL);

		if (c == '\0') break;
	}

	cl_args.flags = CLONE_VM | CLONE_THREAD | CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_SIGHAND | CLONE_SYSVSEM;

	for (i = 0; i < N_THREADS; ++i) {
		own_ptr stack = alloc(256, NULL);
		cl_args.stack = (usz)stack;
		cl_args.stack_size = 256;

		clone(thread_fn, counter, &cl_args, NULL);
	}

	sleep(1, NULL);

	num = atomic_load_i(counter, MO_STRICT);

	print(s, "Expecting value ");
	print(i, N_ITERS * N_THREADS);
	print(s, ", got ");
	print(i, num);
	print(c, '\n');

	return 0;
}
