#ifndef __MAP_TEST_DATA_STRUCT_H__
#define __MAP_TEST_DATA_STRUCT_H__

#include <string>
#include <random>
extern "C" {
#include "rwonce.h"
#include "rb_tree.h"
}

namespace test::map
{
    class test_node {
    public:
        static const int str_len = 10;
        static const int node_count = 1000000;
        struct rb_node rb;
        const std::string* key;
        const std::string* value;
        static int compare(struct test_node* a,
            struct test_node* b)
        {
            return (*(a->key)).compare(*(b->key));
        }
        inline test_node(const char* key_str, const char* value_str) {
            this->key = new std::string(key_str);
            this->value = new std::string(value_str);
        }
        inline test_node(const std::string* key_str, const std::string* value_str) {
            this->key = key_str;
            this->value = value_str;
        }
        inline ~test_node() {
            delete(key);
            delete(value);
        }
    };
} // namespace test::map

extern const char* random_str(int length);


#endif