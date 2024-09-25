#ifndef _CONTAINER_OF_H_
#define _CONTAINER_OF_H_

// #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - ((size_t) &((type *)0)->member) );})


#endif