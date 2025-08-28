#include <ministd_atomic.h>

#include <ministd_memory.h>

struct AtomicC {
	signed char val;
};
struct AtomicUC {
	unsigned char val;
};
struct AtomicH {
	short val;
};
struct AtomicUH {
	unsigned short val;
};
struct AtomicI {
	int val;
};
struct AtomicUI {
	unsigned int val;
};
struct AtomicL {
	long val;
};
struct AtomicUL {
	unsigned long val;
};
struct AtomicZ {
	isz val;
};
struct AtomicUZ {
	usz val;
};

struct AtomicC own
atomic_new_c(signed char val, err_t ref err_out)
{
	err_t err = ERR_OK;
	struct AtomicC own res;

	res = alloc(sizeof(*res), &err);
	TRY_WITH(err, NULL);

	res->val = val;

	return res;
}

signed char
atomic_load_c(volatile struct AtomicC ref this, enum memory_order order)
{
	signed char res;

	(void) order;

	__asm__ volatile(
		"mov (%[atomic]), %[res]"
		: [res] "=g" (res)
		: [atomic] "g" (&this->val)
		: "memory"
	);

	return res;
}

void
atomic_store_c(volatile struct AtomicC ref this, signed char val,
	       enum memory_order order)
{
	(void) order;

	__asm__ volatile(
		"mov %[val], (%[atomic])"
		:
		: [atomic] "g" (&this->val), [val] "a" (val)
		: "memory"
	);
}

#define LOCKED_BODY(type, instr) do { \
		type res = val; \
		(void) order; \
		__asm__ volatile( \
			"lock " instr " %[res], (%[atomic])" \
			: [res] "+r" (res) \
			: [atomic] "g" (&this->val) \
			: "memory" \
		); \
		return res; \
	} while (0);

signed char
atomic_swap_c(volatile struct AtomicC ref this, signed char val,
	      enum memory_order order)
{
	signed char res = val;

	(void) order;

	__asm__ volatile(
		"xchg %[res], (%[atomic])"
		: [res] "+g" (res)
		: [atomic] "g" (&this->val)
		: "memory"
	);

	return res;
}

signed char
atomic_fetch_add_c(volatile struct AtomicC ref this, signed char val,
		   enum memory_order order)
{
	LOCKED_BODY(signed char, "xadd");
}

signed char
atomic_fetch_sub_c(volatile struct AtomicC ref this, signed char val,
		   enum memory_order order)
{
	val = -val;

	LOCKED_BODY(signed char, "xadd");
}

#define LOCKED_COMPLEX_BODY(type, suffix, op) do { \
		type old, new; \
		old = atomic_load_ ## suffix(this, order); \
		do { \
			new = old op val; \
		} while (!atomic_compare_exchange_ ## suffix(this, &old, new, order)); \
		return old; \
	} while (0)

signed char
atomic_fetch_or_c(volatile struct AtomicC ref this, signed char val,
		   enum memory_order order)
{
	LOCKED_COMPLEX_BODY(signed char, c, |);
}

signed char
atomic_fetch_xor_c(volatile struct AtomicC ref this, signed char val,
		   enum memory_order order)
{
	LOCKED_COMPLEX_BODY(signed char, c, ^);
}

signed char
atomic_fetch_and_c(volatile struct AtomicC ref this, signed char val,
		   enum memory_order order)
{
	LOCKED_COMPLEX_BODY(signed char, c, &);
}

#define LOCKED_MINMAX_BODY(type, suffix, cmp) do { \
		type old, new; \
		do { \
			old = atomic_load_ ## suffix(this, order); \
			new = val cmp old ? val : old; \
		} while (!atomic_compare_exchange_ ## suffix(this, &old, new, order)); \
		return old; \
	} while (0)

signed char
atomic_fetch_max_c(volatile struct AtomicC ref this, signed char val,
		   enum memory_order order)
{
	LOCKED_MINMAX_BODY(signed char, c, >);
}

signed char
atomic_fetch_min_c(volatile struct AtomicC ref this, signed char val,
		   enum memory_order order)
{
	LOCKED_MINMAX_BODY(signed char, c, <);
}

bool
atomic_compare_exchange_c(volatile struct AtomicC ref this, signed char ref old,
			  signed char new, enum memory_order order)
{
	bool res;

	(void) order;

	__asm__ volatile(
		"lock cmpxchg %[new], (%[atomic])\n"
		"setz %b[res]\n"
		"movzx %b[res], %[res]\n"
		: [expected] "+a" (*old),
		  [res] "=r" (res)
		: [atomic] "g" (&this->val), [new] "d" (new)
		: "memory"
	);

	return res;
}

void
atomic_fence(enum memory_order order)
{
	(void) order;
}
