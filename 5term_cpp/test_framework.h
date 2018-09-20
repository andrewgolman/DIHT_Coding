//------------------------------------------
//! Framework for running unit tests
//! Available options:
//!     TEST(name) { test_body } - add test
//!     RUN_ALL_TESTS()
//!
//! All tests should have unique names! Names will be used beautifully soon.
//------------------------------------------
#pragma once

#include <cassert>
#include <memory>


struct TestCase {
    virtual void run() const = 0;
};

std::vector<std::shared_ptr<TestCase>> tests;
void add_test(const std::shared_ptr<TestCase>& ptr) {
    tests.push_back(ptr);
}

#define TEST(name) \
void run_test_##name(); \
struct Test_##name : public TestCase { \
    void run() const override { \
        run_test_##name(); \
    } \
}; \
struct AddTest_##name { \
    AddTest_##name() { \
        add_test(std::make_shared<Test_##name>()); \
    } \
}; \
AddTest_##name addtest_##name; \
void run_test_##name()


#define RUN_ALL_TESTS() \
int main() { \
    for (auto ptr : tests) { \
        ptr->run(); \
    } \
    std::cout << "Passed " << tests.size() << " tests!" << std::endl; \
    return 0;\
}
