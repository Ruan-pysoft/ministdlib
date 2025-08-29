#include <ministd_sched.h>

#include <ministd_syscall.h>

static usz
clone_thread(int (ref fn)(ptr), ptr arg, struct clone_args ref cl_args, err_t ref err_out)
{
	/*register isz cl_res;

	_syscall_clone3(cl_res, cl_args);
	if (cl_res < 0) ERR_WITH(-cl_res, 0);

	if (cl_res == 0) {
		/ * in child * /
		register int exitcode;

		__asm__ volatile(
			"mov %[stack_top], %%rsp;"
			"mov %[stack_top], %%rbp;"
			:
			: [stack_top] "g" (((cl_args->stack + cl_args->stack_size) & ~0xF) - 8)
			:
		);

		exitcode = fn(arg);
		thread_exit(exitcode);
	}

	/ * in parent (cl_res != 0) * /
	return (usz) cl_res;*/

	isz res;

	/* I gave up trying to get this to work in c for now,
	 * so enjoy some inline assembly instead.
	 * Tbf, I don't think c is really made for
	 * *this* kind of low-level stack manipulation and the like
	 */
	__asm__ volatile(
		/* call the clone3 syscall:
		 * syscall number is provided via $rax ("a") in input list,
		 * cl_args is provided via $rdi ("D") in input list,
		 * and sizeof(*cl_args) is provided via $rsi ("S") in input list.
		 */
		"syscall\n"
		/* if the result ($rax) != zero,
		 * you are not in the child, so jump to parent code
		 */
		"test %%rax, %%rax\n"
		"jnz parent\n"
		/* in child (syscall result is 0) */
		/* adjust $rsp and $rbp to point just below the stack top,
		 * aligned such that $rsp == $rbp and $rsp % 16 == 8.
		 * I *think* I don't need to adjust $rbp?
		 * As when I call a function, it updates $rbp to $rsp,
		 * and when I finally return I don't use the stack anyways,
		 * so doesn't really matter what its value is here.
		 * Either way, doing both for now.
		 */
		"mov %[stack_top], %%rsp\n"
		"mov %[stack_top], %%rbp\n"
		/* the first argument of the function is passed via $rdi */
		"mov %[arg], %%rdi\n"
		"call *%[fn]\n"
		/* result is in $rax, which is the exit code,
		 * so move $rax into $rdx for the argument of the exit call
		 */
		"mov %%rax, %%rdi\n"
		"call thread_exit\n"
		/* parent code goes after my inline asm;
		 * it is the owner of the main stack,
		 * so I can once again let C run wild
		 * instead of using inline asm
		 */
		"parent:\n"
		: "=a" (res)
		: [fn] "r" (fn), [arg] "d" (arg), [cl_args] "D" (cl_args),
		  [cl_args_size] "S" (sizeof(*cl_args)),
		  [stack_top] "r" (((cl_args->stack + cl_args->stack_size) & ~0xF) - 8),
		  "a" ((usz)__NR_clone3)
		: "rcx"
	);

	/* res < 0 --> error; res > 0 --> pid of child */

	if (res < 0) ERR_WITH(-res, 0);

	return (usz) res;
}
static usz
clone_process(int (ref fn)(ptr), ptr arg, struct clone_args ref cl_args, err_t ref err_out)
{
	isz cl_res;

	_syscall_clone3(cl_res, cl_args);
	if (cl_res < 0) ERR_WITH(-cl_res, 0);

	if (cl_res == 0) {
		/* in child */
		exit(fn(arg));
	}

	/* in parent (cl_res != 0) */
	return (usz) cl_res;
}

usz
clone(int (ref fn)(ptr), ptr arg, struct clone_args ref cl_args, err_t ref err_out)
{
	if (cl_args->flags & CLONE_VM) {
		return clone_thread(fn, arg, cl_args, err_out);
	} else {
		return clone_process(fn, arg, cl_args, err_out);
	}
}
