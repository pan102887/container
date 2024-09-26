#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vector>
#include <gtest/gtest.h>
extern "C" {
#include "rwonce.h"
#include "rb_tree.h"
}

extern "C" {
struct test_node {
    struct rb_node rb;
    const char* key;
    size_t key_size;
    const char* value;
    size_t value_size;
};


static int compare(struct test_node* a,
    struct test_node* b)
{
    size_t compare_len =
        a->key_size < b->key_size ?
        a->key_size : b->key_size;
    return memcmp(a->key, b->key, compare_len);
}


static inline struct test_node *test_node_search(struct rb_root *root, const char *key) {
    struct rb_node *node = root->rb_node;

    while (node) {
        struct test_node *data = rb_entry(node, struct test_node, rb);
        int compare_result = strcmp(key, data->key);
        if (compare_result < 0) {
            node = node->rb_left;
        } 
        else if (compare_result > 0) {
            node = node->rb_right;
        }
        else {
            return data;
        }
    }
    return NULL;
}

static inline bool test_node_insert(struct rb_root *root, struct test_node *node) {
    struct rb_node **new_one = &(root->rb_node), *parent = NULL;

    while (*new_one) {
        struct test_node *that = container_of(*new_one, struct test_node, rb);
        int result = compare(node, that);

        parent = *new_one;
        if (result < 0) {
            new_one = &((*new_one)->rb_left);
        }
        else if (result > 0) {
            new_one = &((*new_one)->rb_right);
        } 
        else {
            return false;
        }
    }
    rb_link_node(&node->rb, parent, new_one);
    rb_insert_color(&node->rb, root);
    return true;
}

static inline void init_test_node(const char *key, const char *value, struct test_node *p) {
    if (!p) {
        return;
    }
    p->rb.__rb_parent_color = 0;
    p->rb.rb_left = NULL;
    p->rb.rb_right = NULL;
    p->key = key,
    p->key_size = strlen(key);
    p->value = value;
    p->value_size = strlen(value);    
}

}

static inline test_node *create_node(const char* key, const char* value) {
    struct test_node *node_0 = new test_node();
    init_test_node(key, value, node_0);
    return node_0;
}


static struct rb_root root;
static inline std::vector<test_node*> test_data_list = {
    create_node("KEY", "Hello rb tree!!"),
    create_node("key_1", "that is a test for rb tree!!"),
    create_node("key_2", "that a value 2!!"),
    create_node("3", "node 3!!")
};


static inline void rb_tree_test(void) {
    for (auto &node : test_data_list) {
        test_node_insert(&root, node);
    }
}

TEST(rb_tree, insert_and_search_test) {
    rb_tree_test();
    for (auto &node : test_data_list) {
        int i = 0;
        struct test_node *search_result = test_node_search(&root, node->key);
        ASSERT_EQ(search_result->value, node->value) << "the search result does't match expect";
        printf("expect value:\t%s.\n", node->value);
        printf("searched value:\t%s.\n", search_result->value);
    }
    for (auto node : test_data_list) {
        free(node);
    }
}
