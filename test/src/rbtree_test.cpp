#include <stdio.h>
#include <string>
#include <iostream>
#include <stdbool.h>
#include <stdlib.h>
#include <vector>
#include <gtest/gtest.h>
#include <random>
#include <chrono>
extern "C" {
#include "rwonce.h"
#include "rb_tree.h"
}

extern "C" {
class test_node {
public:
    struct rb_node rb;
    const std::string *key;
    const std::string *value;
    test_node(const char *key_str, const char* value_str) {
        this->key = new std::string(key_str);
        this->value = new std::string(value_str);
    }
    test_node(const std::string *key_str, const std::string *value_str) {
        this->key = key_str;
        this->value = value_str;
    }
    ~test_node() {
        delete(key);
        delete(value);
    }
};



static int compare(struct test_node* a,
    struct test_node* b)
{
    return (*(a->key)).compare(*(b->key));
}


static inline struct test_node *test_node_search(struct rb_root *root, std::string key) {
    struct rb_node *insert_node = root->rb_node;

    while (insert_node) {
        struct test_node *data = rb_entry(insert_node, struct test_node, rb);
        int compare_result = key.compare(*data->key);
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

} // extern "C"


static struct rb_root root;
inline std::vector<test_node*> test_data_list = {
    // new test_node("KEY", "Hello rb tree!!"),
    // new test_node("key_1", "that is a test for rb tree!!"),
    // new test_node("key_2", "that a value 2!!"),
    // new test_node("key_3", "insert_node 3!!")
};

const char* random_str(int length) {    
    char *buffer = (char *)malloc(sizeof(char) * length + 1);

    std::random_device rd;
    std::default_random_engine random(rd());

    char tmp;
    for (int i = 0; i < length; i++) {
        auto value = random() % 36;
        tmp = value;
        if (tmp <= 10) {
            tmp += '0';
        } else {
            tmp -= 10;
            tmp += 'A';
        }
        buffer[i] = tmp;
    }
    buffer[length] = '\0';
    return buffer;
}

static inline void rb_tree_test(void) {
    const static int str_len = 8;
    const static int node_count = 100000;
    for (size_t i = 0; i < node_count; i++) {
        const char *key = random_str(str_len);
        const char * value = random_str(str_len);
        class test_node *node = new test_node(key, value);
        test_data_list.push_back(node);
    }

    for (std::vector<test_node*>::size_type i = 0; i < test_data_list.size(); i++) {
        class test_node* node = test_data_list[i];
        if (!test_node_insert(&root, node)) {
            test_data_list.erase(test_data_list.begin() + i);
        }
    }
}

TEST(rb_tree, insert_and_search_test) {
    rb_tree_test();
    for (auto &insert_node : test_data_list) {
        struct test_node *search_result = test_node_search(&root, *insert_node->key);
        ASSERT_NE(nullptr, search_result) << "CAN NOT FIND " << *insert_node->key;
        ASSERT_EQ(search_result->value, insert_node->value) << "the search result does't match expect";
        // std::cout << "expect value:\t" << *insert_node->value << std::endl
        //         << "searched value:\t" << *search_result->value <<std::endl;
    }
    for (auto insert_node : test_data_list) {
        delete(insert_node);
    }
}
