/* debug.h */

#include <stdio.h>

#ifndef DEBUG_H
#define DEBUG_H

#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG    0
#endif

#define DEBUGF(fmt, ...)        if (ENABLE_DEBUG) printf(fmt, __VA_ARGS__)
#define DEBUG(msg)              DEBUGF("%s", msg)

#endif  /* DEBUG_H */
