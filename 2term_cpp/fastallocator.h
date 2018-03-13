//
// Created by Andrew on 03.05.17.
//

#ifndef ALLOCATOR_FASTALLOCATOR_H
#define ALLOCATOR_FASTALLOCATOR_H

#include <memory>

template <size_t chunkSize>
class FixedAllocator {

    class MyStack {
    public:
        int size = 0;
        int capacity = 0;
        int sv = sizeof(void*);
        void** stack = nullptr;

        MyStack() :
                size(0),
                sv(sizeof(void*)),
                stack(nullptr)
        {}

        void fill(void* ptr, size_t n) {
            capacity = n+16;
            stack = static_cast<void**>(realloc(stack, capacity * sv));
            for (size_t i = 0; i < n; ++i) {
                push(static_cast<char*>(ptr) + i * chunkSize);
            }
        }

        void push(void* obj) {
            if (size >= capacity - 1) {
                capacity *= 2;
                stack = static_cast<void**>(realloc(stack, capacity * sv));
            }
            stack[size] = obj;
            ++size;
        }

        void* pop() {
            if (!size)
                return nullptr;
            --size;
            return stack[size];
        }

        ~MyStack() {
            if (stack) {
                free(stack);
            }
        }
    };

    MyStack buffer = MyStack();
    size_t next_allocation = 16;

    int blocks_allocated = 0;
    void* blocks[32];

    bool created = 0;
    FixedAllocator() {};
    FixedAllocator(const FixedAllocator&);
    FixedAllocator& operator= (const FixedAllocator&);
    ~FixedAllocator() {
        for (int i = 0; i < blocks_allocated; ++i) {
            free(blocks[i]);
        }
    }

public:

    void* allocate() {
        void* res = buffer.pop();
        if (res)
            return res;
        blocks[blocks_allocated] = malloc(chunkSize * next_allocation);
        buffer.fill(blocks[blocks_allocated], next_allocation);
        next_allocation *= 2;
        ++blocks_allocated;
        return buffer.pop();
    }

    void deallocate(void* ptr) {
        buffer.push(ptr);
    }

    static FixedAllocator& instance() {
        static FixedAllocator s;
        return s;
    }
};


template <typename T>
class FastAllocator {
    FixedAllocator<sizeof(T)>& fixedAllocator;
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;


    FastAllocator() :
        fixedAllocator(FixedAllocator<sizeof(T)>::instance())
    {}

    FastAllocator(const FastAllocator&) :
            fixedAllocator(FixedAllocator<sizeof(T)>::instance())
    {}

    FastAllocator& operator=(const FastAllocator&) {
        return *this;
    }

    T* allocate(size_t n) {
        if (n == 1)
            return static_cast<T*>(fixedAllocator.allocate());
        return new T[n];
    }

    void deallocate(T* ptr, size_t n=1) {
        if (n == 1)
            fixedAllocator.deallocate(ptr);
        else {
            delete ptr;
        }
    }

    template<class U, class... Args>
    void construct(U* p, Args&&... args) {
        ::new((void*) p) U(std::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U* p) {
        p->~U();
    }

    template<class U>
    struct rebind {
        typedef FastAllocator<U> other;
    };

};



template <typename T, class Allocator = std::allocator<T>>
class List {

    struct Node {
        T val;
        Node* prev = nullptr;
        Node* next = nullptr;

        Node() : val(T()), prev(nullptr), next(nullptr) {}

        template <class U>
        Node(U&& value, Node* prevNode=nullptr, Node* nextNode=nullptr) :
                val(value), prev(prevNode), next(nextNode) {}
    };

    size_t size_ = 0;
    Node* begin = nullptr;
    Node* end = nullptr;

    using allocType = typename Allocator::template rebind<Node>::other;

    allocType allocator;

    void fill(const List& l) {
        allocator = allocType();
        size_ = l.size_;
        begin = allocator.allocate(1);
        Node* lptr = l.begin;
        Node* cur = begin;
        allocator.construct(begin, lptr->val);
        for (size_t i = 1; i < size_; ++i) {
            cur->next = allocator.allocate(1);
            lptr = lptr->next;
            allocator.construct(cur->next, lptr->val, cur);
            cur = cur->next;
        }
    }

public:

    explicit List(const Allocator& = Allocator()) {
        allocator = allocType();
    }

    List(size_t count, const T& value = T(), const Allocator& = Allocator()) {
        allocator = allocType();
        size_ = count;
        begin = allocator.allocate(1);
        allocator.construct(begin, value);
        Node* cur = begin;
        for (size_t i = 1; i < count; ++i) {
            cur->next = allocator.allocate(1);
            allocator.construct(cur->next, value, cur);
            cur = cur->next;
        }
    }

    List(const List& l) {
        fill(l);
    }

    List(List&& l) {
        allocator = allocType();
        begin = l.begin;
        end = l.end;
        size_ = l.size_;
    }

    List& operator=(const List& l) {
        fill(l);
    }

    List& operator=(List&& l) {
        allocator = allocType();
        begin = l.begin;
        end = l.end;
        size_ = l.size_;
    }

    size_t size() const {
        return size_;
    }

    template <class U>
    void push_back(U&& t) {
        insert_after(end, std::forward<U>(t));
    }

    template <class U>
    void push_front(U&& t) {
        insert_before(begin, std::forward<U>(t));
    }

    void pop_back() {
        erase(end);
    }
    void pop_front() {
        erase(begin);
    }

    template <class U>
    void insert_before(Node* n, U&& t) {
        Node* newNode = allocator.allocate(1);
        allocator.construct(newNode, std::forward<U>(t), nullptr, n);
        if (n) {
            if (n->prev)
                n->prev->next = newNode;
            else
                begin = newNode;
            newNode->prev = n->prev;
            n->prev = newNode;
        }
        else
            begin = end = newNode;
        ++size_;
    }

    template <class U>
    void insert_after(Node* n, U&& t) {
        Node* newNode = allocator.allocate(1);
        allocator.construct(newNode, std::forward<U>(t), n, nullptr);
        if (n) {
            if (n->next)
                n->next->prev = newNode;
            else
                end = newNode;
            newNode->next = n->next;
            n->next = newNode;
        }
        else
            begin = end = newNode;
        ++size_;
    }

    void erase(Node* n) {
        if (!n)
            return;
        if (n->prev)
            n->prev->next = n->next;
        else {
            begin = n->next;
            if (begin)
                begin->prev = nullptr;
        }
        if (n->next)
            n->next->prev = n->prev;
        else {
            end = n->prev;
            if (end)
                end->next = nullptr;
        }
        allocator.destroy(n);
        allocator.deallocate(n, 1);
        --size_;
    }

    ~List() {
        while (begin)
            erase(begin);
    }
};

#endif //ALLOCATOR_FASTALLOCATOR_H
