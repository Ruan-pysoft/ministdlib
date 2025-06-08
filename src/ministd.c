#include "ministd.h"
#include "ministd_io.h"

#include <asm/unistd.h>

void exit(code)
{
	long lcode;
	lcode = code;

	__asm__ volatile(
		"mov %[code], %%rdi;" /* exit code */
		"syscall;"
		: /* outputs */
		: "a" ((long)__NR_exit), [code] "r" (lcode) /* inputs */
		: "rdi" /* clobber */
	);
	__builtin_unreachable();
}
int main(int, str arr);
void setup()
{
	_minilib_io_init();
}
void _start()
{
	int argc;
	str arr argv;

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
