#include <ministd_atomic.h>

#include <ministd_memory.h>

#define X(SUFF, suff, type) struct Atomic ## SUFF { type val; };
LIST_OF_ATOMICS
#undef X

#define X(SUFF, suff, type) \
	struct Atomic ## SUFF own \
	atomic_new_ ## suff(type val, err_t ref err_out) \
	{ \
		err_t err = ERR_OK; \
		struct Atomic ## SUFF own res; \
		res = alloc(sizeof(*res), &err); \
		TRY_WITH(err, NULL); \
		res->val = val; \
		return res; \
	}
LIST_OF_ATOMICS
#undef X

#define X(SUFF, suff, type) \
	type \
	atomic_load_ ## suff(volatile struct Atomic ## SUFF ref this, enum memory_order order) \
	{ \
		type res; \
		if (order == MO_STRICT) __asm__ volatile("mfence" ::: "memory"); \
		__asm__ volatile( \
			"mov (%[atomic]), %[res]" \
			: [res] "=g" (res) \
			: [atomic] "g" (&this->val) \
			: "memory" \
		); \
		return res; \
	}
LIST_OF_ATOMICS
#undef X

#define X(SUFF, suff, type) \
	void \
	atomic_store_ ## suff(volatile struct Atomic ## SUFF ref this, type val, enum memory_order order) \
	{ \
		if (order == MO_STRICT) __asm__ volatile("mfence" ::: "memory"); \
		__asm__ volatile( \
			"mov %[val], (%[atomic])" \
			: \
			: [atomic] "g" (&this->val), [val] "a" (val) \
			: "memory" \
		); \
	}
LIST_OF_ATOMICS
#undef X

#define X(SUFF, suff, type) \
	type \
	NAME(suff)(volatile struct Atomic ## SUFF ref this, type val, enum memory_order order) \
	{ \
		type res = VAL(val); \
		if (order == MO_STRICT) __asm__ volatile("mfence" ::: "memory"); \
		__asm__ volatile( \
			INSTR " %[res], (%[atomic])" \
			: [res] "+r" (res) \
			: [atomic] "g" (&this->val) \
			: "memory" \
		); \
		return res; \
	}

#define VAL(val) val

#define NAME(suff) atomic_swap_ ## suff
#define INSTR "xchg"
LIST_OF_ATOMICS
#undef INSTR
#undef NAME

#define NAME(suff) atomic_fetch_add_ ## suff
#define INSTR "lock xadd"
LIST_OF_ATOMICS
#undef INSTR
#undef NAME

#undef VAL
#define VAL(val) -val;
#define NAME(suff) atomic_fetch_sub_ ## suff
#define INSTR "lock xadd"
LIST_OF_ATOMICS
#undef INSTR
#undef NAME
#undef VAL
#define VAL(val) val

#undef BODY_EXTRA

#undef X

#define X(SUFF, suff, type) \
	type \
	NAME(suff)(volatile struct Atomic ## SUFF ref this, type val, enum memory_order order) \
	{ \
		type old, new; \
		old = atomic_load_ ## suff(this, order); \
		do { \
			new = OP(old, val); \
		} while (!atomic_compare_exchange_ ## suff(this, &old, new, order)); \
		return old; \
	}

#define NAME(suff) atomic_fetch_or_ ## suff
#define OP(lhs, rhs) (lhs) | (rhs)
LIST_OF_ATOMICS
#undef OP
#undef NAME

#define NAME(suff) atomic_fetch_xor_ ## suff
#define OP(lhs, rhs) (lhs) ^ (rhs)
LIST_OF_ATOMICS
#undef OP
#undef NAME

#define NAME(suff) atomic_fetch_and_ ## suff
#define OP(lhs, rhs) (lhs) & (rhs)
LIST_OF_ATOMICS
#undef OP
#undef NAME

#define NAME(suff) atomic_fetch_min_ ## suff
#define OP(lhs, rhs) (lhs) < (rhs) ? (lhs) : (rhs)
LIST_OF_ATOMICS
#undef OP
#undef NAME

#define NAME(suff) atomic_fetch_max_ ## suff
#define OP(lhs, rhs) (lhs) > (rhs) ? (lhs) : (rhs)
LIST_OF_ATOMICS
#undef OP
#undef NAME

#undef X

#define X(SUFF, suff, type) \
	bool \
	atomic_compare_exchange_ ## suff(volatile struct Atomic ## SUFF ref this, type ref old, type new, enum memory_order order) \
	{ \
		bool res; \
		if (order == MO_STRICT) __asm__ volatile("mfence" ::: "memory"); \
		__asm__ volatile( \
			"lock cmpxchg %[new], (%[atomic])\n" \
			"setz %b[res]\n" \
			"movzx %b[res], %[res]\n" \
			: [expected] "+a" (*old), \
			  [res] "=r" (res) \
			: [atomic] "g" (&this->val), [new] "d" (new) \
			: "memory" \
		); \
		return res; \
	}
LIST_OF_ATOMICS
#undef X

void
memory_fence(void)
{
	__asm__ volatile("mfence" ::: "memory");
}

#ifdef TEST
#include <_ministd_tests.h>

#define X(SUFF, suff, type) \
	static TEST_FN(atomic_ ## suff) { \
		err_t err = ERR_OK; \
		struct Atomic ## SUFF own atom = atomic_new_ ## suff(0, &err); \
		type a, b; \
		TRY_VOID(err); \
		OUT(s, "Testing atomic type " #type "\n"); \
		a = atomic_load_ ## suff(atom, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, 0, "initial load"); \
		atomic_store_ ## suff(atom, 42, MO_STRICT); \
		a = atomic_load_ ## suff(atom, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, 42, "after store"); \
		b = 13; \
		a = atomic_swap_ ## suff(atom, b, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, 42, "swap (old value)"); \
		a = atomic_fetch_add_ ## suff(atom, 127, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, b, "swap (new value)"); \
		b += 127; \
		a = atomic_fetch_sub_ ## suff(atom, 21, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, b, "add"); \
		b -= 21; \
		a = atomic_fetch_or_ ## suff(atom, ~52, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, b, "sub"); \
		b |= ~52; \
		a = atomic_fetch_and_ ## suff(atom, ~32, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, b, "or"); \
		b &= ~32; \
		a = atomic_fetch_xor_ ## suff(atom, 45, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, b, "and"); \
		b ^= 45; \
		a = atomic_fetch_min_ ## suff(atom, 125, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, b, "xor"); \
		b = b < 125 ? b : 125; \
		a = atomic_fetch_max_ ## suff(atom, 120, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, b, "min"); \
		b = b > 120 ? b : 120; \
		a = atomic_load_ ## suff(atom, MO_STRICT); \
		ASSERT_EQ_MSG(z, a, b, "max"); \
		/* TODO: test atomic_compare_exchange_ */ \
	}
LIST_OF_ATOMICS
#undef X

#define N_ITERS 999
#define N_THREADS 63

int
thread_fn(void ref arg)
{
	struct AtomicI ref counter = arg;
	usz i;

	for (i = 0; i < N_ITERS; ++i) {
		atomic_fetch_add_i(counter, 1, MO_STRICT);
	}

	return 0;
}

const char ref MSG_LOWER = "hello, world\n";
const char lowercase_bit = 'a'^'A';

#include <ministd_sched.h>
#include <ministd_time.h>

TEST_FN(atomics) {
	err_t err = ERR_OK;
	struct AtomicC own ch = atomic_new_c(0, NULL);
	struct AtomicI own counter = atomic_new_i(0, NULL);
	struct clone_args cl_args = { 0 };
	usz i;
	char c = ' ';
	int num;

	for (i = 0; c != '\0'; ++i) {
		atomic_store_c(ch, MSG_LOWER[i], MO_STRICT);

		if (MSG_LOWER[i] >= 'a' && MSG_LOWER[i] <= 'z') {
			c = atomic_fetch_and_c(ch, ~lowercase_bit, MO_STRICT);
		} else {
			c = atomic_load_c(ch, MO_STRICT);
		}
		ASSERT_EQ_MSG(c, c, MSG_LOWER[i], "wrong value stored in atomic");

		c = atomic_load_c(ch, MO_STRICT);
		if (c) OUT(c, c);
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

	ASSERT_EQ_MSG(i, N_ITERS * N_THREADS, num, "atomic counter value is incorrect");

#define X(SUFF, suff, type) \
	TEST_FN_NAME(atomic_ ## suff)(outf, &err); \
	TRY_VOID(err);
	LIST_OF_ATOMICS
#undef X
}
TEST_OUTPUT(atomics) =
	"HELLO, WORLD\n"
	"Testing atomic type signed char\n"
	"Testing atomic type unsigned char\n"
	"Testing atomic type short\n"
	"Testing atomic type unsigned short\n"
	"Testing atomic type int\n"
	"Testing atomic type unsigned int\n"
	"Testing atomic type long\n"
	"Testing atomic type unsigned long\n"
	"Testing atomic type isz\n"
	"Testing atomic type usz\n"
;

#endif
