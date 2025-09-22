#ifndef _RM_STD_PRELUDE_H
#define _RM_STD_PRELUDE_H

/* this header is included by every other <ministd*.h> header,
 * and the purpose of it is to provide a basic interface
 * which is available everywhere in the library,
 * and everywhere the library is used.
 *
 * furthermore, as far as possible headers should avoid
 * including any headers other than this one,
 * to prevent functionality "leaking"
 * (ie. getting <ministd_io.h> functions when including <ministd_fmt.h>
 */

#include <ministd_types.h>

#include <ministd.h>
#include <ministd_error.h>

typedef struct FILE FILE;

#endif /* _RM_STD_PRELUDE_H */
