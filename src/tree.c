#include <stdlib.h>
#include <pthread.h>
#include "tree.h"


TreeNode(int) tree;

void test(void) {
    void * pointer = malloc(sizeof(int) * 4);
    void * pointerB = malloc(sizeof(int) * 4);
    free(pointer);
    free(pointerB);
    
}