#ifndef __TREE_H__
#define __TREE_H__


#define TreeRoot(ElementType) struct TreeRoot_ ## ElementType { struct TreeNode_ ## ElementType *root }

#define TreeNode(ElementType)               \
struct TreeNode_ ## ElementType {           \
    ElementType element;                    \
    struct TreeNode_ ## ElementType *firstChild;     \
    struct TreeNode_ ## ElementType *nextSibling;    \
}



#endif