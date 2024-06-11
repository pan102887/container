#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "test.hpp"

inline void A::test_fun(void)
{
    std::cout << "test for\n";
}

int main(int argc, char *argv[]) {
    int *p = new int[10]{ 0 };
    p[0] = 17;
    p[3] = 10;
    p[4] = 8;

    A a;
    A::test_fun_b(p);
    
    Complex<int> a(3, 5);
    Complex<int> b(3, -5);
    {
        Complex<int> result = a + b;
        printf("(%d, %di)\n", result.real, result.image);
    }

    {
        Complex<int> result = a - b;
        printf("(%d, %di)\n", result.real, result.image);
    }
    
    {
        Complex<int> result = a * b;
        printf("(%d, %di)\n", result.real, result.image);
    }
    
    {
        Complex<int> result = a / b;
        printf("(%d, %di)\n", result.real, result.image);
    }

    {
        Complex<int> result = a(1, 2);
        printf("(%d, %di)\n", result.real, result.image);
    }
    
    printf("a = b --> %s\n", a == b ? "true" : "false");
    printf("a = a --> %s\n", a == a ? "true" : "false");
    

    
    
    return 0;

    
    
}