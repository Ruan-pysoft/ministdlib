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
child_fn(int ref arg)
{
	err_t err = ERR_OK;
	ptr rsp;

	__asm__ volatile("mov %%rsp, %[rsp]" : [rsp] "=r" (rsp));

	fprints("Immediately in child!\n", stderr, &err);
	if (err != ERR_OK) goto exit_error;

	sleep(1, &err);

	fprints("Child stack base at 0x", stderr, &err);
	if (err != ERR_OK) goto exit_error;
	fprintp(rsp, stderr, &err);
	if (err != ERR_OK) goto exit_error;
	fprintc('\n', stderr, &err);
	if (err != ERR_OK) goto exit_error;

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
forklike_demo(err_t ref err_out)
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
	usz cl_res;
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

	fprints("Running fork-like clone test!\n", stderr, &err);
	TRY_VOID(err);

	cl_res = _raw_clone(&cl_args, &err);
	TRY_VOID(err);

	if (cl_res == 0) {
		/* in child */
		exit(child_fn(shared));
	}

	/* in parent (cl_res != 0) */
	fprints("Immediately in parent!\n", stderr, NULL);

	fprints("Child PID = ", stderr, NULL);
	fprintuz(cl_res, stderr, NULL);
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
clonelike_demo(err_t ref err_out)
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
	isz cl_res;
	int own shared;
	own_ptr stack;

	shared = alloc(sizeof(int), &err);
	TRY_VOID(err);
	stack = alloc(CHILD_STACK_SZ, &err);
	TRY_VOID(err);

	fprints("Allocated child stack at 0x", stderr, &err);
	TRY_VOID(err);
	fprintp(stack, stderr, &err);
	TRY_VOID(err);
	fprintc('\n', stderr, &err);
	TRY_VOID(err);

	cl_args.flags = CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_SIGHAND | CLONE_SYSVSEM;
	cl_args.stack = (usz)stack;
	cl_args.stack_size = CHILD_STACK_SZ;

	fprints("Running clone-like clone test!\n", stderr, &err);
	TRY_VOID(err);

	_syscall_clone3(cl_res, &cl_args);
	if (cl_res < 0) err = -cl_res;
	TRY_VOID(err);

	if (cl_res == 0) {
		/* in child */
		thread_exit(child_fn(shared));
	}

	/* in parent (cl_res != 0) */
	fprints("Immediately in parent!\n", stderr, NULL);

	fprints("Child PID = ", stderr, NULL);
	fprintuz(cl_res, stderr, NULL);
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
	ptr rsp;

	__asm__ volatile("mov %%rsp, %[rsp]" : [rsp] "=r" (rsp));

	fprints("Parent stack base at 0x", stderr, &err);
	fprintp(rsp, stderr, &err);
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

	forklike_demo(&err);
	if (err != ERR_OK) {
		perror(err, "Error in fork-like demo");
		exit(-1);
	}

	fprints("\n=====\n\n", stderr, &err);
	if (err != ERR_OK) {
		perror(err, "Error while printing stack base address");
		exit(-1);
	}

	clonelike_demo(&err);
	if (err != ERR_OK) {
		perror(err, "Error in clone-like demo");
		exit(-1);
	}

	return 0;
}
