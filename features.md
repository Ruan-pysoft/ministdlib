# Features and Implementation

Here I will document the features of ministdlib,
along with their test coverage.

I will also document some of the process/decisions behind their implementation.

## Table of Contents

 1. [Prelude](#prelude-anchor)
 2. [Headers](#headers-anchor)
    1. [Core](#headers-core-anchor)
       1. [`<ministd.h>`](#ministd_h-anchor)
       2. [`<ministd_types.h>`](#ministd_types_h-anchor)
       3. [`<ministd_error.h>`](#ministd_error_h-anchor)
       4. [`<ministd_syscall.h>`](#ministd_syscall_h-anchor)
       5. [`<ministd_memory.h>`](#ministd_memory_h-anchor)
    2. [Data](#headers-data-anchor)
       1. [`<ministd_string.h>`](#ministd_string_h-anchor)
    3. [IO](#headers-io-anchor)
       1. [`<ministd_io.h>`](#ministd_io_h-anchor)
       2. [`<ministd_fmt.h>`](#ministd_fmt_h-anchor)
       3. [`<ministd_term.h>`](#ministd_term_h-anchor)
       4. [`<ministd_poll.h>`](#ministd_poll_h-anchor)
    4. [Concurrency](#headers-concurrency-anchor)
       1. [`<ministd_atomic.h>`](#ministd_atomic_h-anchor)
       2. [`<ministd_sched.h>`](#ministd_sched_h-anchor)
       2. [`<ministd_threads.h>`](#ministd_threads_h-anchor)
    5. [Misc](#headers-misc-anchor)
       1. [`<ministd_time.h>`](#ministd_time_h-anchor)
 3. [Core](#core-anchor)
    1. [Startup and exit](#startup_exit-anchor)
       1. [`_start`](#fn-_start-anchor)
       2. [`atexit`](#fn-atexit-anchor)
       3. [`exit` and `thread_exit`](#fn-exit-thread_exit-anchor)
       4. [`panic` and `PANIC`](#fn-panic-PANIC-anchor)
    2. [Interacting with `argv` and `envp`](#interacting_argv_envp-anchor)
    3. [Basic types](#basic_types-anchor)
    4. [Basic types](#own_ref-anchor)
    5. [Errors interface](#errors-anchor)
 4. [IO](#io-anchor)
 5. [Concurrency](#concurrency-anchor)
 6. [Memory management](#memory_management-anchor)
 7. [Misc](#misc-anchor)
    1. [`__stack_chk_fail`](#fn-__stack_chk-fail-anchor)

## Prelude <a name="prelude-anchor"></a>

The header `<_ministd_prelude.h>` provides a core set of functionality
that is available everywhere,
and it is imported at the start of every other header to ensure this.

As far as possible,
importing any header other than `<_ministd_prelude.h>` in a header is avoided.
This also assists with giving a uniform core API.
(eg. if you import `<ministd_fmt.h>` you can know
you won't get functions from `<ministd_io.h>`)

The prelude header imports the following headers:
 - `<ministd_types.h>` for core types used widely throughout the library
 - `<ministd.h>` providing basic functionality like `_start` and `exit` functions,
   and access to `argv` and `envp`, etc.
 - `<ministd_error.h>` for the library's error propagation/reporting interface
   which is widely used throughout the library

Furthermore, it defines a type alias `typedef struct FILE FILE;`
so that headers can use files without including `<ministd_io.h>`
(which also includes the definition of the `FILE` struct,
to allow rudimentary polymorphism
by having the first member of a struct be a `struct FILE`,
thereby having the same initial bytes as a `FILE` struct)

TODO shouldn't the `FILE` typedef be moved to `<ministd_types.h>`?

## Headers <a name="headers-anchor"></a>

### Core <a name="headers-core-anchor"></a>

#### `<ministd.h>` <a name="ministd_h-anchor"></a>

_STABLE_

**Purpose**: startup/exit functionality

Basic functions to do with startup, exit, and interacting with the environment.

#### `<ministd_types.h>` <a name="ministd_types_h-anchor"></a>

_STABLE_

**Purpose**: expose unified type interface

Includes the definitions of basic types used throughout the library,
such as the boolean type (which is not a C builtin type).

Also includes the `#define`s for the different pointer types
used for ownership tracking,
more on that later.

#### `<ministd_error.h>` <a name="ministd_error_h-anchor"></a>

_STABLE_

**Purpose**: define unified error interface

Defines the error propagation/reporting interface of the library.

#### `<ministd_syscall.h>` <a name="ministd_syscall_h-anchor"></a>

_SEMI-INTERNAL_

**Purpose**: provide wrappers for calling syscalls

Defines macros `_syscall0` through `_syscall6`,
as well as including `<asm/unistd.h>`
where the Linux syscall numbers are defined.

Mostly used internally, and has no source file associated.

Originally it was defined as a public header,
since the library is by no means fully-featured
and the user might on occasion have to delve into calling syscalls manually;
however I am considering moving it to `<_ministd_syscall.h>` to mark it internal
as the library is getting more full-featured.
This should especially be done if/when multi-platform support is added.

#### `<ministd_memory.h>` <a name="ministd_memory_h-anchor"></a>

_TENTATIVE_

**Purpose**: provide memory allocation and management facilities

Provides memory management functionality.

Provides a suite of functions for allocating and freeing memory,
as well as functions for moving and zeroing memory.

Should be expanded at some point to contain `memset` equivalents and the like.

Additionally provides an `Allocator` interface,
which allows the implementation of custom allocators.

Unfortunately, support for the use of custom allocators
is not widespread through the rest of the library,
so it is mostly a curiosity at the current point in time.

### Data <a name="headers-data-anchor"></a>

#### `<ministd_string.h>` <a name="ministd_string_h-anchor"></a>

_REDESIGN PENDING_

**Purpose**: provide dynamic strings

Provides some basic cstring functions
as well as an automatically-managed growable string type.

Currently it is vaguely based on plan9's `string.h`,
but it was made with little planning or documentation
and is currently something of a mess.

At some point it will be redesigned,
and I recommend avoiding its use until then.

### IO <a name="headers-io-anchor"></a>

#### `<ministd_io.h>` <a name="ministd_io_h-anchor"></a>

_STABLE_

**Purpose**: provide basic io interface

The basic IO interface used in the library.

Defines the `FILE` struct,
as well as a few different functions for working with files.

Also provides the `stdin`, `stdout`, and `stderr` files.

The `FILE` struct is designed to be extensible,
in order to allow the same IO functions to be used with,
for example,
raw files, buffered files, string files, sockets, etc.

(Note that sockets aren't a supported feature yet,
but the point is that they could easily be added
without touching the code in `ministd_io.c` or `<ministd_io.h>`)

#### `<ministd_fmt.h>` <a name="ministd_fmt_h-anchor"></a>

_TENTATIVE_

**Purpose**: provide formatted io interface

Formatted IO built on top of `<ministd_io.h>`.

Note that none of the low-level details in `<ministd_io.h>` is included
when including `<ministd_fmt.h>`,
though the `stdin`, `stdout`, and `stderr` files are also exposed here.

A suite of `printf`-like and `scanf`-like functions are provided,
but modified to fit with the ministdlib's way of doing things.
It also does not make use of varargs,
which makes typing a lot more difficult
(and moves some of it over to the runtime).

My initial motivation for eschewing variadics
probably also involved me not wanting to
spend the time to learn how to use variadics in C,
especially without stdlib support...

#### `<ministd_term.h>` <a name="ministd_term_h-anchor"></a>

_DEVELOPING_

**Purpose**: facilitate formatted terminal output

Contains various functions and constants
for writing formatted output to the terminal.

#### `<ministd_poll.h>` <a name="ministd_poll_h-anchor"></a>

_STABLE_

**Purpose**: expose polling capabilities for file descriptors

Provides polling functionality to poll file descriptors.

### Concurrency <a name="headers-concurrency-anchor"></a>

#### `<ministd_atomic.h>` <a name="ministd_atomic_h-anchor"></a>

_STABLE_

**Purpose**: provide atomic types/operations

Provides ten different atomics,
five different integer sizes, both signed and unsigned.

It also provides functions for working with atomics,
along with the `memory_fence` function.

Protip: If you want to be naughty,
you can use the fact that each atomic type
is just a thin wrapper over the corresponding integer type,
and that this design is unlikely to change in the future
to do some pointer typecasting and interact directly with the raw bytes.

#### `<ministd_sched.h>` <a name="ministd_sched_h-anchor"></a>

_STABLE_

**Purpose**: provide support for spawning subprocesses

Currently just provides a wrapper around the Linux `clone3` syscall.

(I *would* call it a thin wrapper,
except I spent several weeks working on it
and it was quite tricky to get right in the end.
Turns out just doing half of it in assembly
was easier than fighting my C compiler,
who knew?)

#### `<ministd_threads.h>` <a name="ministd_threads_h-anchor"></a>

_BUDDING_

**Purpose**: provide support for multithreading

Currently just provides a mutex lock.

In the future, I plan on also supporting futexes and threadpools
through this header too.

### Misc <a name="headers-misc-anchor"></a>

#### `<ministd_time.h>` <a name="ministd_time_h-anchor"></a>

_BUDDING_

**Purpose**: provide timing and waiting functionality

Contains some functions to support sleeping,
including a more low-level `nanosleep`
which stays close to the way the syscall works for maximum control,
and more high-level functions like `sleep` which handles details automatically.

## Core <a name="core-anchor"></a>

### Startup and exit <a name="startup_exit-anchor"></a>

#### `_start` <a name="fn-_start-anchor"></a>

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

#### `atexit` <a name="fn-atexit-anchor"></a>

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

#### `exit` and `thread_exit` <a name="fn-exit-thread_exit-anchor"></a>

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

#### `panic` and `PANIC` <a name="fn-panic-PANIC-anchor"></a>

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

### Interacting with `argv` and `envp` <a name="interacting_argv_envp-anchor"></a>

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

### Basic types <a name="basic_types-anchor"></a>

The basic types defined in `ministd_types.h` are:
`usz`, `isz`, `ptr`, `own_ptr`, and `bool`.

There are two values defined:
`NULL`, `true`, and `false`.

### `own` and `ref` pointers <a name="own_ref-anchor"></a>

There are two aliases to the pointer (`*`) to differentiate ownership:
`own` and `ref`.

### Errors interface <a name="errors-anchor"></a>

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

## IO <a name="io-anchor"></a>

I/O revolves around the `FILE` interface,
which is forward declared in the prelude header
and defined in `ministd_io.h`.

...

## Concurrency <a name="concurrency-anchor"></a>

...

## Memory management <a name="memory_management-anchor"></a>

...

## Misc <a name="misc-anchor"></a>

### `__stack_chk_fail` <a name="fn-__stack_chk_fail-anchor"></a>

`ministd.h` also provides a `__stack_chk_fail` function,
which is run whenever the program detects stack smashing
(don't ask me how that is done, that's entirely the compiler's job).

It just uses a syscall to write a message to standard error,
before exiting with the exit code 127.
