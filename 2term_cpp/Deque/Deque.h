#ifndef INC_1113_DEQUE_DEQUE_H
#define INC_1113_DEQUE_DEQUE_H

#include <iterator>
#include <cstddef>
#include "myStack.h"
#include "DequeIterator.h"

template <class Type>
class Deque {
private:
    std::vector<Type> frontStack, backStack;
    void reallocate(bool toFront) {
        if (toFront)
            moveRange(backStack, frontStack, (backStack.size() + 1) / 2);
        else
            moveRange(frontStack, backStack, (frontStack.size() + 1) / 2);
    }
public:
    typedef DequeIterator<Type, Type*, Type&> iterator;
    typedef DequeIterator< Type, const Type*, const Type&> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    Deque() {}
    Deque(size_t n) : frontStack(n), backStack(n) {}
    Deque(const Deque &deque) {
        frontStack = deque.frontStack;
        backStack = deque.backStack;
    }
    void push_front(const Type& elem) {
        frontStack.push_back(elem);
    }
    Type pop_front() {
        if (!frontStack.size())
            reallocate(true);
        Type res = frontStack.back();
        frontStack.pop_back();
        return res;
    }
    void push_back(const Type& elem) {
        backStack.push_back(elem);
    }
    Type pop_back() {
        if (!backStack.size())
            reallocate(false);
        Type res = backStack.back();
        backStack.pop_back();
        return res;
    }
    Type& operator[] (unsigned k) {
        if (k < frontStack.size())
            return frontStack[frontStack.size() - k - 1];
        else
            return backStack[k - frontStack.size()];
    }
    Type& operator[] (unsigned k) const {
        if (k < frontStack.size())
            return const_cast<Type&>(frontStack[frontStack.size() - k - 1]);
        else
            return const_cast<Type&>(backStack[k - frontStack.size()]);
    }
    bool empty() const {
        return !(frontStack.size() + backStack.size());
    }
    size_t size() const {
        return frontStack.size() + backStack.size();
    }
    Type& back() {
        return operator[](size() - 1);
    }
    Type& back() const {
        return (*this)[size() - 1];
    }
    Type& front() {
        return (*this)[0];
    }
    Type& front() const {
        return (*this)[0];
    }
    iterator begin() {
        return iterator(this, 0);
    }
    const_iterator begin() const {
        return cbegin();
    }
    const_iterator cbegin() const {
        return const_iterator(this, 0);
    }
    iterator end() {
        return iterator(this, size());
    }
    const_iterator end() const {
        return cend();
    }
    const_iterator cend() const {
        return const_iterator(this, size());
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return crbegin();
    }
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return crend();
    }
    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }
    void print() {
        for (int i = 0; i < size(); ++i)
            std::cout << (*this)[i] << " ";
        std::cout << std::endl;
    }
};
#endif //INC_1113_DEQUE_DEQUE_H
