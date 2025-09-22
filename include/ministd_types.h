#ifndef _RM_STD_TYPES_H
#define _RM_STD_TYPES_H

/* ownership-indicating pointer aliases */
#define own *
#define ref *

typedef unsigned long usz;
typedef long isz;
typedef void ref ptr;
typedef void own own_ptr;
typedef int bool;

#define NULL ((void*)0)
#define true 1
#define false 0

#endif /* _RM_STD_TYPES_H */
