#ifndef __READ_WRITE_ONCE_H__
#define __READ_WRITE_ONCE_H__

#include "compiler.h"

/*
 * Use __READ_ONCE() instead of READ_ONCE() if you do not require any
 * atomicity. Note that this may result in tears!
 */
#ifndef __READ_ONCE
#define __READ_ONCE(x)	(*(const volatile)&(x))
#endif

#define READ_ONCE(X) ({__READ_ONCE(X);})

#endif