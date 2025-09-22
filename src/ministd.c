#include <ministd.h>

#include <ministd_io.h>
#include <ministd_syscall.h>

#include <_ministd_tests.h>

#include <asm/unistd.h>

static int __argc;
static char own own __argv;
static int __envc;
static char own own __envp;

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

/* store exit hooks into a statically-allocated array of a fixed max size
 * the exit hooks are run on program exit
 */

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

	/* run exit hooks in reverse order of registration.
	 * this allows a pattern of
	 *  init_comp1(); atexit(deinit_comp1());
	 *  init_comp2_dep_comp1(); atexit(deinit_comp2_dep_comp1());
	 * where a later-initialised component will be cleaned up
	 * before any of its (earlier-initialised) dependencies are cleaned up
	 */
	for (i = 0; i < exithooks_count; ++i) {
		exithooks[exithooks_count - i - 1]();
	}

	_syscall1(__NR_exit_group, dummy, lcode);
	__builtin_unreachable();
}
void
thread_exit(int exitcode)
{
	isz lcode, dummy;
	lcode = exitcode;

	/* no exit hooks run:
	 * resources still in use by main thread
	 */

	_syscall1(__NR_exit, dummy, lcode);
	__builtin_unreachable();
}
void
panic(isz exitcode)
{
	isz dummy;

	_syscall1(__NR_exit, dummy, exitcode);
	__builtin_unreachable();
}
int main(void);
extern void _ministd_io_init(void);
void
setup(void)
{
	/* find the length of envp, as the kernel does not provide it */
	for (__envc = 0; __envp[__envc] != NULL; ++__envc);

	_ministd_io_init();
}
#ifdef TEST
/* alternative entry point for running stdlib tests */
int
run_test_main(void)
{
	err_t err = ERR_OK;

	test_main(&err);
	if (err != ERR_OK) perror(err, "while running tests");

	return (int)err;
}
#endif
void
_start(void)
{
	/* retrieve argv, argc, envp from stack where the kernel pushed it */
	/* the _start function is deliberately minimal,
	 * having no local variables, branches, or loops,
	 * so that gcc won't place extra stuff on the stack
	 * and mess up the assembly
	 */
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

#ifdef TEST
	exit(run_test_main());
#else
	exit(main());
#endif
	__builtin_unreachable();
}

/* provided bc at some point gcc wouldn't link without this function defined,
 * and I might as well have it
 */
void
__stack_chk_fail(void)
{
	const char msg[] = "Stack smashing detected!\n";

	fd_write(2 /* stderr */, (ptr)msg, sizeof(msg)/sizeof(*msg), NULL);
	PANIC(127);
}

#ifdef TEST
void TEST_FN_NAME(atexit_helper)(void) {}
TEST_FN(atexit) {
	isz printuzx(usz, err_t ref);

	atexit(TEST_FN_NAME(atexit_helper));

	ASSERT_EQ(
		uzx,
		(usz)exithooks[exithooks_count-1],
		(usz)TEST_FN_NAME(atexit_helper)
	);

	--exithooks_count;
}
TEST_OUTPUT(atexit) = "";
#endif
