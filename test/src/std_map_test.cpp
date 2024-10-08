#include <stdio.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <gtest/gtest.h>
#include <random>
#include <chrono>
#include <unordered_map>
#include "map_test_data_struct.h"



namespace test::std_map {
    using namespace test::map;
    std::map<std::string, test_node*> std_map;
    inline std::vector<test_node*> test_data_list = {};
    inline std::vector<test_node*> inserted = {};
    std::unordered_map<std::string, std::vector<test_node*>> duplicated;

    // inline const char* random_str(int length) {
    //     char* buffer = (char*)malloc(sizeof(char) * length + 1);

    //     std::random_device rd;
    //     std::default_random_engine random(rd());

    //     char tmp;
    //     for (int i = 0; i < length; i++) {
    //         auto value = random() % 36;
    //         tmp = value;
    //         if (tmp < 10) {
    //             tmp += '0';
    //         }
    //         else {
    //             tmp -= 10;
    //             tmp += 'A';
    //         }
    //         buffer[i] = tmp;
    //     }
    //     buffer[length] = '\0';
    //     return buffer;
    // }

    static inline void data_gen() {
        
        for (size_t i = 0; i < test_node::node_count; i++) {
            const char* key = random_str(test_node::str_len);
            const char* value = random_str(test_node::str_len);
            test_node* node = new test_node(key, value);
            test_data_list.push_back(node);
        }
    }

    static inline void std_map_insert_test() {
        std::unordered_map<std::string, std::vector<test_node*>>::iterator iter;
        for (std::vector<test_node*>::size_type i = 0; i < test_data_list.size(); i++) {
            test_node* node = test_data_list[i];
            if (std_map.find(*node->key) == std_map.end()) {
            // if (true) {
                std_map[*node->key] = node;
                inserted.push_back(node);
            }
            else if ((iter = duplicated.find(*node->key)) != duplicated.end()) {
                iter->second.push_back(node);
            }
            else {
                struct test_node* first_one = std_map.find(*node->key)->second;
                duplicated[*node->key] = { first_one, node };
            }
        }
    }
    static inline void std_map_searching_test() {
        for (auto& insert_node : inserted) {
            auto iter = std_map.find(*insert_node->key);
            struct test_node* search_result = iter == std_map.end() ? nullptr : iter->second;
            ASSERT_NE(nullptr, search_result) << "CAN NOT FIND " << *insert_node->key;
            ASSERT_EQ(search_result->value, insert_node->value) << "the search result does't match expect";
        }
        for (auto node : duplicated) {
            std::cout << "values (";
            for (auto value : node.second) {
                std::cout << *(value->value);
                if (value != node.second.back()) {
                    std::cout << ", ";
                }
                else {
                    std::cout << ") ";
                }
            }
            std::cout << "with duplicated key: " << node.first << std::endl;
        }
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

    TEST(STD_MAP, _TEST) {
        auto time_count = std::chrono::duration_cast<std::chrono::milliseconds>(run_and_count_time(data_gen));
        std::cout << "data_gen spend: " << time_count.count() << " milliseconds" << std::endl;

        time_count = std::chrono::duration_cast<std::chrono::milliseconds>(run_and_count_time(std_map_insert_test));
        std::cout << "std_map_insert_test spend: " << time_count.count() << " milliseconds" << std::endl;

        time_count = std::chrono::duration_cast<std::chrono::milliseconds>(run_and_count_time(std_map_searching_test));
        std::cout << "std_map_searching_test spend: " << time_count.count() << " milliseconds" << std::endl;

        time_count = std::chrono::duration_cast<std::chrono::milliseconds>(run_and_count_time(destroy));
        std::cout << "destroy spend: " << time_count.count() << " milliseconds" << std::endl;
    }
}

