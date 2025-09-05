# Features and Implementation

Here I will document the features of ministdlib,
along with their test coverage.

I will also document some of the processes/decisions behind their implementation.

Note that I do not provide the full type signature of each function;
to find that navigate to the header I indicated the function can be found in
(located in the `include/` directory)
and search for the name of the function.

Keep in mind that this is also an exercise in design and creativity for myself,
so much of the API differs greatly from standard libc,
even though many other parts take great inspiration from
(or outright copies) it.
As such, I recommend reading the documentation on functions you wish to use
in order to make sure you understand what they actually do,
rather than assuming they work the same as a similarly-named libc one.

## Prelude

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

## Design guidelines

All functions that may fail should take `err_t ref err_out` as an argument
(with that type signature *and* argument name).
It is recommended that this should always be the *last* argument
for a consistent API.
The function should handle `err_out` being `NULL`
when the caller has no interest in handling errors.

Use `own` or `ref` rather than `*` when declaring pointer types.
This is to facilitate ownership-tracking to make memory management clearer.
For more information on the precise usage of `own` and `ref`,
see the "`own` and `ref` pointers" section.
Note that the correct usage of `own` and `ref`
is currently merely a matter of convention,
and is not enforced or checked automatically in any way.

The `main` function should have the signature `int main(void)`
as expected by `_start`.
Note also that because programs are compiled with `-ffreestanding`
the compiler does not recognise the `main` function as a `main` function,
so there is no implicit `return 0`.

## Headers

Here is a quick summary of all the user-facing headers supplied;
more complete descriptions of each header follows.

<table>
<thead>
  <tr> <th>header</th> <th>status</th> <th>summary</th> </tr>
</thead>
<tbody>
  <tr> <td><code>&lt;ministd.h&gt;</code></td> <td>stable</td> <td>startup/exit functionality</td> </tr>
  <tr> <td><code>&lt;ministd_types.h&gt;</code></td> <td>stable</td> <td>expose unified type interface</td> </tr>
  <tr> <td><code>&lt;ministd_error.h&gt;</code></td> <td>stable</td> <td>define unified error interface</td> </tr>
  <tr> <td><code>&lt;ministd_syscall.h&gt;</code></td> <td>semi-internal</td> <td>provide wrappers for calling syscalls</td> </tr>
  <tr> <td><code>&lt;ministd_memory.h&gt;</code></td> <td>tentative</td> <td>provide memory allocation and management facilities</td> </tr>
  <tr> <td><code>&lt;ministd_string.h&gt;</code></td> <td>redesign pending</td> <td>provide dynamic strings</td> </tr>
  <tr> <td><code>&lt;ministd_io.h&gt;</code></td> <td>stable</td> <td>provide basic io interface</td> </tr>
  <tr> <td><code>&lt;ministd_fmt.h&gt;</code></td> <td>tentative</td> <td>provide formatted io interface</td> </tr>
  <tr> <td><code>&lt;ministd_term.h&gt;</code></td> <td>developing</td> <td>facilitate formatted terminal output</td> </tr>
  <tr> <td><code>&lt;ministd_poll.h&gt;</code></td> <td>stable</td> <td>expose polling capabilities for file descriptors</td> </tr>
  <tr> <td><code>&lt;ministd_atomic.h&gt;</code></td> <td>stable</td> <td>provide atomic types/operations</td> </tr>
  <tr> <td><code>&lt;ministd_sched.h&gt;</code></td> <td>stable</td> <td>provide support for spawning subprocess</td> </tr>
  <tr> <td><code>&lt;ministd_threads.h&gt;</code></td> <td>budding</td> <td>provide support for multithreading</td> </tr>
  <tr> <td><code>&lt;ministd_time.h&gt;</code></td> <td>budding</td> <td>provide timing and waiting functionality</td> </tr>
</tbody>
</table>

### Core

#### `<ministd.h>`

_STABLE_

**Purpose**: startup/exit functionality

Basic functions to do with startup, exit, and interacting with the environment.

#### `<ministd_types.h>`

_STABLE_

**Purpose**: expose unified type interface

Includes the definitions of basic types used throughout the library,
such as the boolean type (which is not a C builtin type).

Also includes the `#define`s for the different pointer types
used for ownership tracking,
more on that later.

#### `<ministd_error.h>`

_STABLE_

**Purpose**: define unified error interface

Defines the error propagation/reporting interface of the library.

#### `<ministd_syscall.h>`

_SEMI-INTERNAL_

**Purpose**: provide wrappers for calling syscalls

**NOTE**: meant for internal use,
will likely hide later and implementation details might change without warning
(but that might happen anyways bc this whole project is just me playing around)

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

#### `<ministd_memory.h>`

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

### Data

#### `<ministd_string.h>`

_REDESIGN PENDING_

**Purpose**: provide dynamic strings

Provides some basic cstring functions
as well as an automatically-managed growable string type.

Currently it is vaguely based on plan9's `string.h`,
but it was made with little planning or documentation
and is currently something of a mess.

At some point it will be redesigned,
and I recommend avoiding its use until then.

### IO

#### `<ministd_io.h>`

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

#### `<ministd_fmt.h>`

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

#### `<ministd_term.h>`

_DEVELOPING_

**Purpose**: facilitate formatted terminal output

Contains various functions and constants
for writing formatted output to the terminal.

#### `<ministd_poll.h>`

_STABLE_

**Purpose**: expose polling capabilities for file descriptors

Provides polling functionality to poll file descriptors.

### Concurrency

#### `<ministd_atomic.h>`

_STABLE_

**Purpose**: provide atomic types/operations

Provides ten different atomics;
five different integer sizes, both signed and unsigned.

It also provides functions for working with atomics,
along with the `memory_fence` function.

Pro tip: If you want to be naughty,
you can use the fact that each atomic type
is just a thin wrapper over the corresponding integer type,
and that this design is unlikely to change in the future,
to do some pointer typecasting and interact directly with the raw bytes.

#### `<ministd_sched.h>`

_STABLE_

**Purpose**: provide support for spawning subprocesses

Currently just provides a wrapper around the Linux `clone3` syscall.

(I *would* call it a thin wrapper,
except I spent several weeks working on it
and it was quite tricky to get right in the end.
Turns out just doing half of it in assembly
was easier than fighting my C compiler,
who knew?)

#### `<ministd_threads.h>`

_BUDDING_

**Purpose**: provide support for multithreading

Currently just provides a mutex lock.

In the future, I plan on also supporting futexes and threadpools
through this header too.

### Misc

#### `<ministd_time.h>`

_BUDDING_

**Purpose**: provide timing and waiting functionality

Contains some functions to support sleeping,
including a more low-level `nanosleep`
which stays close to the way the syscall works for maximum control,
and more high-level functions like `sleep` which handles details automatically.

## Core

### Startup and exit

#### `_start`

**Provided by**: `<ministd.h>`

_Part of the prelude_

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

**Provided by**: `<ministd.h>`

_Part of the prelude_

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

**Provided by**: `<ministd.h>`

_Part of the prelude_

Three program termination functions are provided:
`exit`, `thread_exit`, and `panic`.

`panic` is documented in the next section.

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

**Provided by**: `<ministd.h>`

_Part of the prelude_

`panic` is a third termination function,
equivalent to `exit` except that no exit hooks are run.

This is used when resources required by the exit hooks might not be available.

`PANIC` is a macro,
which simply runs `panic`
and places the `__builtin_unreachable` compiler builtin right after,
which should allow for some compiler optimisations.

It is recommended that you use `PANIC` rather than `panic`.

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

	PANIC(0);

	return 0;
}
```

### Interacting with `argv` and `envp`

**Provided by**: `<ministd.h>`

_Part of the prelude_

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

**Provided by**: `<ministd_types.h>`

_Part of the prelude_

The basic types defined in `ministd_types.h` are:
`usz`, `isz`, `ptr`, `own_ptr`, and `bool`.

`usz` and `isz` are signed and unsigned 64-bit integers
(typedef'd `unsigned long long` and `long long`).

`ptr` and `own_ptr` are simply aliases to `void ref` and `void own`,
to be used when a generic pointer is required (no type information).

`bool` is simply an `int`,
but logically functions as a boolean type.
When working with conditionals,
it is recommended to use `bool` rather than int,
just as documentation/to provide clarity.
Booleans should also only be assigned the values `true` or `false`,
rather than an arbitrary integer value,
though this is in no way enforced.

There are three values defined:
`NULL`, `true`, and `false`.

`NULL` is the null pointer, `((void*)0)`.

`true` and `false` are just aliases for `1` and `0` respectively.

### `own` and `ref` pointers

**Provided by**: `<ministd_types.h>`

_Part of the prelude_

There are two aliases to the pointer (`*`) to differentiate ownership:
`own` and `ref`.

An `own` pointer is a pointer to owned memory,
meaning that the function is responsible for cleanup.

`ref`, meanwhile, is a pointer to non-owned memory.

If a function declares an `own` pointer or receives an `own` pointer,
then it _has_ to do one of the following _exactly once_ in each branch of execution,
unless it is the null pointer:
 - return it, if the return type is also marked `own`
 - pass it to another function as an `own` parameter
 - assign it as an `own` member of a struct,
   cleaning up the previous value if need be
The only exception to these requirements are memory-freeing functions,
which in some sense are "sinks",
the last place that handles all `own` pointers eventually.

`own` pointers can be passed as `ref` parameters,
assigned to `ref` pointers, etc,
but those are just copies;
the original `own` must still be handled and not just discarded.

Similarly, you cannot do the following to a `ref` pointer:
 - return it as an `own` pointer
 - pass it as an `own` parameter
 - assign it to an `own` member of a struct
 - assign it to a variable declard as an `own` pointer

TODO create an example here
(not quite sure how I'd go about it,
but demonstrate what should and shouldn't be done with `own`s and `ref`s)

### Errors interface

**Provided by**: `<ministd_error.h>`

_Part of the prelude_

Errors are based around the `err_t` enumeration,
containing `ERR_OK` indicating a lack of an error,
various error codes from the Linux errno interface,
and a few custom errors added as its needed in the library.

**IMPORTANT**: whenever an `err_t` variable is declared,
**always** initialise it with `ERR_OK`:

```c
err_t err = ERR_OK; /* this is good */
...
err_t err; /* this is *bad* */
```

It also provides five macros and three functions
for working with error codes:

 - `SET_ERR(err)`
 - `ERR_WITH(err, val)`
 - `ERR_VOID(err)`
 - `TRY_WITH(err, val)`
 - `TRY_VOID(err)`
 - `err_repr(err)`
 - `err_str(err)`
 - `perror(err, s)`

All functions that can error MUST take an `err_t ref err_out` parameter;
note that the name is `err_out`,
which is required to use the various error propagation macros!
Furthermore, for a consistent, clean interface,
it is recommended to take this as the *last* parameter of the function.

When calling a function that could error,
the consumer may pass a pointer to an `err_t` to handle potential errors,
or pass `NULL` to ignore any errors.

If such a function runs without any error,
it should leave the `err_out` argument untouched,
meaning that if a pointer to an `err_t` initialised with `ERR_OK` was passed
it will still be `ERR_OK` after the function has run.

If such a function does error **and** it has not been passed `NULL` for `err_out`,
then it should write an error code *other than* `ERR_OK` to `err_out`.
Therefore, if a pointer to an `err_t` was passed
it will now contain the error code indicating what went wrong.

#### Error setting and propagation

Since it requires quite some boilerplate
to check the `err_out` argument against `NULL`
before touching it each time,
the five error-handling macros are provided:

`SET_ERR(err)` sets `err_out` to `err` if it is not `NULL`.
That is, it is equivalent to `if (err_out != NULL) *err_out = err;`.

`ERR_WITH(err, val)` sets `err_out` before returning `val`.
That is, it is equivalent to `SET_ERR(err); return val;`.

`ERR_VOID(err)` sets `err_out` before returning from a void function.
That is, it is equivalent to `SET_ERR(err); return;`.

`TRY_WITH(err, val)` checks if `err` is not `ERR_OK`,
and errors with `val` if so.
That is, it is equivalent to `if (err != ERR_OK) ERR_WITH(err, val);`.

`TRY_VOID(err)` checks if `err` is not `ERR_OK`,
and errors if so.
That is, it is equivalent to `if (err != ERR_OK) ERR_VOID(err);`.

#### Error display

Three functions are provided for displaying errors.

`err_repr` returns a `const char ref`
containing the enum variant used for that error number.
For example, `err_repr(ERR_OK)` returns `"ERR_OK"`
and `err_repr(ERR_OVERFLOW)` returns `"ERR_OVERFLOW"`.

`err_str` returns a `const char ref` containing a description of the error.
For example, `err_str(ERR_OK)` returns `"No error"`,
and `err_repr(ERR_OVERFLOW)` returns `"Operation would result in overflow"`.

`perror` takes an error and optional message (pass `NULL` for no message),
and displays the message first (if provided)
and then the `err_str` of the error to stderr.
So `perror(ERR_OVERFLOW, "doing some math")`
would display `doing some math: operation would result in overflow`.

#### Reasoning behind design

The error design for the library is meant to be
somewhat reminiscent of golang's error handling,
though I have never programmed in go myself,
so it could be way off.

I have considered various different error handling methods
before settling on the current model.

First, I dislike libc's method of handling errors,
where some special value is returned from a function
which indicates an error,
and then more information is stored in a global `errno` variable.

One problem with this method is
that it is very easy to ignore errors implicitly;
how many times have you actually checked the return value of `printf`
so that you may handle an IO error?
And when you don't,
how aware are you of the fact that the call to `printf` might fail?

The second big problem is that it is hard to make this thread safe,
unless you make `errno` an atomic value from the start or something,
and even then you now have the problem
that if multiple functions have an error,
only the error code of one of them will be captured.

Another method which I didn't really consider
is using setjmp/longjmp to essentially create an exceptions mechanism.
The long and the short is that I don't really know how to use setjmp/longjmp
(and I didn't want to spend time learning how to use it at the time)
and I really dislike the exception model of error handling anyways.

An error handling mechanism I found quite attractive
is Rust's result types,
however I could not work out how to practically do this
without introducing a ton of boilerplate and possible overhead,
so in the end I decided against this.

In the end I emulated go's method which, as far as I'm aware,
consists of returning both the function's value and an error object,
with a `nil` object indicating no error,
and error handling is done with a simple `if err != nil`.
They then make you explicitly discard the error
if you do not want to handle it.

C, however, provides no easy method for having multiple return values,
nor does it have a mechanism
that prevents you from implicitly ignoring a return value.
So, to imitate go's method,
I decided to instead take a pointer to the error object
as the last argument of a function,
allowing the error to be explicitly ignored by passing the `NULL` pointer.

This leads to a very similar error handling method,
though I did provide some macros to work around the boilerplate
of checking if the error argument is `NULL`:

```go
// how error handling is done in Go
val, err := someFunc()
if err != nil {
	...
}
// and discarding an error:
val, _ := errorIgnored()
...
```

```c
/* similar interface in c */
type val = someFunc(&err);
if (err != ERR_OK) {
	...
}
/* and similarly, ignoring an error in c: */
type val = errorIgnored(NULL);
...
```

#### Example

```c
#include <ministd_io.h>
#include <ministd_fmt.h>

void
no_error(err_t ref err_out)
{
	int the_answer;

	the_answer = 6;
	the_answer *= 7;

	if (the_answer != 42) {
		/* Something went very wrong! */
		ERR_VOID(ERR_AGAIN);
	}

	prints("Look ma, no errors!\n", NULL);

	/* What's the question though? */
}

void
somethings_wrong(err_t ref err_out)
{
	char msg[] = "this file doesn't exist!";

	/* let's try writing to a random file! */
	fd_write(10, msg, sizeof(msg)/sizeof(*msg), err_out);

	/* note that I'm not doing more stuff after the call,
	 * so I can just provide the function with the same err_out variable
	 * and not worry about error handling or early returns
	 */
}

int
main(void)
{
	err_t err = ERR_OK;

	prints("Hello, world!\n", &err);
	if (err != ERR_OK) {
		perror(err, "While printing hello world");
		return 1;
	}

	no_error(&err);
	if (err != ERR_OK) {
		perror(err, "While running no_error");
		return 1;
	}

	somethings_wrong(&err);
	if (err != ERR_OK) {
		perror(err, "While running somethings_wrong");
		return 1;
	}

	return 0;
}
```

### Syscall wrapper

**NOTE**: currently this wrapper is publicly exposed through `<ministd_syscall.h>`,
but I will likely hide it as an implementation detail in the future
(perhaps by moving it to `<_ministd_syscall.h>`)

`<ministd_syscall.h>` imports the definitions of the Linux syscall numbers from `<asm/unistd.h>`,
and defines seven syscall macros,
from `_syscall0` to `_syscall6`,
where the number indicates the number of arguments the syscall takes.

To use the macros,
provide the syscall number as the first argument,
the output variable as the second argument,
and the syscall's arguments after those two.

For the most part, the macros are thin wrappers over `__asm__ volatile` calls
which calls the syscall instruction,
as well as some supporting code
to move the arguments into the correct registers.

### Memory management

#### Allocating and freeing – `alloc`, `nalloc`, `realloc`, `nrealloc`, `free`

**Provided by**: `<ministd_memory.h>`

`alloc` allocates a certain amounts of bytes,
returning the allocated memory.

`nalloc` allocates enough memory for n copies of size bytes.
It is essentially equivalent to `alloc(size*n, err_out)`
with some overflow protection.

`realloc` takes a pointer to allocated memory,
and resizes the allocated memory to fit the provided number of bytes.
If passed `NULL` as the pointer,
it acts as `alloc`.
If told to resize to 0 bytes,
it frees the memory.

`nrealloc` functions similarly to `nalloc`,
but reallocating memory rather than doing the initial allocation.

`free` takes a pointer to allocated memory and releases the memory again.

#### Copying and setting – `memzero`, `nmemzero`, `memmove`, `nmemmove`

**Provided by**: `<ministd_memory.h>`

`memzero` takes a pointer to some memory and a size in bytes
and zeroes out that memory.

`nmemzero` does the same,
taking the size of a single element and the number of elements.

`memmove` takes a pointer to a destination and a source,
as well as the number of bytes to copy over.
It then copies the memory from the source into the destination.

**NOTE**: `memmove` expects the memory regions to be non-overlapping!

`nmemmove` does the same as `memmove`,
taking the size of a single element and the number of elements.

#### Allocator interface – `Allocator` and `Allocator_alloc_t`, `Allocator_realloc_t`, `Allocator_free_t`

**Provided by**: `<ministd_memory.h>`

`Allocator` is, just like file,
a collection of pointers to an allocator's functions,
allowing the implementation of custom allocators.

It needs three functions:

`Allocator_alloc_t` which allocates a certain amount of bytes of memory.
When told to allocate zero bytes,
it should return the `NULL` pointer.

`Allocator_realloc_t` grows or shrinks the allocated memory
to the provided new size.
When passed a `NULL` pointer,
it should just allocate new memory.
When told to reallocate to zero bytes,
it should free the memory.

`Allocator_free_t` releases the allocated memory again.

#### Allocator convenience functions – `a_alloc`, `a_nalloc`, `a_realloc`, `a_nrealloc`, `a_free`

**Provided by**: `<ministd_memory.h>`

`a_alloc`, `a_realloc`, and `a_free` are equivalent to `alloc->alloc(alloc, bytes, err_out)`, `alloc->realloc(alloc, buf, bytes, err_out)`, and `alloc->free(alloc, buf)`.

`a_nalloc` and `a_nrealloc` are equivalent to `a_alloc(alloc, size*n, err_out)` and `a_realloc(alloc, buf, size*n, err_out)`,
but with some overflow protection added.

## Data

### Strings support

#### Cstring operations – `strlen`, `strnlen`

**Provided by**: `<ministd_string.h>`

#### String and StringView types – `String`, `StringView`

**Provided by**: `<ministd_string.h>`

#### String macros – `s_to_c`, `s_len`, `s_clone`, `s_as_sv`, `sv_to_c`, `sv_len`, `sv_clone`

**Provided by**: `<ministd_string.h>`

#### Creating and destroying strings – `s_new`, `s_free`, `s_newalloc`, `sv_new`, `s_frombuf`

**Provided by**: `<ministd_string.h>`

#### String management – `s_grow`, `s_terminate`, `s_reset`, `s_restart`, `s_copy`

**Provided by**: `<ministd_string.h>`

#### String writing – `s_putc`, `s_append`, `s_nappend`, `s_memappend`

**Provided by**: `<ministd_string.h>`

#### String modification – `s_tolower`, `s_toupper`

**Provided by**: `<ministd_string.h>`

#### String parsing – `s_parse`

**Provided by**: `<ministd_string.h>`

#### String files – `StringFile` and `sf_open`

**Provided by**: `<ministd_string.h>`

## IO

I/O revolves around the `FILE` interface,
which is forward declared in the prelude header
and defined in `ministd_io.h`.

### Basic file interface

#### `struct FILE`, `enum FILE_OP` and `FILE_read_t`, `FILE_write_t`, `FILE_close_t`, `FILE_run_t`

**Provided by**: `<ministd_io.h>`

The IO interface of ministdlib is built around the `FILE` struct.
The `FILE` struct is, for the most part, a table of file pointers
which are used to run operations on a file object.

Along with that, it also comes with a single character buffer
to support "un-getting" a character from input files.

There are four required function pointers:

 - `FILE_read_t` takes in the file, a mutable buffer with a size, and an `err_out` parameter, returning `usz`

   The read function will attempt
   to read up to `cap` number of bytes into the buffer,
   returning the number of bytes read.

 - `FILE_write_t` takes in the file, a constant buffer with a size, and an `err_out` parameter, returning `usz`

   The write function will attempt
   to write up to `cap` number of bytes from the buffer to the file,
   returning the number of bytes written.

 - `FILE_close_t` takes in only the file and an `err_out` parameter, not returning a value

   The close function should close the file
   and clean up any resources it might own.
   After closing, a file's memory may be deallocated.

 - `FILE_run_t` takes the file, a `FILE_OP`, and an `err_out` parameter, returning `usz`

   The run function's behaviour depends on the operation it is instructed to run.

There are currently three different file operations:

 - `FO_FLUSH` flushes a buffered file, does nothing otherwise
 - `FO_HASFD` returns `1` if the file has a file descriptor, `0` otherwise
 - `FO_GETFD` returns the file's file descriptor if it exists,
   otherwise it errors with `ERR_BADF`

Here follows an example of how to create your own `FILE`,
for more examples see `src/ministd_io.h`
for the implementations of `BufferedFile` and `LockedFile`,
or `src/ministd_string.h` for the implementation of `StringFile`.

When run the program should exit basically immediately,
producing no output.

```c
#include <ministd_io.h>
#include <ministd_fmt.h>
#include <ministd_memory.h>

struct MemoryFile {
	FILE ptrs;
	ptr memory;
	usz size;
	usz read_pos;
	usz write_pos;
};

static usz mf_read(struct MemoryFile ref this, ptr buf, usz cap,
		   err_t ref err_out);
static usz mf_write(struct MemoryFile ref this, const ptr buf, usz cap,
		    err_t ref err_out);
static void mf_close(struct MemoryFile ref this, err_t ref err_out);
static usz mf_run(struct MemoryFile ref this, enum FILE_OP op,
		  err_t ref err_out);
static FILE mf_pointers = {
	(FILE_read_t)mf_read,
	(FILE_write_t)mf_write,
	(FILE_close_t)mf_close,
	(FILE_run_t)mf_run,
	0, false,
};

static usz
mf_read(struct MemoryFile ref this, ptr buf, usz cap, err_t ref err_out)
{
	usz read_bytes = cap;

	/* errors not possible */
	(void) err_out;

	if (read_bytes > this->size - this->read_pos) {
		read_bytes = this->size - this->read_pos;
	}

	memmove(buf, (char ref)this->memory + this->read_pos, read_bytes);

	this->read_pos += read_bytes;

	return read_bytes;
}
static usz
mf_write(struct MemoryFile ref this, const ptr buf, usz cap, err_t ref err_out)
{
	usz write_bytes = cap;

	if (write_bytes > this->size - this->write_pos) {
		write_bytes = this->size - this->write_pos;
	}

	if (write_bytes == 0) {
		ERR_WITH(ERR_IO, 0);
	}

	memmove((char ref)this->memory + this->write_pos, buf, write_bytes);

	this->write_pos += write_bytes;

	return write_bytes;
}
static void
mf_close(struct MemoryFile ref this, err_t ref err_out)
{
	/* nothing to do,
	 * if this->memory was owned we'd free it here
	 */

	(void) this;
	(void) err_out;
}
static usz
mf_run(struct MemoryFile ref this, enum FILE_OP op, err_t ref err_out)
{
	usz r = 0;

	(void) this;

	switch (op) {
		case FO_FLUSH: break; /* no-op */
		case FO_HASFD: {
			r = false; /* does not have file descriptor */
		break; }
		case FO_GETFD: {
			ERR_WITH(ERR_BADF, 0); /* no file descriptor to get */
		break; }
	}

	return r;
}

struct MemoryFile own mf_new(ptr buf, usz bufcap, usz write_at,
			     err_t ref err_out)
{
	err_t err = ERR_OK;
	struct MemoryFile own res;

	res = alloc(sizeof(*res), &err);
	TRY_WITH(err, NULL);

	res->ptrs = mf_pointers;
	res->memory = buf;
	res->size = bufcap;
	res->read_pos = 0;
	res->write_pos = write_at;

	return res;
}

int
main(void)
{
	err_t err = ERR_OK;
	FILE own file;
	char buffer[1024] = "123";

	file = (FILE own)mf_new(buffer, 1024, 3, &err);
	if (err != ERR_OK) {
		free(buffer);

		perror(err, "while creating file");
		return 1;
	}

	fprintc(' ', file, &err);
	fprinti(-42, file, &err);
	if (err != ERR_OK) {
		close(file, NULL);
		free(file);
		free(buffer);

		perror(err, "while writing to file");
		return 1;
	}

	/* buffer should now contain "123 -42" */

	if (fscani(file, &err) != 123) {
		close(file, NULL);
		free(file);
		free(buffer);

		fprints("didn't read correct value from file!\n", stderr, NULL);
		if (err != ERR_OK) perror(err, "while reading from file");
		return 1;
	}
	if (fscani(file, &err) != -42) {
		close(file, NULL);
		free(file);
		free(buffer);

		fprints("didn't read correct value from file!\n", stderr, NULL);
		if (err != ERR_OK) perror(err, "while reading from file");
		return 1;
	}

	close(file, &err);
	free(file);
	free(buffer);
	if (err != ERR_OK) {
		perror(err, "while closing file");
		return 1;
	}

	return 0;
}
```

#### Low-level IO – `read`, `write`

**Provided by**: `<ministd_io.h>`

`read` and `write` both take a file, a buffer, the size of the buffer, and `err_out`.
They are thin wrappers over
`file->read(file, buf, cap, err_out)` and `file->write(file, buf, cap, err_out)`.

`read` takes a mutable buffer,
and tries to read up to `cap` bytes of data from `file` into `buf`,
returning the number of bytes read.

`write` takes a constant buffer,
and tries to write up to `cap` bytes of data from `buf` to `file`,
returning the number of bytes written.

#### Other operations – `close`, `run_op`, `flush`, `ungetc`, `peekc`

**Provided by**: `<ministd_io.h>`

`close` is a thin wrapper over `file->close(file, err_out)`.
It cleans up any resources owned by the file,
and closes the file.
After closing it is safe to deallocate the file's memory,
and the file should not be used again after calling `close` on it.

`run_op` is a thin wrapper over `file->run(file, op, err_out)`.

There are currently three different file operations:

 - `FO_FLUSH` flushes a buffered file, does nothing otherwise
 - `FO_HASFD` returns `true` if the file has a file descriptor, `false` otherwise
 - `FO_GETFD` returns the file's file descriptor if it exists,
   otherwise it errors with `ERR_BADF`

`flush` is a thin wrapper over `file->run(file, FO_FLUSH, err_out)`,
and it simply flushes a buffered file's buffers.

`ungetc` stores a character in a file's `ungot` buffer,
erroring with `ERR_PERM` if there is already an ungotten character.

`peekc` tries to "peek" into the file,
returning the peeked character as an `int`,
returning `-1` if there is no data to be read from the file
(that is, there is no ungotten character
and calling `read` reads zero characters but doesn't error).

### Basic IO interface

#### `stdin`, `stdout`, and `stderr`

**Provided by**: `<ministd_io.h>`
**Provided by**: `<ministd_fmt.h>`

Interfaces to the standard input, output, and error streams.

All of them are `LockedFile`s,
in order to allow concurrent access.

Additionally, `stdin` and `stdout` are buffered,
with `stdin` being "tied" to `stdout`:
before a `read` is performed on `stdin`,
`stdout` is first flushed.

#### Higher-level output – `fputs`, `fputc`, `puts`, `putc`

**Provided by**: `<ministd_io.h>`

`fputs` takes a null-terminated string as an argument,
which it will then write to the file in a loop
until the whole string has been written,
returning the length of the string.
This allows the user to not worry about partial writes
or determining the length of a buffer themselves.

`fputc` tries to write a single character to the file,
returning the written character.

Both `fputs` and `fputc` errors with `ERR_IO`
when a call to `write` results in zero bytes written.

`puts` takes a null-terminated string but no file,
using `fputs` to write the string to `stdout`,
before writing a newline to `stdout`.

`putc` is a thin wrapper over `fputs(c, stdout, err_out)`.

#### Higher-level input – `fgets`, `fgetc`, `fgetline`, `gets`, `getc`, `getline`

**Provided by**: `<ministd_io.h>`

`gets`, `getc`, and `getline` are thin wrappers
over `fgets`, `fgetc`, and `fgetline` respectively
passing `stdin` as the file argument.

`fgets` tries to read a space-delimited word from the input file,
where a "space" is defined as
a byte that is less than or equal to the space character.
If a word is read such that the next character is a space or EOF,
then `true` is returned, otherwise `false`.

`fgetc` tries to read a single character from the file,
returning it upon success (cast to an `int`) or `-1` on failure.

`fgetline` tries to read a line into the provided buffer,
returning `true` if it read up to a newline
and `false` if it read up to EOF.

### Basic file types

#### Creating a basic file – `open`, `from_fd`

**Provided by**: `<ministd_io.h>`

`open` opens a file descriptor-based `FILE` from a given path.
Currently all files are created with both the read and write bits set,
and there is no support for customising the mode the file is opened with.

`from_fd` takes a file descriptor and creates a `FILE own`.

#### File descriptors – `fd_open`, `fd_read`, `fd_write`, `fd_close`

**Provided by**: `<ministd_io.h>`

`fd_open` takes in a path, opens that file, and returns the file descriptor.
Currently all files are created with both the read and write bits set,
and there is no support for customising the mode the file is opened with.

`fd_read`, `fd_write`, and `fd_close` function as `read`, `write`, and `close` respectively,
but taking file descriptors rather than a `FILE` pointers.

#### Buffered files – `BufferedFile`, `bf_new`, `bf_new_from`

**Provided by**: `<ministd_io.h>`

A `BufferedFile` is has a read and write buffer
so that it can read and write large pieces of data at once,
reducing the number of syscalls.

`bf_new` wraps a given file into a `BufferedFile`
using default buffer sizes.

`bf_new_from` wraps a given file into a `BufferedFile`
taking a read and write buffer as arguments.

#### Locked Files – `LockedFile`, `lf_new`

**Provided by**: `<ministd_io.h>`

A `LockedFile` is a wrapper over another file
which just protects access to the underlying file behind a mutex lock,
enabling concurrent access without data races.

`lf_new` wraps the given file in a `LockedFile`.

### Formatted IO interface

#### Formatted output – `fprint*`, `print*`

**Provided by**: `<ministd_fmt.h>`

The following print functions are defined,
taking an argument of the type indicated:

<table>
<thead>
  <tr> <th>function(s)</th> <th>type</th> </tr>
</thead>
<tbody>
  <tr> <td><code>fprinth</code> <code>fprinthx</code> <code>fprinthb</code> <code>printh</code> <code>printhx</code> <code>printhb</code></td> <td><code>short</code></td> </tr>
  <tr> <td><code>fprintuh</code> <code>fprintuhx</code> <code>fprintuhb</code> <code>printuh</code> <code>printuhx</code> <code>printuhb</code></td> <td><code>unsigned short</code></td> </tr>
  <tr> <td><code>fprinti</code> <code>fprintix</code> <code>fprintib</code> <code>printi</code> <code>printix</code> <code>printib</code></td> <td><code>int</code></td> </tr>
  <tr> <td><code>fprintui</code> <code>fprintuix</code> <code>fprintuib</code> <code>printui</code> <code>printuix</code> <code>printuib</code></td> <td><code>unsigned int</code></td> </tr>
  <tr> <td><code>fprintl</code> <code>fprintlx</code> <code>fprintlb</code> <code>printl</code> <code>printlx</code> <code>printlb</code></td> <td><code>long</code></td> </tr>
  <tr> <td><code>fprintul</code> <code>fprintulx</code> <code>fprintulb</code> <code>printul</code> <code>printulx</code> <code>printulb</code></td> <td><code>unsigned long</code></td> </tr>
  <tr> <td><code>fprintz</code> <code>fprintzx</code> <code>fprintzb</code> <code>printz</code> <code>printzx</code> <code>printzb</code></td> <td><code>isz</code></td> </tr>
  <tr> <td><code>fprintuz</code> <code>fprintuzx</code> <code>fprintuzb</code> <code>printuz</code> <code>printuzx</code> <code>printuzb</code></td> <td><code>usz</code></td> </tr>
  <tr> <td><code>fprints</code> <code>prints</code></td> <td><code>const char ref</code></td> </tr>
  <tr> <td><code>fprintc</code> <code>printc</code></td> <td><code>char</code></td> </tr>
  <tr> <td><code>fprintp</code> <code>printp</code></td> <td><code>const ptr</code></td> </tr>
</tbody>
</table>

All `print*` functions are just wrappers over `fprint*(..., stdout, err_out)`.

`fprint*` and `print*` returns the number of bytes written to the file.

Integer types get three variants of the print function:
 - `fprint_` prints the integer in base ten
 - `fprint_x` prints the integer in base sixteen (hexadecimal)
 - `fprint_b` prints the integer in base two (binary)

`fprints` just prints the provided string to the file,
and is just a wrapper around `fputs`.

`fprintc` just prints the provided character to the file,
and is just a wrapper around `fputc`.

`fprintp` prints the address of the provided pointer as a hexadecimal value.

#### Formatted input – `fscan*`, `scan*`

**Provided by**: `<ministd_fmt.h>`

The following scan functions are defined,
returning an argument of the type indicated:

<table>
<thead>
  <tr> <th>function(s)</th> <th>type</th> </tr>
</thead>
  <tr> <td><code>fscanh</code> <code>scanh</code></td> <td><code>short</code></td> </tr>
  <tr> <td><code>fscanuh</code> <code>scanuh</code></td> <td><code>unsigned short</code></td> </tr>
  <tr> <td><code>fscani</code> <code>scani</code></td> <td><code>int</code></td> </tr>
  <tr> <td><code>fscanui</code> <code>scanui</code></td> <td><code>unsigned int</code></td> </tr>
  <tr> <td><code>fscanl</code> <code>scanl</code></td> <td><code>long</code></td> </tr>
  <tr> <td><code>fscanul</code> <code>scanul</code></td> <td><code>unsigned long</code></td> </tr>
  <tr> <td><code>fscanz</code> <code>scanz</code></td> <td><code>isz</code></td> </tr>
  <tr> <td><code>fscanuz</code> <code>scanuz</code></td> <td><code>usz</code></td> </tr>
  <tr> <td><code>fscans</code> <code>scans</code></td> <td><code>char own</code></td> </tr>
  <tr> <td><code>fscanc</code> <code>scanc</code></td> <td><code>char</code></td> </tr>
</table>

All `scan*` functions are just wrappers over `fscan*(stdin, err_out)`.

`fscanc` is just a wrapper over `fgetc`
and gets a single character from the file.

`fscans` is a wrapper of `fgets` with an automatically growing buffer,
which will continue reading from the file until a whole word has been read.

The other `fscan*` functions just reads an integer value from the input file.

### Formatted terminal output

Formatted terminal output in `<ministd_term.h>` is implemented
using ANSI escape codes.

#### Cursor movement – `term_csi_cursor_*`, `term_csi_goto_col`, `term_csi_goto`

**Provided by**: `<ministd_term.h>`

There are eight provided functions for moving the cursor.

`term_csi_cursor_up`, `term_csi_cursor_down`, `term_csi_cursor_forward`, and `term_csi_cursor_back`
moves the cursor n cells up, down, forwards, or backwards.

`term_csi_cursor_nextline` and `term_csi_cursor_prevline`
moves the cursor to the beginning of the next or previous line.

`term_csi_goto_col` moves the cursor to the given column.

`term_csi_goto` moves the cursor to column x, row y.

#### Clearing – `term_csi_clearscreen*`, `term_csi_clear_line*`

**Provided by**: `<ministd_term.h>`

`term_csi_clearscreen_to_end` clears the screen
from where the cursor is to the end of the screen.

`term_csi_clearscreen_to_start` clears the screen
from where the cursor is to the start of the screen.

`term_csi_clearscreen` clears the entire screen.

`term_csi_clearscreen_full` clears the entire screen
and deletes the scrollback buffer.

`term_csi_clear_line_to_end` clears the current line,
from the position of the cursor to the end of the line.

`term_csi_clear_line_to_start` clears the current line,
from the position of the cursor to the start of the line.

`term_csi_clear_line` clears the whole current line.

#### Scrolling – `term_csi_scroll*`

**Provided by**: `<ministd_term.h>`

`term_csi_scroll_up` scrolls the terminal buffer up by n lines.

`term_csi_scroll_down` scrolls the terminal buffer down by n lines.

#### Graphics – `enum TERM_GRAPHICS` and `term_set_graphics`

**Provided by**: `<ministd_term.h>`

The `TERM_GRAPHICS` enumeration contains the numbers
for the "Select Graphic Rendition" ANSI code.

It contains values for resetting graphics,
changing decoration (bold, italic, blinking, etc),
changing fonts, and setting background and foreground colours.

They are then used with the `term_set_graphics` function
to change the graphics mode of the terminal output.

TODO create an example of coloured & decorated text

### Polling

#### Creating pollfds – `enum poll_flag`, `struct pollfd` and `pollfd_new`, `pollfd_new_from_file`

**Provided by**: `<ministd_poll.h>`

The `pollfd` struct is used for monitoring files for events
(for example, data being available to be read)
using the `poll` function.

The `poll_flag` enumeration lists the different events
which can be listened for,
passed to functions like `pollfd_new` or `pollfd_has_events`.
You can get documentation for the different flags
by viewing the `poll(3p)` manpage on Linux.

In order to create a `pollfd` from a file descriptor,
you must use the `pollfd_new` function,
also passing all the events you want to listen for,
bitwise or'd together.

To create a `pollfd` from a `FILE` object,
you must use the `pollfd_new_from_file` function,
once again passing the desired event flags or'd together.

#### Using pollfds – `pollfd_has_events`, `poll`

**Provided by**: `<ministd_poll.h>`

You can check if a `pollfd` has pending events of a certain type
by using the `pollfd_has_events` function
along with the events you wish to test all or'd together.
If you want to test for *any* event,
pass `~0` as the events argument.

`poll` is a wrapper around Linux's `poll` syscall,
and takes an array of `pollfd`s
along with the length of said array and a timeout.
The kernel will then set and reset the pending events
for each provided `pollfd` as appropriate.
If there are no new events at the time it is called
then `poll`'s behaviour changes depending on the timeout provided:
 - when the timeout is `0`, `poll` returns immediately
 - when the timeout is `-1`, `poll` will block until a requested event occurs
   or until the program receives an interrupt
 - otherwise, `poll` will timeout in the number of milliseconds specified

`poll` returns the number of files with events.

#### Poll list type – `poll_list_t` and `poll_list_new`, `poll_list_free`, `poll_list_poll`

**Provided by**: `<ministd_poll.h>`

`poll_list_t` is a type for managing lists of `pollfd`s.

A poll list can be created with `poll_list_new`,
and resources are cleaned up with `poll_list_free` once you're done with it.

#### Modifying poll lists – `poll_list_add*`, `poll_list_remove`

**Provided by**: `<ministd_poll.h>`

There are three methods for adding a new `pollfd` to a poll list:
 - `poll_list_add` is used to add an existing `pollfd` struct
 - `poll_list_add_fd` is used to add a `pollfd` created from a file descriptor
 - `poll_list_add_file` is used to add a `pollfd` created from a `FILE`

The function `poll_list_remove` is used
to remove the `pollfd` at the given index.

#### Using poll lists – `poll_list_first`, `poll_list_next`

**Provided by**: `<ministd_poll.h>`

Use `poll_list_poll` to run `poll` on the underlying array.

After calling `poll_list_poll`,
you can then iterate over the files with waiting events
using `poll_list_first` and `poll_list_next`.

`poll_list_first` returns the first `pollfd` with an event,
or `NULL` if there is no such `pollfd`.

`poll_list_next` returns the next `pollfd` with an event,
or `NULL` if it was passed the last such `pollfd`.

#### Example

TODO expand this example

Also, this example demonstrates that this API isn't properly developed yet...

```
#include <ministd_fmt.h>
#include <ministd_poll.h>

int
main(void)
{
	poll_list_t poll_list = poll_list_new(NULL);

	poll_list_add_file(&poll_list, stdin, PF_IN, NULL);
	/* add more files ... */

	for (;;) {
		struct pollfd ref curr;

		poll_list_poll(&poll_list, -1, NULL);

		for (curr = poll_list_first(&poll_list); curr != NULL;
		     curr = poll_list_next(&poll_list, curr, NULL)) {
			int fd = curr->fd;
			prints("Got some input!\n", NULL);
			/* get input or something */
		}
	}

	return 0;
}
```

## Concurrency

### Atomics

#### Memory ordering – `enum memory_order` and `memory_fence`

**Provided by**: `<ministd_atomic.h>`

The `memory_fence` function prevents the cpu
from reordering memory read/writes across the function call.

The `memory order` enumeration defines how strict
the memory ordering guarantees of an atomic operation should be,
where `MO_STRICT` prevents all memory reorderings across the operation,
while `MO_RELAXED` allows some reordering.

For example (as I understand it),
memory writes could be reordered with relaxed memory ordering:

```c
some_value = dosomething();
atomic_store_i(updated, 1, MO_RELAXED);
/* the assignment to some_value could be moved to here,
 * meaning an observer will see the old version of some_value,
 * even though updated is set!
 */
```

#### Atomic types – `struct Atomic*` and `atomic_new_*`

**Provided by**: `<ministd_atomic.h>`

The `Atomic*` structs are the core of the atomics support,
where each atomic struct is actually a thin wrapper around an integer value.

A new atomic is created with the associated `atomic_new_*` function,
wherein you supply the initial value.

<table>
<thead>
  <tr> <th>atomic type</th> <th>integer type</th> <th>function suffix</th> </tr>
</thead>
<tbody
  <tr> <td><code>AtomicC</code></td> <td><code>signed char</code></td> <td><code>_c</code></td> </tr>
  <tr> <td><code>AtomicUC</code></td> <td><code>unsigned char</code></td> <td><code>_uc</code></td> </tr>
  <tr> <td><code>AtomicH</code></td> <td><code>short</code></td> <td><code>_h</code></td> </tr>
  <tr> <td><code>AtomicUH</code></td> <td><code>unsigned short</code></td> <td><code>_uh</code></td> </tr>
  <tr> <td><code>AtomicI</code></td> <td><code>int</code></td> <td><code>_i</code></td> </tr>
  <tr> <td><code>AtomicUI</code></td> <td><code>unsigned int</code></td> <td><code>_ui</code></td> </tr>
  <tr> <td><code>AtomicL</code></td> <td><code>long</code></td> <td><code>_l</code></td> </tr>
  <tr> <td><code>AtomicUL</code></td> <td><code>unsigned long</code></td> <td><code>_ul</code></td> </tr>
  <tr> <td><code>AtomicZ</code></td> <td><code>isz</code></td> <td><code>_z</code></td> </tr>
  <tr> <td><code>AtomicUZ</code></td> <td><code>usz</code></td> <td><code>_uz</code></td> </tr>
</tbody>
</table>

#### Loading and storing – `atomic_load_*`, `atomic_store_*`, `atomic_swap_*`

**Provided by**: `<ministd_atomic.h>`

Basic loading and storing of atomic values is done with `atomic_load_*`, `atomic_store_*`, and `atomic_swap_*`.

`atomic_load_*` takes an atomic and returns its value.

`atomic_store_*` takes an atomic and sets it to a new value.

`atomic_swap_*` takes an atomic,
sets it to a new value,
and returns its previous value.

#### Basic operations – `atomic_fetch_add_*`, `atomic_fetch_sub_*`, `atomic_fetch_or_*`, `atomic_fetch_xor_*`, `atomic_fetch_and_*`, `atomic_fetch_min_*`, `atomic_fetch_max_*`

**Provided by**: `<ministd_atomic.h>`

`atomic_fetch_*` applies the given operation to the atomic's value
and returns the old value.

<table>
<thead>
  <tr> <th><code>atomic_fetch_XXX_*</code></th> <th>roughly equivalent non-atomic code</th> </tr>
</thead>
<tbody>
  <tr> <th>add</th> <th><code>old_val = atomic, atomic += val, old_val</code></th> </tr>
  <tr> <th>sub</th> <th><code>old_val = atomic, atomic -= val, old_val</code></th> </tr>
  <tr> <th>or</th> <th><code>old_val = atomic, atomic |= val, old_val</code></th> </tr>
  <tr> <th>xor</th> <th><code>old_val = atomic, atomic ^= val, old_val</code></th> </tr>
  <tr> <th>and</th> <th><code>old_val = atomic, atomic &= val, old_val</code></th> </tr>
  <tr> <th>min</th> <th><code>old_val = atomic, atomic = (atomic < val ? atomic : val), old_val</code></th> </tr>
  <tr> <th>max</th> <th><code>old_val = atomic, atomic = (atomic > val ? atomic : val), old_val</code></th> </tr>
</tbody>
</table>

#### Compare-exchange – `atomic_compare_exchange_*`

**Provided by**: `<ministd_atomic.h>`

`atomic_compare_exchange_*` is a very powerful operation,
one which some of the previous ones are implemented with.

Besides the atomic and memory ordering,
it takes a pointer to the expected value of the atomic
and the new value the atomic should be set to.

If the atomic matches the expected value,
then it is set to the new value and `true` is returned.

If the atomic does *not* match the old value
then the expected value is updated to the atomic's current value
and `false` is returned.

As an example, this is roughly how `atomic_fetch_max_*` is implemented
(here given for `AtomicI`):

```c
int
atomic_fetch_max_i(volatile struct AtomicI this, int val, enum memory_order order)
{
	int old, new;

	/* get the current value */
	old = atomic_load_i(this, order);

	do {
		/* calculate the new value */
		new = old > val ? old : val;
	/* and set the atomic, unless it's been changed in the meanwhile,
	 * if so then try again
	 */
	} while (!atomic_compare_exchange_i(this, &old, new, order));

	return new;
}
```

### Spawning subprocesses

#### `clone`

**Provided by**: `<ministd_sched.h>`

#### `_syscall_clone3`

**Provided by**: `<ministd_sched.h>`

### Locks

#### Mutex – `mutex_t` and `mutex_new`, `mutex_try_lock`, `mutex_lock`, `mutex_unlock`

**Provided by**: `<ministd_threads.h>`

A mutex is a simple atomic flag,
which can be either acquired or released.

Currently, it is implemented with `mutex_t` reffering to `AtomicI`,
but that could change without warning.

`mutex_new` is used to create a new mutex,
which is in the released state.

`mutex_try_lock` uses an atomic swap operation
to set the mutex's state to acquired.
It then checks the old state,
and if it was already in an acquired state (ie. someone else is using it)
then it returns `false`,
otherwise (ie. we just acquired the lock) it returns `true`.

`mutex_lock` is simply a while loop
that keeps trying to acquire the lock using `mutex_try_lock`.
In other words, it blocks the thread until the lock has been acquired,
implementing a simple spin lock.

`mutex_unlock` stores the released state to the mutex,
allowing other threads to acquire it.

## Misc

### Timing and waiting

#### Low-level sleeping – `struct timespec` and `nanosleep`

**Provided by**: `<ministd_time.h>`

#### High-level sleeping – `millisleep`, `sleep`

**Provided by**: `<ministd_time.h>`

### `__stack_chk_fail`

**Provided by**: not exposed through headers, defined in `ministd.c`

`ministd.c` also provides a `__stack_chk_fail` function,
which is run whenever the program detects stack smashing
(don't ask me how that is done, that's entirely the compiler's job).

It just uses a syscall to write a message to standard error,
before exiting with the exit code 127.
