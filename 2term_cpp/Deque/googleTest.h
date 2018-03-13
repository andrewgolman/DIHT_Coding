#ifndef INC_1113_DEQUE_GOOGLETEST_H
#define INC_1113_DEQUE_GOOGLETEST_H

#include "gtest/gtest.h"
#include "/Deque.h"
#include "/DequeIterator.h"
#include <string>
#include <ctime>

TEST(stack, allocation) {
    myStack<std::string> stack;
    for (unsigned long long i = 0; i < 100000; ++i)
        stack.push("aaaa");
    EXPECT_NO_THROW();
};


TEST(Deque, copy_constructor) {
    Deque<int> sample;
    for (size_t i = 0; i < 1000; ++i) {
        sample.push_back(i);
        sample.push_front(i);
    }
    const Deque<int> testing(sample);
    for (size_t i = 0; i < 2000; ++i)
        EXPECT_EQ(sample[i], testing[i]);
}

TEST(Deque_stdCompare, all_methods) {
    srand(time(0));
    Deque<int> deque;
    std::deque<int> stddeque;
    for (int i = 0; i < 10000; ++i) {
        int j = rand() % 8, k = rand() % 10000;
        if (deque.size() > 1 && j == 0) {
            deque.pop_back();
            stddeque.pop_back();
            EXPECT_EQ(deque.back(), stddeque.back());
        }
        if (deque.size() > 1 && j == 1) {
            deque.pop_front();
            stddeque.pop_front();
            EXPECT_EQ(deque.front(), stddeque.front());
        }
        if (deque.size() < 10 && j >= 2 && j < 5) {
            deque.push_back(k);
            stddeque.push_back(k);
        }
        if (deque.size() < 10 && j >= 5) {
            deque.push_front(k);
            stddeque.push_front(k);
        }
        EXPECT_EQ(*deque.begin(), *stddeque.begin());
        EXPECT_EQ(*deque.cend(), *stddeque.cend());
        EXPECT_EQ(*deque.crbegin(), *stddeque.crbegin());
        if (deque.size()) {
            int rnd = rand();
            EXPECT_EQ(deque[rnd % deque.size()], stddeque[rnd % stddeque.size()]);
        }
    }
}

TEST(Deque, push_and_pop) {
    Deque<int> deque;
    for (int i = 0; i < 3000; ++i) {
        deque.push_front(i);
    }
    for (int i = 0; i < 2000; ++i) {
        EXPECT_EQ(deque.size(), 3000 - i);
        EXPECT_EQ(deque.front(), 2999);
        EXPECT_EQ(deque.back(), i);
        EXPECT_EQ(deque.empty(), false);
        EXPECT_EQ(deque.pop_back(), i);
    }
    for (int i = 0; i < 1000; ++i)
        EXPECT_EQ(2999 - i, deque[i]);
    for (int i = 0; i < 1000; ++i)
        EXPECT_EQ(deque.pop_back(), 2000 + i);
    EXPECT_EQ(deque.empty(), true);
}

TEST(DequeIterator, creating_iterators) {
    Deque<int> deque;
    for (size_t i = 0; i < 1000; ++i) {
        deque.push_front(999-i);
        deque.push_back(1000+i);
    }
    int k = 0;
    for (DequeIterator<int, int*, int&> iter = deque.begin(); iter != deque.end(); ++iter, ++k)
        EXPECT_EQ(*iter, k);
}

TEST(DequeIterator, increment) {
    Deque<int> deque;
    for (int i = 0; i <= 3000; ++i)
        deque.push_front(i);
    deque.pop_back();
    Deque<int>::iterator iter = deque.begin();
    for (int i = 0; iter != deque.end(); ++iter, ++i) {
        EXPECT_EQ(*iter, 3000 - i);
    }
}

TEST(iterator, begin) {
    Deque<int> deque;
    for (size_t i = 0; i < 1000; ++i) {
        deque.push_front(999-i);
        deque.push_back(1000+i);
    }
    int k = 0;
    for (DequeIterator<int, int*, int&> iter = deque.begin(); iter != deque.end(); ++iter, ++k)
        EXPECT_EQ(*iter, k);
    k = 0;
    for (Deque<int>::reverse_iterator iter = deque.rbegin(); iter != deque.rend(); ++iter, ++k)
        EXPECT_EQ(*iter, 1999-k);
}

#endif //INC_1113_DEQUE_GOOGLETEST_H

