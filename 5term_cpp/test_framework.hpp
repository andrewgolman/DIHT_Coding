//------------------------------------------
//! Framework for running unit tests
//! Available options:
//!     TEST(name) { test_body } - add test
//!     RUN_ALL_TESTS()
//!
//! All tests should have unique names! Names will be used soon beautifully.
//------------------------------------------
#pragma once

#include <iostream>
#include <cassert>
#include <memory>
#include "utils.hpp"

struct TestCase {
    virtual void run() const = 0;
};

std::vector<std::shared_ptr<TestCase>> tests;
void add_test(const std::shared_ptr<TestCase>& ptr) {
    tests.push_back(ptr);
}

#define TEST(name) \
void run_test_##name(const std::string&); \
struct Test_##name : public TestCase { \
    void run() const override { \
        run_test_##name(#name); \
    } \
}; \
struct AddTest_##name { \
    AddTest_##name() { \
        add_test(std::make_shared<Test_##name>()); \
    } \
}; \
AddTest_##name addtest_##name; \
void run_test_##name(const std::string& TEST_NAME)


#define RUN_ALL_TESTS() \
int main() { \
    for (auto ptr : tests) { \
        ptr->run(); \
    } \
    std::cout << "Passed " << tests.size() << " tests!" << std::endl; \
    return 0;\
}

template <typename Type>
void ASSERT_EQUAL_VALUES(const Type& a, const Type& b) {
    if (compare_to_zero(b - a) != 0) {
        std::cout << "Assertion failed!" << std::endl;
        std::cout << "Found: " << a << std::endl;
        std::cout << "Expected: " << b << std::endl;
    }
}
#define ASSERT_EQUAL_VALUES(a, b) \
ASSERT_EQUAL_VALUES(a, b);


template <typename Type>
void assert_equal_vectors(const std::vector<Type>& a, const std::vector<Type>& b, const std::string& test_name) {
    if (a.size() != b.size()) {
        std::cout << "Test " << test_name << " failed" << std::endl;
        std::cout << "Vector size found: " << a.size() << std::endl;
        std::cout << "Vector size expected: " << b.size() << std::endl;
        throw std::runtime_error("Assertion failed");
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (compare_to_zero(a[i] - b[i]) != 0) {
            std::cout << "Test " << test_name << " failed" << std::endl;
            std::cout << "Vector elem at position " << i << " found: " << a[i] << std::endl;
            std::cout << "Vector elem at position " << i << " expected: " << b[i] << std::endl;
            throw std::runtime_error("Assertion failed");
        }
    }
}
#define ASSERT_EQUAL_VECTORS(a, b) \
assert_equal_vectors(a, b, TEST_NAME);
