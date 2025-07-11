#ifndef _RM_STD_SYSCALL_H
#define _RM_STD_SYSCALL_H

#include <ministd_types.h>

/* Apparently from kernel not stdc, so I'll do this... for now */
#include <asm/unistd.h>

#define _syscall0(n, out) \
	__asm__ volatile( \
		"syscall;" \
		: "=a" (out) \
		: "0" ((usz)n) \
		: "rcx", "r11" /* clobber */ \
	)
#define _syscall1(n, out, arg1) \
	__asm__ volatile( \
		"mov %[a1], %%rdi;" \
		"syscall;" \
		: "=a" (out) \
		: "0" ((usz)n), [a1] "g" (arg1) \
		: "rcx", "r11", "rdi" /* clobber */ \
	)
#define _syscall2(n, out, arg1, arg2) \
	__asm__ volatile( \
		"mov %[a1], %%rdi;" \
		"mov %[a2], %%rsi;" \
		"syscall;" \
		: "=a" (out) \
		: "0" ((usz)n), [a1] "g" (arg1), [a2] "g" (arg2) \
		: "rcx", "r11", "rdi", "rsi" /* clobber */ \
	)
#define _syscall3(n, out, arg1, arg2, arg3) \
	__asm__ volatile( \
		"mov %[a1], %%rdi;" \
		"mov %[a2], %%rsi;" \
		"syscall;" \
		: "=a" (out) \
		: "0" ((usz)n), [a1] "g" (arg1), [a2] "g" (arg2), "d" (arg3) \
		: "rcx", "r11", "rdi", "rsi" /* clobber */ \
	)
#define _syscall4(n, out, arg1, arg2, arg3, arg4) \
	__asm__ volatile( \
		"mov %[a1], %%rdi;" \
		"mov %[a2], %%rsi;" \
		"mov %[a4], %%r10;" \
		"syscall;" \
		: "=a" (out) \
		: "0" ((usz)n), [a1] "g" (arg1), [a2] "g" (arg2), "d" (arg3), \
		  [a4] "g" (arg4) \
		: "rcx", "r11", "rdi", "rsi", "r10" /* clobber */ \
	)
#define _syscall5(n, out, arg1, arg2, arg3, arg4, arg5) \
	__asm__ volatile( \
		"mov %[a1], %%rdi;" \
		"mov %[a2], %%rsi;" \
		"mov %[a4], %%r10;" \
		"mov %[a4], %%r10;" \
		"mov %[a5], %%r8;" \
		"syscall;" \
		: "=a" (out) \
		: "0" ((usz)n), [a1] "g" (arg1), [a2] "g" (arg2), "d" (arg3), \
		  [a4] "g" (arg4), [a5] "g" (arg5) \
		: "rcx", "r11", "rdi", "rsi", "r10", "r8" /* clobber */ \
	)
#define _syscall6(n, out, arg1, arg2, arg3, arg4, arg5, arg6) \
	__asm__ volatile( \
		"mov %[a1], %%rdi;" \
		"mov %[a2], %%rsi;" \
		"mov %[a4], %%r10;" \
		"mov %[a4], %%r10;" \
		"mov %[a5], %%r8;" \
		"mov %[a6], %%r9;" \
		"syscall;" \
		: "=a" (out) \
		: "0" ((usz)n), [a1] "g" (arg1), [a2] "g" (arg2), "d" (arg3), \
		  [a4] "g" (arg4), [a5] "g" (arg5), [a6] "g" (arg6) \
		: "rcx", "r11", "rdi", "rsi", "r10", "r8", "r9" /* clobber */ \
	)

#endif /* _RM_STD_SYSCALL_H */
