#ifndef _CONTAINER_OF_H_
#define _CONTAINER_OF_H_

#define container_of(ptr, type, member) \
    ((type *)((void *)(ptr) - (void *)(&((type *)0)->member)))

#endif