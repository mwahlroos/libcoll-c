/* debug.h */

#include <stdio.h>

#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG_ENABLED   1

#define DEBUG(fmt, ...)      if (DEBUG_ENABLED) printf(fmt, __VA_ARGS__)

#endif  /* DEBUG_H */
