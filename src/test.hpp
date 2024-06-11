#include<stdlib.h>

class A {
public:

    static inline void test_fun(void);
    template<typename R>
    inline static R test_fun_b(R* pointer);

private:
    template<typename R>
    struct TestImpl;
};



template<typename R>
inline R A::test_fun_b(R* pointer) {
    return TestImpl<R>()(pointer);
}

template<typename R>
struct A::TestImpl<R*>
{
    R operator()(R* pointer) const {
        return *((R*)(pointer));
    }
};

template<typename T>
class Complex {
#define product_with_conjugate(complex) ((complex.real * complex.real) + (complex.image * complex.image))
#define divisor = ((other.real * other.real) + (other.image * other.image));
public:
    T real;
    T image;
    Complex(T real, T image) : real(real), image(image) {}

    Complex operator+(const Complex& other) const {
        Complex<T> result(real + other.real, image + other.image);
        return result;
    }

    Complex operator-(const Complex& other) const {
        Complex<T> result(real - other.real, image - other.image);
        return result;
    }

    Complex operator*(const Complex& other) const {
        Complex<T> result((real * other.real) - (image * other.image), (real * other.image) + (image * other.real));
        return result;
    }


    Complex operator/(const Complex& other) const {
        Complex<T> result(
            ((real * other.real) + (image * other.image)) / product_with_conjugate(other),
            ((image * other.real) - (real * other.image)) / product_with_conjugate(other)
        );
        return result;
    }

    Complex operator()(T real, T image) const {
        Complex<T> result(
            this->real + real,
            this->image + image
        );
        return result;
    }

    bool operator==(const Complex& other) const {
        return this->real == other.real && this->image == other.image;
    }
#undef product_with_conjugate
#undef divisor
    

};