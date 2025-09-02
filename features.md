# Features

Here I will document the features of ministdlib,
along with their test coverage.

I will also document some of the process/decisions behind their implementation.

## Core

### Startup and exit

A custom `_start` function is supplied to set up the environment
and run the user's `main` function,
which should have the signature `int main(void)`.

An `atexit` function is supplied
for scheduling cleanup functions to run before program exit.

Two program termination functions are provided:
`exit` and `thread_exit`.

### Interacting with `argv` and `envp`

Command line arguments can be accessed through the `argv()` function,
with the number of command line arguments supplied by `argc()`.

Environment variables can be accessed through `envp()`,
with the number of environment variables supplied by `envc()`.

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

...
