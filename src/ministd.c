#include "ministd.h"

#include "ministd_syscall.h"

#include <asm/unistd.h>

int __argc;
char own own __argv;
int __envc;
char own own __envp;

int
argc(void)
{
	return __argc;
}
char ref ref
argv(void)
{
	return __argv;
}
int
envc(void)
{
	return __envc;
}
char ref ref
envp(void)
{
	return __envp;
}

#define EXITHOOKS_CAP 64
static void (ref exithooks[EXITHOOKS_CAP])(void);
static usz exithooks_count = 0;

void
atexit(void(ref func)(void))
{
	exithooks[exithooks_count++] = func;
}
void
exit(int exitcode)
{
	isz lcode, dummy;
	isz i;
	lcode = exitcode;

	for (i = 0; i < exithooks_count; ++i) {
		exithooks[i]();
	}

	_syscall1(__NR_exit, dummy, lcode);
	__builtin_unreachable();
}
int main(void);
extern void ministd_io_init(void);
void
setup(void)
{
	for (__envc = 0; __envp[__envc] != NULL; ++__envc);

	ministd_io_init();
}
void
_start(void)
{
	__asm__ volatile(
		"mov %%rsp, %%rax;" /* sp -> ax */
		"mov 8(%%rax), %[argc];" /* retrieve argc, top item of stack */
		"mov %%rax, %[argv];" /* retrieve argv, second-to-top of stack */
		"add $16, %[argv];"
		: [argc] "=r" (__argc), [argv] "=r" (__argv) /* outputs */
		: /* inputs */
		: "rax" /* clobber */
	);

	__envp = __argv + __argc + 1;

	setup();

	exit(main());
	__builtin_unreachable();
}

#include "ministd_io.h"

void
__stack_chk_fail(void)
{
	const char msg[] = "Stack smashing detected!";

	fd_write(2 /* stderr */, (ptr)msg, sizeof(msg)/sizeof(*msg), NULL);
	exit(127);
}
