#include <ministd.h>

#include <ministd_io.h>
#include <ministd_syscall.h>

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
	usz i;
	lcode = exitcode;

	for (i = 0; i < exithooks_count; ++i) {
		exithooks[i]();
	}

	_syscall1(__NR_exit_group, dummy, lcode);
	__builtin_unreachable();
}
void
thread_exit(int exitcode)
{
	isz lcode, dummy;
	lcode = exitcode;

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

	/* align stack pointer to 16 bytes before calling functions
	 * (because apparently we need to do that)
	 */
	__asm__ volatile(
		"add $0x10, %%rsp;" /* sp += 16 */
		"and $0xFFFFFFFFFFFFFFF0, %%rsp;" /* make sp divisible by 16 (rounding down) */
		: /* outputs */
		: /* inputs */
		: /* clobber */
	);

	__envp = __argv + __argc + 1;

	setup();

	exit(main());
	__builtin_unreachable();
}

void
__stack_chk_fail(void)
{
	const char msg[] = "Stack smashing detected!";

	fd_write(2 /* stderr */, (ptr)msg, sizeof(msg)/sizeof(*msg), NULL);
	exit(127);
}
