#ifndef _RB_TREE_C_H_
#define _RB_TREE_C_H_

// ========================= rbtree_typed.h =========================
struct rb_node {
    unsigned long __rb_parent_color;
    struct rb_node* rb_right;
    struct rb_node* rb_left;
} __attribute__((aligned(sizeof(long))));

struct rb_root {
    struct rb_node* rb_node;
};

/*
 * Leftmost-cached rbtrees.
 *
 * We do not cache the rightmost node based on footprint
 * size vs number of potential users that could benefit
 * from O(1) rb_last(). Just not worth it, users that want
 * this feature can always implement the logic explicitly.
 * Furthermore, users that want to cache both pointers may
 * find it a bit asymmetric, but that's ok.
 */
struct rb_root_cached {
    struct rb_root rb_root;
    struct rb_node* rb_leftmost;
};


#define RB_ROOT \
    (struct rb_root) { NULL, }
#define RB_ROOT_CACHED                  \
    (struct rb_root_cached) { {         \
                                  NULL, \
                              },        \
                              NULL }


// ========================= rbtree.h =========================
#include <linux/stddef.h>
#include "container_of.h"

#define rb_parent(r)   ((struct rb_node *)((r)->__rb_parent_color & ~3))

#define rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root) (READ_ONCE((root)->rb_node) == NULL)

/* 'empty' nodes are nodes that are known not to be inserted in an rbtree */
#define RB_EMPTY_NODE(node) \
    ((node)->__rb_parent_color == (unsigned long)(node))
#define RB_CLEAR_NODE(node) \
    ((node)->__rb_parent_color = (unsigned long)(node))

extern void rb_insert_color(struct rb_node*, struct rb_root*);
extern void rb_erase(struct rb_node*, struct rb_root*);


/* Find logical next and previous nodes in a tree */
extern struct rb_node* rb_next(const struct rb_node*);
extern struct rb_node* rb_prev(const struct rb_node*);
extern struct rb_node* rb_first(const struct rb_root*);
extern struct rb_node* rb_last(const struct rb_root*);

/* Postorder iteration - always visit the parent after its children */
extern struct rb_node* rb_first_postorder(const struct rb_root*);
extern struct rb_node* rb_next_postorder(const struct rb_node*);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(struct rb_node* victim, struct rb_node* new_one,
    struct rb_root* root);

#if  0
extern void rb_replace_node_rcu(struct rb_node* victim, struct rb_node* new_one,
    struct rb_root* root);
#endif


static inline void rb_link_node(struct rb_node* node, struct rb_node* parent, struct rb_node** rb_link) {
    node->__rb_parent_color = (unsigned long)parent;
    node->rb_left = node->rb_right = NULL;

    // parent's link to this node
    *rb_link = node;
}

#if 0
// TODO: READ COPY UPDATE: waiting to figure out the details
static inline void rb_link_node_rcu(struct rb_node *node, struct rb_node *parent,
				    struct rb_node **rb_link)
{
	node->__rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;

	rcu_assign_pointer(*rb_link, node);
}

#define rb_entry_safe(ptr, type, member) ({ typeof(ptr) ____ptr = (ptr); ____ptr ? rb_entry(____ptr, type, member) : NULL; })
#endif

#define rb_entry_safe(ptr, type, member) rb_entry(ptr, type, member)

#if 0
/**
 * rbtree_postorder_for_each_entry_safe - iterate in post-order over rb_root of
 * given type allowing the backing memory of @pos to be invalidated
 *
 * @pos:	the 'type *' to use as a loop cursor.
 * @n:		another 'type *' to use as temporary storage
 * @root:	'rb_root *' of the rbtree.
 * @field:	the name of the rb_node field within 'type'.
 *
 * rbtree_postorder_for_each_entry_safe() provides a similar guarantee as
 * list_for_each_entry_safe() and allows the iteration to continue independent
 * of changes to @pos by the body of the loop.
 *
 * Note, however, that it cannot handle other modifications that re-order the
 * rbtree it is iterating over. This includes calling rb_erase() on @pos, as
 * rb_erase() may rebalance the tree, causing us to miss some nodes.
 */


#define rbtree_postorder_for_each_entry_safe(pos, n, root, field)                   \
	for (pos = rb_entry_safe(rb_first_postorder(root), typeof(*pos), field);        \
	     pos && ({ n = rb_entry_safe(rb_next_postorder(&pos->field),                \
			typeof(*pos), field); 1; });                                            \
	     pos = n)
#endif

/* Same as rb_first(), but O(1) */
#define rb_first_cached(root) (root)->rb_leftmost

static inline void rb_insert_color_cached(struct rb_node* node,
    struct rb_root_cached* root,
    bool leftmost)
{
    if (leftmost)
        root->rb_leftmost = node;
    rb_insert_color(node, &root->rb_root);
}


static inline struct rb_node*
rb_erase_cached(struct rb_node* node, struct rb_root_cached* root)
{
    struct rb_node* leftmost = NULL;

    if (root->rb_leftmost == node)
        leftmost = root->rb_leftmost = rb_next(node);

    rb_erase(node, &root->rb_root);

    return leftmost;
}

static inline void rb_replace_node_cached(struct rb_node* victim,
    struct rb_node* new_one,
    struct rb_root_cached* root)
{
    if (root->rb_leftmost == victim)
        root->rb_leftmost = new_one;
    rb_replace_node(victim, new_one, &root->rb_root);
}

/**
 * rb_add() - insert @node into @tree
 * @node: node to insert
 * @tree: tree to insert @node into
 * @less: operator defining the (partial) node order
 */
static __always_inline struct rb_node*
rb_add_cached(struct rb_node* node, struct rb_root_cached* tree,
    bool (*less)(struct rb_node*, const struct rb_node*))
{
    struct rb_node** link = &tree->rb_root.rb_node;
    struct rb_node* parent = NULL;
    bool  leftmost = true;
    while (*link) {
        parent = *link;
        if (less(node, parent)) {
            link = &parent->rb_left;
        }
        else {
            link = &parent->rb_right;
            leftmost = false;
        }
    }

    rb_link_node(node, parent, link);
    rb_insert_color_cached(node, tree, leftmost);

    return leftmost ? node : NULL;
}

/**
 * rb_find_add() - find equivalent @node in @tree, or add @node
 * @node: node to look-for / insert
 * @tree: tree to search / modify
 * @cmp: operator defining the node order
 *
 * Returns the rb_node matching @node, or NULL when no match is found and @node
 * is inserted.
 */
static __always_inline struct rb_node*
rb_find_add(struct rb_node* node, struct rb_root* tree,
    int (*cmp)(struct rb_node*, const struct rb_node*))
{
    struct rb_node** link = &tree->rb_node;
    struct rb_node* parent = NULL;
    int c;

    while (*link) {
        parent = *link;
        c = cmp(node, parent);

        if (c < 0)
            link = &parent->rb_left;
        else if (c > 0)
            link = &parent->rb_right;
        else
            return parent;
    }

    rb_link_node(node, parent, link);
    rb_insert_color(node, tree);
    return NULL;
}

/**
 * rb_find() - find @key in tree @tree
 * @key: key to match
 * @tree: tree to search
 * @cmp: operator defining the node order
 *
 * Returns the rb_node matching @key or NULL.
 */
static __always_inline struct rb_node*
rb_find(const void* key, const struct rb_root* tree,
    int (*cmp)(const void* key, const struct rb_node*))
{
    struct rb_node* node = tree->rb_node;

    while (node) {
        int c = cmp(key, node);

        if (c < 0)
            node = node->rb_left;
        else if (c > 0)
            node = node->rb_right;
        else
            return node;
    }

    return NULL;
}

/**
 * rb_find_first() - find the first @key in @tree
 * @key: key to match
 * @tree: tree to search
 * @cmp: operator defining node order
 *
 * Returns the leftmost node matching @key, or NULL.
 */
static __always_inline struct rb_node*
rb_find_first(const void* key, const struct rb_root* tree,
    int (*cmp)(const void* key, const struct rb_node*))
{
    struct rb_node* node = tree->rb_node;
    struct rb_node* match = NULL;

    while (node) {
        int c = cmp(key, node);

        if (c <= 0) {
            if (!c)
                match = node;
            node = node->rb_left;
        }
        else if (c > 0) {
            node = node->rb_right;
        }
    }

    return match;
}

/**
 * rb_next_match() - find the next @key in @tree
 * @key: key to match
 * @tree: tree to search
 * @cmp: operator defining node order
 *
 * Returns the next node matching @key, or NULL.
 */
static __always_inline struct rb_node*
rb_next_match(const void* key, struct rb_node* node,
    int (*cmp)(const void* key, const struct rb_node*))
{
    node = rb_next(node);
    if (node && cmp(key, node))
        node = NULL;
    return node;
}

/**
 * rb_for_each() - iterates a subtree matching @key
 * @node: iterator
 * @key: key to match
 * @tree: tree to search
 * @cmp: operator defining node order
 */
#define rb_for_each(node, key, tree, cmp) \
	for ((node) = rb_find_first((key), (tree), (cmp)); \
	     (node); (node) = rb_next_match((key), (node), (cmp)))

// ========================= rbtree_augmented.h ========================= 
#include "compiler.h"

 /*
  * Please note - only struct rb_augment_callbacks and the prototypes for
  * rb_insert_augmented() and rb_erase_augmented() are intended to be public.
  * The rest are implementation details you are not expected to depend on.
  *
  * See Documentation/core-api/rbtree.rst for documentation and samples.
  */

struct rb_augment_callbacks {
    void (*propagate)(struct rb_node* node, struct rb_node* stop);
    void (*copy)(struct rb_node* old, struct rb_node* new_one);
    void (*rotate)(struct rb_node* old, struct rb_node* new_one);
};

extern void __rb_insert_augmented(struct rb_node* node, struct rb_root* root,
    void (*augment_rotate)(struct rb_node* old, struct rb_node* new_one));

/*
 * Fixup the rbtree and update the augmented information when rebalancing.
 *
 * On insertion, the user must update the augmented information on the path
 * leading to the inserted node, then call rb_link_node() as usual and
 * rb_insert_augmented() instead of the usual rb_insert_color() call.
 * If rb_insert_augmented() rebalances the rbtree, it will callback into
 * a user provided function to update the augmented information on the
 * affected subtrees.
 */
static inline void
rb_insert_augmented(struct rb_node* node, struct rb_root* root,
    const struct rb_augment_callbacks* augment)
{
    __rb_insert_augmented(node, root, augment->rotate);
}


static inline void
rb_insert_augmented_cached(struct rb_node* node,
    struct rb_root_cached* root, bool new_oneleft,
    const struct rb_augment_callbacks* augment)
{
    if (new_oneleft)
        root->rb_leftmost = node;
    rb_insert_augmented(node, &root->rb_root, augment);
}

static __always_inline struct rb_node*
rb_add_augmented_cached(struct rb_node* node, struct rb_root_cached* tree,
    bool (*less)(struct rb_node*, const struct rb_node*),
    const struct rb_augment_callbacks* augment)
{
    struct rb_node** link = &tree->rb_root.rb_node;
    struct rb_node* parent = NULL;
    bool leftmost = true;

    while (*link) {
        parent = *link;
        if (less(node, parent)) {
            link = &parent->rb_left;
        }
        else {
            link = &parent->rb_right;
            leftmost = false;
        }
    }

    rb_link_node(node, parent, link);
    augment->propagate(parent, NULL);
    rb_insert_augmented_cached(node, tree, leftmost, augment);
    return leftmost ? node : NULL;
}

/*
 * Template for declaring augmented rbtree callbacks (generic case)
 *
 * RBSTATIC:    'static' or empty
 * RBNAME:      name of the rb_augment_callbacks structure
 * RBSTRUCT:    struct type of the tree nodes
 * RBFIELD:     name of struct rb_node field within RBSTRUCT
 * RBAUGMENTED: name of field within RBSTRUCT holding data for subtree
 * RBCOMPUTE:   name of function that recomputes the RBAUGMENTED data
 */

#define RB_DECLARE_CALLBACKS(RBSTATIC, RBNAME,				        \
			     RBSTRUCT, RBFIELD, RBAUGMENTED, RBCOMPUTE)	        \
static inline void							                        \
RBNAME ## _propagate(struct rb_node *rb, struct rb_node *stop)		\
{									                                \
	while (rb != stop) {						                    \
		RBSTRUCT *node = rb_entry(rb, RBSTRUCT, RBFIELD);	        \
		if (RBCOMPUTE(node, true))				                    \
			break;						                            \
		rb = rb_parent(&node->RBFIELD);				                \
	}								                                \
}									                                \
static inline void							                        \
RBNAME ## _copy(struct rb_node *rb_old, struct rb_node *rb_new_one)		\
{	                                								\
	RBSTRUCT *old = rb_entry(rb_old, RBSTRUCT, RBFIELD);		    \
	RBSTRUCT *new_one = rb_entry(rb_new_one, RBSTRUCT, RBFIELD);		    \
	new_one->RBAUGMENTED = old->RBAUGMENTED;				            \
}									                                \
static void							                            	\
RBNAME ## _rotate(struct rb_node *rb_old, struct rb_node *rb_new_one)	\
{									                                \
	RBSTRUCT *old = rb_entry(rb_old, RBSTRUCT, RBFIELD);		    \
	RBSTRUCT *new_one = rb_entry(rb_new_one, RBSTRUCT, RBFIELD);		    \
	new_one->RBAUGMENTED = old->RBAUGMENTED;				            \
	RBCOMPUTE(old, false);						                    \
}									                                \
RBSTATIC const struct rb_augment_callbacks RBNAME = {			    \
	.propagate = RBNAME ## _propagate,				                \
	.copy = RBNAME ## _copy,					                    \
	.rotate = RBNAME ## _rotate					                    \
};

 /*
  * Template for declaring augmented rbtree callbacks,
  * computing RBAUGMENTED scalar as max(RBCOMPUTE(node)) for all subtree nodes.
  *
  * RBSTATIC:    'static' or empty
  * RBNAME:      name of the rb_augment_callbacks structure
  * RBSTRUCT:    struct type of the tree nodes
  * RBFIELD:     name of struct rb_node field within RBSTRUCT
  * RBTYPE:      type of the RBAUGMENTED field
  * RBAUGMENTED: name of RBTYPE field within RBSTRUCT holding data for subtree
  * RBCOMPUTE:   name of function that returns the per-node RBTYPE scalar
  */

#define RB_DECLARE_CALLBACKS_MAX(RBSTATIC, RBNAME, RBSTRUCT, RBFIELD,	      \
				 RBTYPE, RBAUGMENTED, RBCOMPUTE)	     	                  \
static inline bool RBNAME ## _compute_max(RBSTRUCT *node, bool exit)	      \
{                                   									      \
	RBSTRUCT *child;				                            		      \
	RBTYPE max = RBCOMPUTE(node);					                          \
	if (node->RBFIELD.rb_left) {					                          \
		child = rb_entry(node->RBFIELD.rb_left, RBSTRUCT, RBFIELD);           \
		if (child->RBAUGMENTED > max)				                          \
			max = child->RBAUGMENTED;			                              \
	}								                                          \
	if (node->RBFIELD.rb_right) {					                          \
		child = rb_entry(node->RBFIELD.rb_right, RBSTRUCT, RBFIELD);          \
		if (child->RBAUGMENTED > max)				                          \
			max = child->RBAUGMENTED;			                              \
	}								                                          \
	if (exit && node->RBAUGMENTED == max)				                      \
		return true;						                                  \
	node->RBAUGMENTED = max;					                              \
	return false;							                                  \
}									                                          \
RB_DECLARE_CALLBACKS(RBSTATIC, RBNAME,					                      \
		     RBSTRUCT, RBFIELD, RBAUGMENTED, RBNAME ## _compute_max)

#define RB_RED 0
#define RB_BLACK 1


#define __rb_parent(pc) ((struct rb_node *)((pc) & ~3))
#define __rb_color(pc) ((pc) & 1)
#define __rb_is_black(pc) __rb_color(pc)
#define __rb_is_red(pc) (!__rb_color(pc))
#define rb_color(rb) __rb_color((rb)->__rb_parent_color)
#define rb_is_red(rb) __rb_is_red((rb)->__rb_parent_color)
#define rb_is_black(rb) __rb_is_black((rb)->__rb_parent_color)

static inline void rb_set_parent(struct rb_node* rb, struct rb_node* parent) {
    rb->__rb_parent_color = rb_color(rb) + (unsigned long)parent;
}

static inline void rb_set_parent_color(struct rb_node* rb, struct rb_node* p, int color) {
    rb->__rb_parent_color = (unsigned long)p + color;
}

static inline void
__rb_change_child(struct rb_node* old, struct rb_node* new_one,
    struct rb_node* parent, struct rb_root* root)
{
    if (parent) {
        if (parent->rb_left == old)
            WRITE_ONCE(parent->rb_left, new_one);
        else
            WRITE_ONCE(parent->rb_right, new_one);
    }
    else
        WRITE_ONCE(root->rb_node, new_one);
}

#if 0
static inline void
__rb_change_child_rcu(struct rb_node *old, struct rb_node *new_one,
		      struct rb_node *parent, struct rb_root *root)
{
	if (parent) {
		if (parent->rb_left == old)
			rcu_assign_pointer(parent->rb_left, new_one);
		else
			rcu_assign_pointer(parent->rb_right, new_one);
	} else
		rcu_assign_pointer(root->rb_node, new_one);
}
#endif

extern void __rb_erase_color(struct rb_node* parent, struct rb_root* root,
    void (*augment_rotate)(struct rb_node* old, struct rb_node* new_one));


static __always_inline struct rb_node*
__rb_erase_augmented(struct rb_node* node, struct rb_root* root,
    const struct rb_augment_callbacks* augment)
{
    struct rb_node* child = node->rb_right;
    struct rb_node* tmp = node->rb_left;
    struct rb_node* parent, * rebalance;
    unsigned long pc;

    if (!tmp) {
        /*
         * Case 1: node to erase has no more than 1 child (easy!)
         *
         * Note that if there is one child it must be red due to 5)
         * and node must be black due to 4). We adjust colors locally
         * so as to bypass __rb_erase_color() later on.
         */
        pc = node->__rb_parent_color;
        parent = __rb_parent(pc);
        __rb_change_child(node, child, parent, root);
        if (child) {
            child->__rb_parent_color = pc;
            rebalance = NULL;
        }
        else
            rebalance = __rb_is_black(pc) ? parent : NULL;
        tmp = parent;
    }
    else if (!child) {
        /* Still case 1, but this time the child is node->rb_left */
        tmp->__rb_parent_color = pc = node->__rb_parent_color;
        parent = __rb_parent(pc);
        __rb_change_child(node, tmp, parent, root);
        rebalance = NULL;
        tmp = parent;
    }
    else {
        struct rb_node* successor = child, * child2;

        tmp = child->rb_left;
        if (!tmp) {
            /*
             * Case 2: node's successor is its right child
             *
             *    (n)          (s)
             *    / \          / \
             *  (x) (s)  ->  (x) (c)
             *        \
             *        (c)
             */
            parent = successor;
            child2 = successor->rb_right;

            augment->copy(node, successor);
        }
        else {
            /*
             * Case 3: node's successor is leftmost under
             * node's right child subtree
             *
             *    (n)          (s)
             *    / \          / \
             *  (x) (y)  ->  (x) (y)
             *      /            /
             *    (p)          (p)
             *    /            /
             *  (s)          (c)
             *    \
             *    (c)
             */
            do {
                parent = successor;
                successor = tmp;
                tmp = tmp->rb_left;
            } while (tmp);
            child2 = successor->rb_right;
            WRITE_ONCE(parent->rb_left, child2);
            WRITE_ONCE(successor->rb_right, child);
            rb_set_parent(child, successor);

            augment->copy(node, successor);
            augment->propagate(parent, successor);
        }

        tmp = node->rb_left;
        WRITE_ONCE(successor->rb_left, tmp);
        rb_set_parent(tmp, successor);

        pc = node->__rb_parent_color;
        tmp = __rb_parent(pc);
        __rb_change_child(node, successor, tmp, root);

        if (child2) {
            rb_set_parent_color(child2, parent, RB_BLACK);
            rebalance = NULL;
        }
        else {
            rebalance = rb_is_black(successor) ? parent : NULL;
        }
        successor->__rb_parent_color = pc;
        tmp = successor;
    }

    augment->propagate(tmp, NULL);
    return rebalance;
}

static __always_inline void
rb_erase_augmented(struct rb_node *node, struct rb_root *root,
		   const struct rb_augment_callbacks *augment)
{
	struct rb_node *rebalance = __rb_erase_augmented(node, root, augment);
	if (rebalance)
		__rb_erase_color(rebalance, root, augment->rotate);
}

static __always_inline void
rb_erase_augmented_cached(struct rb_node *node, struct rb_root_cached *root,
			  const struct rb_augment_callbacks *augment)
{
	if (root->rb_leftmost == node)
		root->rb_leftmost = rb_next(node);
	rb_erase_augmented(node, &root->rb_root, augment);
}
#endif /* _RB_TREE_H_ */