#ifndef _RM_STD_ATOMIC_H
#define _RM_STD_ATOMIC_H

#include <ministd_types.h>

/* TODO: figure out if I also want release, acquire, consume etc.
 * like in <stdatomic.h>
 */
enum memory_order {
	MO_RELAXED,
	MO_STRICT
};

struct AtomicC;
struct AtomicUC;
struct AtomicH;
struct AtomicUH;
struct AtomicL;
struct AtomicUL;
struct AtomicI;
struct AtomicUI;
struct AtomicZ;
struct AtomicUZ;

#define ATOMIC_FN0_DECL(name) \
	char atomic_ ## name ## _c(volatile struct AtomicC ref this, enum memory_order order); \
	unsigned char atomic_ ## name ## _uc(volatile struct AtomicUC ref this, enum memory_order order); \
	short atomic_ ## name ## _h(volatile struct AtomicH ref this, enum memory_order order); \
	unsigned short atomic_ ## name ## _uh(volatile struct AtomicUH ref this, enum memory_order order); \
	int atomic_ ## name ## _i(volatile struct AtomicI ref this, enum memory_order order); \
	unsigned int atomic_ ## name ## _ui(volatile struct AtomicUI ref this, enum memory_order order); \
	long atomic_ ## name ## _l(volatile struct AtomicL ref this, enum memory_order order); \
	unsigned long atomic_ ## name ## _ul(volatile struct AtomicUL ref this, enum memory_order order); \
	isz atomic_ ## name ## _z(volatile struct AtomicZ ref this, enum memory_order order); \
	usz atomic_ ## name ## _uz(volatile struct AtomicUZ ref this, enum memory_order order)
#define ATOMIC_FN0_VOID_DECL(name) \
	void atomic_ ## name ## _c(volatile struct AtomicC ref this, enum memory_order order); \
	void atomic_ ## name ## _uc(volatile struct AtomicUC ref this, enum memory_order order); \
	void atomic_ ## name ## _h(volatile struct AtomicH ref this, enum memory_order order); \
	void atomic_ ## name ## _uh(volatile struct AtomicUH ref this, enum memory_order order); \
	void atomic_ ## name ## _i(volatile struct AtomicI ref this, enum memory_order order); \
	void atomic_ ## name ## _ui(volatile struct AtomicUI ref this, enum memory_order order); \
	void atomic_ ## name ## _l(volatile struct AtomicL ref this, enum memory_order order); \
	void atomic_ ## name ## _ul(volatile struct AtomicUL ref this, enum memory_order order); \
	void atomic_ ## name ## _z(volatile struct AtomicZ ref this, enum memory_order order); \
	void atomic_ ## name ## _uz(volatile struct AtomicUZ ref this, enum memory_order order)
#define ATOMIC_FN1_DECL(name) \
	char atomic_ ## name ## _c(volatile struct AtomicC ref this, char val, enum memory_order order); \
	unsigned char atomic_ ## name ## _uc(volatile struct AtomicUC ref this, unsigned char val, enum memory_order order); \
	short atomic_ ## name ## _h(volatile struct AtomicH ref this, short val, enum memory_order order); \
	unsigned short atomic_ ## name ## _uh(volatile struct AtomicUH ref this, unsigned short val, enum memory_order order); \
	int atomic_ ## name ## _i(volatile struct AtomicI ref this, int val, enum memory_order order); \
	unsigned int atomic_ ## name ## _ui(volatile struct AtomicUI ref this, unsigned int val, enum memory_order order); \
	long atomic_ ## name ## _l(volatile struct AtomicL ref this, long val, enum memory_order order); \
	unsigned long atomic_ ## name ## _ul(volatile struct AtomicUL ref this, unsigned long val, enum memory_order order); \
	isz atomic_ ## name ## _z(volatile struct AtomicZ ref this, isz val, enum memory_order order); \
	usz atomic_ ## name ## _uz(volatile struct AtomicUZ ref this, usz val, enum memory_order order)
#define ATOMIC_FN1_VOID_DECL(name) \
	void atomic_ ## name ## _c(volatile struct AtomicC ref this, char val, enum memory_order order); \
	void atomic_ ## name ## _uc(volatile struct AtomicUC ref this, unsigned char val, enum memory_order order); \
	void atomic_ ## name ## _h(volatile struct AtomicH ref this, short val, enum memory_order order); \
	void atomic_ ## name ## _uh(volatile struct AtomicUH ref this, unsigned short val, enum memory_order order); \
	void atomic_ ## name ## _i(volatile struct AtomicI ref this, int val, enum memory_order order); \
	void atomic_ ## name ## _ui(volatile struct AtomicUI ref this, unsigned int val, enum memory_order order); \
	void atomic_ ## name ## _l(volatile struct AtomicL ref this, long val, enum memory_order order); \
	void atomic_ ## name ## _ul(volatile struct AtomicUL ref this, unsigned long val, enum memory_order order); \
	void atomic_ ## name ## _z(volatile struct AtomicZ ref this, isz val, enum memory_order order); \
	void atomic_ ## name ## _uz(volatile struct AtomicUZ ref this, usz val, enum memory_order order)

ATOMIC_FN0_DECL(load);
ATOMIC_FN1_VOID_DECL(store);
ATOMIC_FN1_DECL(swap);
ATOMIC_FN1_DECL(fetch_add);
ATOMIC_FN1_DECL(fetch_sub);
ATOMIC_FN1_DECL(fetch_or);
ATOMIC_FN1_DECL(fetch_xor);
ATOMIC_FN1_DECL(fetch_and);
ATOMIC_FN1_DECL(fetch_max);
ATOMIC_FN1_DECL(fetch_min);

#define ATOMIC_CMP_EXCH_DECL(fn_suffix, atomic_suffix, type) \
	bool atomic_compare_exchange_ ## fn_suffix(volatile struct Atomic ## atomic_suffix, type ref old, type new, enum memory_order order)
ATOMIC_CMP_EXCH_DECL(c, C, char);
ATOMIC_CMP_EXCH_DECL(uc, UC, unsigned char);
ATOMIC_CMP_EXCH_DECL(h, H, char);
ATOMIC_CMP_EXCH_DECL(uh, UH, unsigned char);
ATOMIC_CMP_EXCH_DECL(i, I, char);
ATOMIC_CMP_EXCH_DECL(ui, UI, unsigned char);
ATOMIC_CMP_EXCH_DECL(l, L, char);
ATOMIC_CMP_EXCH_DECL(ul, UL, unsigned char);
ATOMIC_CMP_EXCH_DECL(z, Z, char);
ATOMIC_CMP_EXCH_DECL(uz, UZ, unsigned char);
#undef ATOMIC_CMP_EXCH_DECL

#undef ATOMIC_FN1_VOID_DECL
#undef ATOMIC_FN1_DECL
#undef ATOMIC_FN0_VOID_DECL
#undef ATOMIC_FN0_DECL

void atomic_fence(enum memory_order order);

#endif /* _RM_STD_ATOMIC_H */
