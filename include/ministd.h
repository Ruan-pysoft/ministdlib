#ifndef _RM_STD_H
#define _RM_STD_H

#include "ministd_io.h"
#include "ministd_types.h"

int argc(void);
char ref ref argv(void);
int envc(void);
char ref ref envp(void);

void atexit(void (ref func)(void));
void exit(int exitcode);
void _start(void);

#endif /* _RM_STD_H */
