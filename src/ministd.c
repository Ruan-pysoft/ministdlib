#include "ministd.h"

#include "ministd_syscall.h"

#include <asm/unistd.h>

void
exit(int exitcode)
{
	isz lcode, dummy;
	lcode = exitcode;

	_syscall1(__NR_exit, dummy, lcode);
	__builtin_unreachable();
}
int main(int argc, char ref ref argv);
extern void ministd_io_init(void);
void
setup(void)
{
	ministd_io_init();
}
void
_start(void)
{
	int argc;
	char ref ref argv;

	__asm__ volatile(
		"mov %%rsp, %%rax;" /* sp -> ax */
		"mov 24(%%rax), %[argc];" /* retrieve argc, top item of stack */
		"mov %%rax, %[argv];" /* retrieve argv, second-to-top of stack */
		"add $32, %[argv];"
		: [argc] "=r" (argc), [argv] "=r" (argv) /* outputs */
		: /* inputs */
		: "rax" /* clobber */
	);

	setup();

	exit(main(argc, argv));
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
