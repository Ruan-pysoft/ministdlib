#include <linux/sched.h>
#include <ministd.h>
#include <ministd_atomic.h>
#include <ministd_fmt.h>
#include <ministd_memory.h>
#include <ministd_time.h>
#include <ministd_sched.h>

#define fprint(sort, val, file) fprint ## sort(val, file, NULL)
#define print(sort, val) fprint(sort, val, stdout)
#define debug(sort, val) fprint(sort, val, stderr)

#define ASSERT_EQ(msg, sort, A, B) \
	if ((A) == (B)) { \
		print(s, "Successfull assert "); \
		print(s, msg); \
		print(s, ": equals value "); \
		print(sort, (B)); \
		print(c, '\n'); \
	} else { \
		print(s, "Failed assert "); \
		print(s, msg); \
		print(s, ": "); \
		print(sort, (A)); \
		print(s, " != "); \
		print(sort, (B)); \
		print(c, '\n'); \
		exit(1); \
	}

#define X(SUFF, suff, type) \
	void atomic_test_ ## suff(void) { \
		struct Atomic ## SUFF own atom = atomic_new_ ## suff(0, NULL); \
		type a, b; \
		print(s, "Testing type " #type "\n"); \
		a = atomic_load_ ## suff(atom, MO_STRICT); \
		ASSERT_EQ("initial load", z, a, 0); \
		atomic_store_ ## suff(atom, 42, MO_STRICT); \
		a = atomic_load_ ## suff(atom, MO_STRICT); \
		ASSERT_EQ("after store", z, a, 42); \
		b = 13; \
		a = atomic_swap_ ## suff(atom, b, MO_STRICT); \
		ASSERT_EQ("swap (old value)", z, a, 42); \
		a = atomic_fetch_add_ ## suff(atom, 127, MO_STRICT); \
		ASSERT_EQ("swap (new value)", z, a, b); \
		b += 127; \
		a = atomic_fetch_sub_ ## suff(atom, 21, MO_STRICT); \
		ASSERT_EQ("add", z, a, b); \
		b -= 21; \
		a = atomic_fetch_or_ ## suff(atom, ~52, MO_STRICT); \
		ASSERT_EQ("sub", z, a, b); \
		b |= ~52; \
		a = atomic_fetch_and_ ## suff(atom, ~32, MO_STRICT); \
		ASSERT_EQ("or", z, a, b); \
		b &= ~32; \
		a = atomic_fetch_xor_ ## suff(atom, 45, MO_STRICT); \
		ASSERT_EQ("and", z, a, b); \
		b ^= 45; \
		a = atomic_fetch_min_ ## suff(atom, 125, MO_STRICT); \
		ASSERT_EQ("xor", z, a, b); \
		b = b < 125 ? b : 125; \
		a = atomic_fetch_max_ ## suff(atom, 120, MO_STRICT); \
		ASSERT_EQ("min", z, a, b); \
		b = b > 120 ? b : 120; \
		a = atomic_load_ ## suff(atom, MO_STRICT); \
		ASSERT_EQ("max", z, a, b); \
		/* TODO: test atomic_compare_exchange_ */ \
	}
LIST_OF_ATOMICS
#undef X

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

	sleep(0.1, NULL);

	num = atomic_load_i(counter, MO_STRICT);

	print(s, "Expecting value ");
	print(i, N_ITERS * N_THREADS);
	print(s, ", got ");
	print(i, num);
	print(c, '\n');

	#define X(SUFF, suff, type) atomic_test_ ## suff();
	LIST_OF_ATOMICS
	#undef X

	return 0;
}
