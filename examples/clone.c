#include <ministd.h>
#include <ministd_fmt.h>
#include <ministd_memory.h>
#include <ministd_sched.h>
#include <ministd_syscall.h>
#include <ministd_time.h>

#define _SYS_MMAN_H
#include <bits/mman-linux.h>

#define CHILD_STACK_SZ (1 << 16)

own_ptr mmap(ptr addr, usz len, int prot, int flags, int fildes, usz off, err_t ref err_out);
void munmap(ptr addr, usz len, err_t ref err_out);

int
child_fn(ptr void_arg)
{
	err_t err = ERR_OK;
	int ref arg = void_arg;
	ptr rbp;

	__asm__ volatile("mov %%rbp, %[rbp]" : [rbp] "=r" (rbp));

	fprints("Immediately in child!\n", stderr, &err);
	if (err != ERR_OK) goto exit_error;

	sleep(1, &err);
	if (err != ERR_OK) goto exit_error;

	fprints("Child stack base at 0x", stderr, &err);
	if (err != ERR_OK) goto exit_error;
	fprintp(rbp, stderr, &err);
	if (err != ERR_OK) goto exit_error;
	fprintc('\n', stderr, &err);
	if (err != ERR_OK) goto exit_error;

	fprints("Doing child stuff after one second!\n", stderr, &err);
	if (err != ERR_OK) goto exit_error;
	*arg = 42;

	return 0;
exit_error:
	perror(err, "in child thread");
	return 1;
}

void
process_demo(err_t ref err_out)
{
	err_t err = ERR_OK;
	struct clone_args cl_args = {
		0, /* flags */
		0, /* pidfd [ (int*) ] */
		0, /* child TID [ (pid_t*) ] */
		0, /* parent TID [ (pid_t*) ] */
		0, /* exit_signal */
		0, /* stack (pointer to lowest byte) */
		0, /* stack size */
		0, /* tls */
		0, /* set_tid ptr to pid_t array */
		0, /* set_tid_size */
		0 /* cgroup */
	};
	usz pid;
	int own shared = mmap(
		NULL,
		sizeof(int),
		PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANONYMOUS,
		-1,
		0,
		&err
	);
	TRY_VOID(err);

	fprints("Running process clone test!\n", stderr, &err);
	TRY_VOID(err);

	pid = clone(&child_fn, shared, &cl_args, &err);
	TRY_VOID(err);

	fprints("Immediately in parent!\n", stderr, NULL);

	fprints("Child PID = ", stderr, NULL);
	fprintuz(pid, stderr, NULL);
	fprintc('\n', stderr, NULL);

	fprints("Shared value = ", stderr, NULL);
	fprinti(*shared, stderr, NULL);
	fprintc('\n', stderr, NULL);

	sleep(2, NULL);

	fprints("Doing parent stuff after two seconds!\n", stderr, NULL);

	fprints("Shared value = ", stderr, NULL);
	fprinti(*shared, stderr, NULL);
	fprintc('\n', stderr, NULL);

	munmap(shared, sizeof(int), NULL);
}

void
thread_demo(err_t ref err_out)
{
	err_t err = ERR_OK;
	struct clone_args cl_args = {
		0, /* flags */
		0, /* pidfd [ (int*) ] */
		0, /* child TID [ (pid_t*) ] */
		0, /* parent TID [ (pid_t*) ] */
		0, /* exit_signal */
		0, /* stack (pointer to lowest byte) */
		0, /* stack size */
		0, /* tls */
		0, /* set_tid ptr to pid_t array */
		0, /* set_tid_size */
		0 /* cgroup */
	};
	isz pid;
	int own shared;
	own_ptr stack;

	shared = alloc(sizeof(int), &err);
	TRY_VOID(err);
	stack = alloc(CHILD_STACK_SZ + 16, &err);
	TRY_VOID(err);

	fprints("Allocated child stack at 0x", stderr, &err);
	TRY_VOID(err);
	fprintp(stack, stderr, &err);
	TRY_VOID(err);
	fprintc('\n', stderr, &err);
	TRY_VOID(err);

	cl_args.flags = CLONE_VM | CLONE_THREAD | CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_SIGHAND | CLONE_SYSVSEM;
	cl_args.stack = (usz)stack;
	cl_args.stack_size = CHILD_STACK_SZ;

	fprints("Running thread clone test!\n", stderr, &err);
	TRY_VOID(err);

	pid = clone(child_fn, shared, &cl_args, &err);
	TRY_VOID(err);

	fprints("Immediately in parent!\n", stderr, NULL);

	fprints("Child PID = ", stderr, NULL);
	fprintuz(pid, stderr, NULL);
	fprintc('\n', stderr, NULL);

	fprints("Shared value = ", stderr, NULL);
	fprinti(*shared, stderr, NULL);
	fprintc('\n', stderr, NULL);

	sleep(2, NULL);

	fprints("Doing parent stuff after two seconds!\n", stderr, NULL);

	fprints("Shared value = ", stderr, NULL);
	fprinti(*shared, stderr, NULL);
	fprintc('\n', stderr, NULL);

	free(shared);
	free(stack);
}

int
main(void)
{
	err_t err = ERR_OK;
	ptr rbp;

	__asm__ volatile("mov %%rbp, %[rbp]" : [rbp] "=r" (rbp));

	fprints("Parent stack base at 0x", stderr, &err);
	fprintp(rbp, stderr, &err);
	fprintc('\n', stderr, &err);
	if (err != ERR_OK) {
		perror(err, "Error while printing stack base address");
		exit(-1);
	}

	fprints("\n=====\n\n", stderr, &err);
	if (err != ERR_OK) {
		perror(err, "Error while printing stack base address");
		exit(-1);
	}

	process_demo(&err);
	if (err != ERR_OK) {
		perror(err, "Error in process demo");
		exit(-1);
	}

	fprints("\n=====\n\n", stderr, &err);
	if (err != ERR_OK) {
		perror(err, "Error while printing stack base address");
		exit(-1);
	}

	thread_demo(&err);
	if (err != ERR_OK) {
		perror(err, "Error in thread demo");
		exit(-1);
	}

	return 0;
}
