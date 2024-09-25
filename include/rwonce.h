#ifndef __READ_WRITE_ONCE_H__
#define __READ_WRITE_ONCE_H__
#ifdef __cplusplus
extern "C" {
#endif

// TODO: READ_ONCE and WRIGHT_ONCE wait for implement
/*
 * Use __READ_ONCE() instead of READ_ONCE() if you do not require any
 * atomicity. Note that this may result in tears!
 */
#ifndef __READ_ONCE
#define __READ_ONCE(x)	(x)
#endif

#define READ_ONCE(x) ({__READ_ONCE(x);})


#ifndef __WRITE_ONCE
#define __WRITE_ONCE(x, v) (x = (v))
#endif

#define WRITE_ONCE(x, v) ({__WRITE_ONCE(x, v);})
#ifdef __cplusplus
}
#endif

#endif