#ifndef _RM_STD_THREADS_H
#define _RM_STD_THREADS_H

#include <_ministd_prelude.h>

typedef struct AtomicI mutex_t;

mutex_t own mutex_new(err_t ref err_out);
bool mutex_try_lock(mutex_t ref this);
void mutex_lock(mutex_t ref this);
void mutex_unlock(mutex_t ref this);

#endif /* _RM_STD_THREADS_H */
