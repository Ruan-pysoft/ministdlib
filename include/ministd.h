#ifndef _RM_STD_H
#define _RM_STD_H

#include <_ministd_prelude.h>

/* This is the main header for providing basic startup and exit functionality,
 * as well as some basic interaction with the program's environment.
 */

/* These functions are used to access
 * the program's command-line arguments (argc/argv)
 * and the program's environment variables (envc/envp)
 */
int argc(void);
char ref ref argv(void);
int envc(void);
char ref ref envp(void);

/* Functions to terminate the program */
void atexit(void (ref func)(void));
void exit(int exitcode);
void thread_exit(int exitcode);
void panic(isz exitcode);
#define PANIC(code) do { panic(code); __builtin_unreachable(); } while (0)

/* The program's entry point, called by the kernel
 * _start in turn call's the user's main.
 * See https://en.wikipedia.org/wiki/Entry_point#Contemporary */
void _start(void);

#endif /* _RM_STD_H */
