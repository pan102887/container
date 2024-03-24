#ifndef _RB_TREE_H_
#define _RB_TREE_H_

struct rb_node {
    unsigned long __rb_parent_color__;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));

struct rb_root {
    struct rb_node *rb_node;
};

#define rb_parent(r)   ((struct rb_node *)((r)->__rb_parent_color__ & ~3))

#define rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root) ((root)->rb_node == NULL)

#define RB_EMPTY_NODE(node) \
    ((node) -> __rb_parent_color__ == (unsigned long)(node))
#define RB_CLEAR_NODE(node) \
    ((node) -> __rb_parent_color__ = (unsigned long)(node))

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_prev(const struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);

/* Postorder iteration - always visit the parent after its children */
extern struct rb_node *rb_first_postorder(const struct rb_root *);
extern struct rb_node *rb_next_postorder(const struct rb_node *);

#endif