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
		(void) order; \
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
		(void) order; \
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
		type res = val; \
		(void) order; \
		BODY_EXTRA \
		__asm__ volatile( \
			INSTR " %[res], (%[atomic])" \
			: [res] "+r" (res) \
			: [atomic] "g" (&this->val) \
			: "memory" \
		); \
		return res; \
	}

#define BODY_EXTRA

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

#undef BODY_EXTRA
#define BODY_EXTRA val = -val;
#define NAME(suff) atomic_fetch_sub_ ## suff
#define INSTR "lock xadd"
LIST_OF_ATOMICS
#undef INSTR
#undef NAME
#undef BODY_EXTRA
#define BODY_EXTRA

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
		(void) order; \
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
atomic_fence(enum memory_order order)
{
	(void) order;
}
