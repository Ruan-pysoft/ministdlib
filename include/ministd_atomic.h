#ifndef _RM_STD_ATOMIC_H
#define _RM_STD_ATOMIC_H

#include <_ministd_prelude.h>

enum memory_order {
	MO_RELAXED,
	MO_STRICT
};

#define LIST_OF_ATOMICS \
	X(C, c, signed char) \
	X(UC, uc, unsigned char) \
	X(H, h, short) \
	X(UH, uh, unsigned short) \
	X(I, i, int) \
	X(UI, ui, unsigned int) \
	X(L, l, long) \
	X(UL, ul, unsigned long) \
	X(Z, z, isz) \
	X(UZ, uz, usz)

#define X(SUFF, suff, type) struct Atomic ## SUFF;
LIST_OF_ATOMICS
#undef X

#define X(SUFF, suff, type) \
struct Atomic ## SUFF own atomic_new_ ## suff(type val, err_t ref err_out);
LIST_OF_ATOMICS
#undef X

#define X(SUFF, suff, type) \
type atomic_load_ ## suff(volatile struct Atomic ## SUFF ref this, enum memory_order order);
LIST_OF_ATOMICS
#undef X

#define X(SUFF, suff, type) \
void atomic_store_ ## suff(volatile struct Atomic ## SUFF ref this, type val, enum memory_order order);
LIST_OF_ATOMICS
#undef X

#define X(SUFF, suff, type) \
type NAME(suff)(volatile struct Atomic ## SUFF ref this, type val, enum memory_order order);

#define NAME(suff) atomic_swap_ ## suff
LIST_OF_ATOMICS
#undef NAME

#define NAME(suff) atomic_fetch_add_ ## suff
LIST_OF_ATOMICS
#undef NAME

#define NAME(suff) atomic_fetch_sub_ ## suff
LIST_OF_ATOMICS
#undef NAME

#define NAME(suff) atomic_fetch_or_ ## suff
LIST_OF_ATOMICS
#undef NAME

#define NAME(suff) atomic_fetch_xor_ ## suff
LIST_OF_ATOMICS
#undef NAME

#define NAME(suff) atomic_fetch_and_ ## suff
LIST_OF_ATOMICS
#undef NAME

#define NAME(suff) atomic_fetch_min_ ## suff
LIST_OF_ATOMICS
#undef NAME

#define NAME(suff) atomic_fetch_max_ ## suff
LIST_OF_ATOMICS
#undef NAME

#undef X

#define X(SUFF, suff, type) \
bool atomic_compare_exchange_ ## suff(volatile struct Atomic ## SUFF ref this, type ref old, type new, enum memory_order order);
LIST_OF_ATOMICS
#undef X

void memory_fence(void);

#endif /* _RM_STD_ATOMIC_H */
