#include <utility>

template <typename T>
class UniquePtr {
private:

    T* ptr;

public:

    UniquePtr(T* p) : ptr(p) {}

    UniquePtr(UniquePtr&& un) {
        ptr = un.ptr;
        un.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& un) {
        if (ptr)
            delete ptr;
        ptr = un.ptr;
        un.ptr = nullptr;
        return *this;
    }

    ~UniquePtr() {
        if (ptr)
            delete ptr;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    T* get() const {
        return ptr;
    }

    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T* next) {
        if (ptr)
            delete ptr;
        ptr = next;
    }

    void swap(UniquePtr& un) {
        T* tmp = ptr;
        ptr = un.ptr;
        un.ptr = tmp;
    }
};


template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

template <typename T>
struct Counter {
    friend WeakPtr<T>;
    friend SharedPtr<T>;
private:
    int c{0};
    int wc{0};
};

template <typename T>
class SharedPtr {
    friend WeakPtr<T>;
private:
    T* ptr;
    Counter<T>* counter;

    void inc() {
        if (ptr)
            counter->c += 1;
    }

    void dec() {
        if (ptr) {
            counter->c -= 1;
            if (!counter->c) {
                delete ptr;
                ptr = nullptr;
                if (!counter->wc) {
                    delete counter;
                    counter = nullptr;
                }
            }
        }
    }

public:

    SharedPtr(T* p = nullptr) : ptr(p) {
        if (ptr) {
            counter = new Counter<T>;
            inc();
        }
    }

    SharedPtr(const SharedPtr& sh) {
        ptr = sh.ptr;
        counter = sh.counter;
        inc();
    }

    SharedPtr(SharedPtr&& sh) {
        ptr = sh.ptr;
        counter = std::move(sh.counter);

        sh.ptr = nullptr;
        sh.counter = nullptr;
    }

    SharedPtr(const UniquePtr<T>& un) {
        ptr = un.get();
        un.release();
        counter = new Counter<T>;
        inc();
    }

    SharedPtr(const WeakPtr<T>& wk) {
        if (!wk.ptr)
            throw;// std::bad_weak_ptr();
        ptr = wk.ptr;
        counter = wk.counter;
        inc();
    }

    SharedPtr& operator=(SharedPtr& sh) {
        dec();
        ptr = sh.ptr;
        counter = sh.counter;
        inc();
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& sh) {
        dec();
        ptr = sh.ptr;
        counter = std::move(sh.counter);

        sh.ptr = nullptr;
        sh.counter = nullptr;
        return *this;
    }

    ~SharedPtr() {
        dec();
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    T* get() const {
        return ptr;
    }

    void reset(T* next = nullptr) {
        dec();
        ptr = next;
        if (ptr) {
            counter = new Counter<T>;
            inc();
        }
    }

    void swap(SharedPtr& sh) {
        T* tmp = ptr;
        ptr = sh.ptr;
        sh.ptr = tmp;

        Counter<T>* tmp_counter = counter;
        counter = sh.counter;
        sh.counter = tmp_counter;
    }

    int use_count() const {
        return counter ? counter->c : 0;
    }
};


template <typename T>
class WeakPtr {
private:

    friend SharedPtr<T>;
    T* ptr;
    Counter<T>* counter;

    void inc() {
        if (ptr)
            counter->wc += 1;
    }

    void dec() {
        if (counter) {
            counter->wc -= 1;
            if (!counter->c && !counter->wc) {
                delete counter;
                counter = nullptr;
            }
        }
    }

public:

    WeakPtr() {
        counter = nullptr;
        ptr = nullptr;
    }

    WeakPtr(const SharedPtr<T>& sh) : ptr(sh.ptr), counter(sh.counter) {
        inc();
    }

    WeakPtr(const WeakPtr& wk) {
        ptr = wk.ptr;
        counter = wk.counter;
        inc();
    }

    WeakPtr(WeakPtr&& wk) {
        ptr = wk.ptr;
        counter = std::move(wk.counter);

        wk.ptr = nullptr;
        wk.counter = nullptr;
    }

    WeakPtr& operator=(const SharedPtr<T>& sh) {
        dec();
        ptr = sh.ptr;
        counter = sh.counter;
        inc();
        return *this;
    }

    WeakPtr& operator=(const WeakPtr& wk) {
        dec();
        ptr = wk.ptr;
        counter = wk.counter;
        inc();
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& wk) {
        dec();
        ptr = wk.ptr;
        counter = std::move(wk.counter);

        wk.ptr = nullptr;
        wk.counter = nullptr;
        return *this;
    }

    bool expired() const {
        return !(ptr && counter && counter->c);
    }

    void reset() {
        dec();
        ptr = nullptr;
        counter = nullptr;
    }

    void swap(WeakPtr& wk) {
        T* tmp = ptr;
        ptr = wk.ptr;
        wk.ptr = tmp;

        Counter<T>* tmp_counter = counter;
        counter = wk.counter;
        wk.counter = tmp_counter;
    }

    int use_count() {
        return counter ? counter->c : 0;
    }

    const SharedPtr<T> lock() {
        return ptr ? SharedPtr<T>(*this) : SharedPtr<T>(nullptr);
    }

    ~WeakPtr() {
        dec();
    }
};
