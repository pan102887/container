#include <stdio.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <gtest/gtest.h>
#include <chrono>
#include <unordered_map>
#include "map_test_data_struct.h"

namespace test::rb_tree {
    using namespace test::map;
    extern "C" {
        static inline struct test_node* test_node_search(struct rb_root* root, std::string key) {
            struct rb_node* insert_node = root->rb_node;

            while (insert_node) {
                struct test_node* data = rb_entry(insert_node, struct test_node, rb);
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

        static inline bool test_node_insert(struct rb_root* root, struct test_node* insert_node) {
            struct rb_node** rb_link = &(root->rb_node), * parent = NULL;

            while (*rb_link) {
                struct test_node* that = container_of(*rb_link, struct test_node, rb);
                int result = test_node::compare(insert_node, that);

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

    inline std::vector<test_node*> test_data_list = {};
    inline std::vector<test_node*> inserted = {};
    std::unordered_map<std::string, std::vector<test_node*>> duplicated;



    inline void data_gen() {
        for (size_t i = 0; i < test_node::node_count; i++) {
            const char* key = random_str(test_node::str_len);
            const char* value = random_str(test_node::str_len);
            test_node* node = new test_node(key, value);
            test_data_list.push_back(node);
        }
    }

    inline void rb_tree_insert_test() {
        std::unordered_map<std::string, std::vector<test_node*>>::iterator iter;
        for (std::vector<test_node*>::size_type i = 0; i < test_data_list.size(); i++) {
            test_node* node = test_data_list[i];
            if (test_node_insert(&root, node)) {
                inserted.push_back(node);
            }
            else if ((iter = duplicated.find(*node->key)) != duplicated.end()) {
                iter->second.push_back(node);
            }
            else {
                test_node* first_one = test_node_search(&root, *node->key);
                duplicated[*node->key] = { first_one, node };
            }
        }
    }
    inline void rb_tree_searching_test() {
        for (auto& insert_node : inserted) {
            test_node* search_result = test_node_search(&root, *insert_node->key);
            ASSERT_NE(nullptr, search_result) << "CAN NOT FIND " << *insert_node->key;
            ASSERT_EQ(search_result->value, insert_node->value) << "the search result does't match expect";
        }
        // for (auto node : duplicated) {
        //     std::cout << "values (";
        //     for (auto value : node.second) {
        //         std::cout << *(value->value);
        //         if (value != node.second.back()) {
        //             std::cout << ", ";
        //         }
        //         else {
        //             std::cout << ") ";
        //         }
        //     }
        //     std::cout << "with duplicated key: " << node.first << std::endl;
        // }
    }

    inline std::chrono::nanoseconds run_and_count_time(void run(void)) {
        auto start = std::chrono::system_clock::now();
        run();
        auto end = std::chrono::system_clock::now();
        return end - start;
    }


    inline void destroy() {
        inserted.clear(),
            duplicated.clear();
        for (auto i : test_data_list) {
            delete(i);
        }
        test_data_list.clear();
    }

    TEST(RB_TREE, _TEST) {
        auto time_count = std::chrono::duration_cast<std::chrono::milliseconds>(run_and_count_time(data_gen));
        std::cout << "data_gen spend: " << time_count.count() << " milliseconds" << std::endl;

        time_count = std::chrono::duration_cast<std::chrono::milliseconds>(run_and_count_time(rb_tree_insert_test));
        std::cout << "rb_tree_insert_test spend: " << time_count.count() << " milliseconds" << std::endl;

        time_count = std::chrono::duration_cast<std::chrono::milliseconds>(run_and_count_time(rb_tree_searching_test));
        std::cout << "rb_tree_searching_test spend: " << time_count.count() << " milliseconds" << std::endl;

        time_count = std::chrono::duration_cast<std::chrono::milliseconds>(run_and_count_time(destroy));
        std::cout << "destroy spend: " << time_count.count() << " milliseconds" << std::endl;
    }
}