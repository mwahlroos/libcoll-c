/* debug.h */

#include <stdio.h>

#ifndef LIBCOLL_DEBUG_H
#define LIBCOLL_DEBUG_H

/* disable debug output by default;
 * can be enabled by defining ENABLE_DEBUG=1 on the compiler command line.
 */
#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG    0
#endif

#define DEBUGF(fmt, ...)        if (ENABLE_DEBUG) printf(fmt, __VA_ARGS__)
#define DEBUG(msg)              DEBUGF("%s", msg)

#endif  /* LIBCOLL_DEBUG_H */
