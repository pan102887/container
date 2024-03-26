#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "rwonce.h"
#include "rb_tree.h"

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


static inline struct test_node *test_node_search(struct rb_root *root, char *key) {
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
    struct rb_node **new = &(root->rb_node), *parent = NULL;

    while (*new) {
        struct test_node *this = container_of(*new, struct test_node, rb);
        int result = compare(node, this);

        parent = *new;
        if (result < 0) {
            new = &((*new)->rb_left);
        }
        else if (result > 0) {
            new = &((*new)->rb_right);
        } 
        else {
            return false;
        }
    }
    rb_link_node(&node->rb, parent, new);
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

static inline void test_node_search_print(const char *key, struct test_node *result) {
    printf("search for key: %s, \n and result's value is: %s\n", key, result ? result->value : "NULL");
}

static inline void rb_tree_test(void) {
    struct rb_root root = {NULL};
    struct test_node node_0;
    init_test_node("KEY", "Hello rb tree!!", &node_0);

    struct test_node node_1;
    init_test_node("key_1", "this is a test for rb tree!!", &node_1);

    struct test_node node_2;
    init_test_node("key_2", "this a value 2!!", &node_2);

    struct test_node node_3;
    init_test_node("3", "node 3!!", &node_3);

    test_node_insert(&root, &node_0);
    test_node_insert(&root, &node_1);
    test_node_insert(&root, &node_2);
    test_node_insert(&root, &node_3);


    char *search_key = "KEY";
    struct test_node *search_result = test_node_search(&root, search_key);
    test_node_search_print(search_key, search_result);

    search_key = "key_1";
    search_result = test_node_search(&root, search_key);
    test_node_search_print(search_key, search_result);

    search_key = "key_2";
    search_result = test_node_search(&root, search_key);
    test_node_search_print(search_key, search_result);

    search_key = "3";
    search_result = test_node_search(&root, search_key);
    test_node_search_print(search_key, search_result);

    search_key = "key_3";
    search_result = test_node_search(&root, search_key);
    test_node_search_print(search_key, search_result);

    

}

int main(int argc, char* argv[]) {
#ifdef __TEST__
    
#else
    rb_tree_test();
    printf("hello world\n");
#endif
    return 0;
}


