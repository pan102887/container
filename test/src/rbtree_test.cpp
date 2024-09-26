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
    struct rb_node *insert_node = root->rb_node;

    while (insert_node) {
        struct test_node *data = rb_entry(insert_node, struct test_node, rb);
        int compare_result = strcmp(key, data->key);
        if (compare_result < 0) {
            insert_node = insert_node->rb_left;
        } 
        else if (compare_result > 0) {
            insert_node = insert_node->rb_right;
        }
        else {
            return data;
        }
    }
    return NULL;
}

static inline bool test_node_insert(struct rb_root *root, struct test_node *insert_node) {
    struct rb_node **rb_link = &(root->rb_node), *parent = NULL;

    while (*rb_link) {
        struct test_node *that = container_of(*rb_link, struct test_node, rb);
        int result = compare(insert_node, that);

        parent = *rb_link;
        if (result < 0) {
            rb_link = &((*rb_link)->rb_left);
        }
        else if (result > 0) {
            rb_link = &((*rb_link)->rb_right);
        } 
        else {
            return false;
        }
    }
    rb_link_node(&insert_node->rb, parent, rb_link);
    rb_insert_color(&insert_node->rb, root);
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
    create_node("3", "insert_node 3!!")
};


static inline void rb_tree_test(void) {
    for (auto &insert_node : test_data_list) {
        test_node_insert(&root, insert_node);
    }
}

TEST(rb_tree, insert_and_search_test) {
    rb_tree_test();
    for (auto &insert_node : test_data_list) {
        int i = 0;
        struct test_node *search_result = test_node_search(&root, insert_node->key);
        ASSERT_EQ(search_result->value, insert_node->value) << "the search result does't match expect";
        printf("expect value:\t%s.\n", insert_node->value);
        printf("searched value:\t%s.\n", search_result->value);
    }
    for (auto insert_node : test_data_list) {
        free(insert_node);
    }
}
