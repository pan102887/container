#include <iostream>

#include "gtest/gtest.h"
#include <string>


TEST(test_1, test_b) {
    std::string s("2");
    const char *s_1 = "2";
    ASSERT_NE(typeid("2"), typeid(s_1)) << "they are different";
}