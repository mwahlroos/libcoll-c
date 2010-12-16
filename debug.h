/* debug.h */

#include <stdio.h>

#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG_ENABLED   1

#define DEBUGF(fmt, ...)        if (DEBUG_ENABLED) printf(fmt, __VA_ARGS__)
#define DEBUG(msg)              DEBUGF("%s", msg)

#endif  /* DEBUG_H */
