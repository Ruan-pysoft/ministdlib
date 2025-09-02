# Features and Implementation

Here I will document the features of ministdlib,
along with their test coverage.

I will also document some of the process/decisions behind their implementation.

## Core

### Startup and exit

#### `_start`

A custom `_start` function is supplied to set up the environment
and run the user's `main` function,
which should have the signature `int main(void)`.

The `_start` function first retrieves `argc` and `argv` from the stack
and stores them to `static` variables (hidden from other compilation units).
It then aligns the stack pointer to 16 bytes
so that functions can be called without UB,
and finds `envp` based on `argv` and `argc`.
Once that is done,
it calls the `setup` function to initialise the runtime
and lastly calls the `main` function,
`exit`ing with the return code from it.

In `setup`, `envc` (the number of environment variables) is determined,
before setup functions from the various components of ministdlib is called.

#### `atexit`

An `atexit` function is supplied
for scheduling cleanup functions to run before program exit.

The `atexit` function simply adds a supplied function
(with the signature `void(void)`)
to an array of exit hooks.

Currently there can be at most 64 exit hooks,
as defined by the `EXITHOOKS_CAP` macro.
Some runtime initialisation functions register exit hooks.

These exit hooks are then run before the program exits.

Exit hooks are run in reverse order of registration,
so that resources allocated first
(and which therefore registered its cleanup function first)
will be cleaned up last
in order to not interfere with the cleanup of other resources
which might depend on the earlier resources still being available.

TODO I should guard the definition of `EXITHOOKS_CAP` in an `#ifdef` expression
to allow the user to make use of a greater number of exit hooks if desired.

##### Example

The following program should print
first `tatta!` then `sien jou later` on program exit.

```c
#include <ministd_fmt.h>

void
groet(void)
{
	prints("tatta!\n", NULL);
}

void
later(void)
{
	prints("sien jou later\n", NULL);
}

int
main(void)
{
	atexit(later);
	atexit(groet);

	return 0;
}
```

#### `exit` and `thread_exit`

Two program termination functions are provided:
`exit` and `thread_exit`.

`exit` runs all the exit hooks,
before terminating the current thread group with the `exit_group` Linux syscall.
Any process which does not share memory with its parent
should end execution with a call to `exit`.

`thread_exit` just immediately exits with the `exit` Linux syscall.
Any process which does share memory with its parent
should end execution with a call to `thread_exit`.

When you make use of the concurrency interface,
then the provided functions should automatically
call the appropriate exit function once the child process's function completes.

#### `panic` and `PANIC`

`panic` is a third termination function,
equivalent to `exit` except that no exit hooks are run.

This is used when resources required by the exit hooks might not be available.

##### Example

The following program should print only `baai!`,
and not `tatta!`.

```c
#include <ministd_fmt.h>

void
groet(void)
{
	prints("tatta!\n", NULL);
}

int
main(void)
{
	atexit(groet);

	prints("baai!\n", NULL);

	panic(0);

	return 0;
}
```

### Interacting with `argv` and `envp`

Command line arguments can be accessed through the `argv()` function,
with the number of command line arguments supplied by `argc()`.

`argv()` simply returns an array of strings (type `char ref ref`),
while `argc()` returns the length of said array (type `int`).

Environment variables can be accessed through `envp()`,
with the number of environment variables supplied by `envc()`.

`envp()` simply returns an array of strings (type `char ref ref`),
while `envp()` returns the length of said array (type `int`).

#### Example

The following program first lists all the command line arguments
and then all the environment variables:

```c
#include <ministd_fmt.h>

int
main(void)
{
	int i;

	prints("argv:\n", NULL);

	for (i = 0; i < argc(); ++i) {
		prints(" - ", NULL);
		prints(argv()[i], NULL);
		printc('\n', NULL);
	}

	prints("envp:\n", NULL);

	for (i = 0; i < envc(); ++i) {
		prints(" - ", NULL);
		prints(envp()[i], NULL);
		printc('\n', NULL);
	}

	return 0;
}
```

### Basic types

The basic types defined in `ministd_types.h` are:
`usz`, `isz`, `ptr`, `own_ptr`, and `bool`.

There are two values defined:
`NULL`, `true`, and `false`.

### `own` and `ref` pointers

There are two aliases to the pointer (`*`) to differentiate ownership:
`own` and `ref`.

### Errors interface

Errors are based around the `err_t` enumeration,
containing `ERR_OK` indicating a lack of an error,
various error codes from the Linux errno interface,
and a few custom errors added as its needed in the library.

It also provides five standard macros and three functions
for working with error codes:

 - `SET_ERR(err)`
 - `ERR_WITH(err, val)`
 - `ERR_VOID(err)`
 - `TRY_WITH(err, val)`
 - `TRY_VOID(err)`
 - `err_repr(err)`
 - `err_str(err)`
 - `perror(err, s)`

## IO

I/O revolves around the `FILE` interface,
which is forward declared in the prelude header
and defined in `ministd_io.h`.

...

## Concurrency

...

## Memory management

...

## Misc

#### `__stack_chk_fail`

`ministd.h` also provides a `__stack_chk_fail` function,
which is run whenever the program detects stack smashing
(don't ask me how that is done, that's entirely the compiler's job).

It just uses a syscall to write a message to standard error,
before exiting with the exit code 127.
