#include <map>
#include "../myio.hpp"

#undef std
#undef true
#undef false
#undef bool
#undef void
#undef size_t
#undef unique_ptr
#undef try
#undef catch
#undef sizeof
#undef typename
#undef this
#undef nullptr
#undef _buffer
#undef _size
#undef _allocated_size
#undef _canary_begin
#undef _canary_this
#undef CANARY
#undef VALIDATE_STACK

constexpr size_t CANARY = 0xAAAABBBBCCCCDDDDul;
constexpr size_t INIT_ALLOC = 64;
constexpr size_t HASH_BASE = 1003;

template <typename T>
class Stack;

template <typename T>
bool validate_stack(Stack<T>* stack) {
    return  stack &&
            stack->_canary_begin == CANARY &&
            stack->_canary_this == stack &&
            stack->_size <= stack->_allocated_size &&
            stack->_buffer.get() &&
            stack->_checksum == stack->checksum();
}

template <typename T>
bool validate_pointer(T* ptr) {
    return size_t(ptr) > 1000;
}

enum class StackErrorType {
    VALIDATE_ERROR,
    RETRIEVE,
    COPY,
    ALLOC,
    PUSH,
};


#define VALIDATE_STACK \
if (!validate_stack(this)) { \
    dump(this, StackErrorType::VALIDATE_ERROR); \
    return false; \
}


const std::map<StackErrorType, std::string> stack_error_descriptions = {
        {StackErrorType::VALIDATE_ERROR, "validation failed"},
        {StackErrorType::RETRIEVE, "can't retrieve element from stack"},
        {StackErrorType::COPY, "failed to copy element while extending the stack"},
        {StackErrorType::ALLOC, "failed to allocate memory while extending the stack"},
        {StackErrorType::PUSH, "failed to copy element onto the stack"},
};

template <typename T>
void dump(Stack<T>* stack, StackErrorType reason, std::ostream& out = std::cerr) {
    print_log("====DUMP====");
    out << "Stack at [" << stack << "] {\n";
    out << "Failure: " << stack_error_descriptions.at(reason) << "\n";
    out << "Canary 1: " << stack->_canary_begin << (stack->_canary_begin == CANARY ? " (ok)" : " (err)") << "\n";
    out << "Canary 2: " << stack->_canary_begin << (stack->_canary_this == stack ? " (ok)" : " (err)") << "\n";
    out << "Size: " << stack->_size << "\n";
    out << "Allocated buffer size: " << stack->_allocated_size << "\n";
    out << "Buffer: [" << stack->_buffer.get() << "]\n";
    for (size_t i = 0; i < stack->_size; ++i) {
        out << "[" << i << "] " << stack->_buffer.get()[i] << "\n";
    }
}


template <typename T>
class Stack {
public:
    Stack() {
        _canary_this = this;
        _buffer = std::unique_ptr<T>(new T[_allocated_size]);
        _checksum = checksum();
    }
    Stack(const Stack&) = delete;
    Stack& operator = (const Stack&) = delete;

    bool push(const T& val) {
        VALIDATE_STACK;
        if (_size == _allocated_size) {
            if (!extend()) {
                return false;
            }
        }
        try {
            _buffer.get()[_size] = val;
        } catch (const std::exception&) {
            dump(this, StackErrorType::PUSH);
            return false;
        }
        ++_size;
        _checksum = checksum();
        VALIDATE_STACK;
        return true;
    }

    bool pop(T* ptr = nullptr) {
        VALIDATE_STACK;
        if (!_size) {
            print_log("Popping from empty stack");
            return false;
        }
        if (ptr) {
            if (!validate_pointer(ptr)) {
                print_log("Pop failed: given invalid pointer");
                return false;
            }
            try {
                *ptr = std::move_if_noexcept(_buffer.get()[_size-1]);
            } catch (const std::exception&) {
                dump(this, StackErrorType::RETRIEVE);
                return false;
            }
        }
        --_size;
        _checksum = checksum();
        VALIDATE_STACK;
        return true;
    }
    size_t size() const {
        return _size;
    }

private:
    size_t _canary_begin{CANARY};
    size_t _size{0};
    size_t _allocated_size{INIT_ALLOC};
    size_t _checksum{0};
    std::unique_ptr<T> _buffer;
    void* _canary_this;

    friend bool validate_stack<T>(Stack<T>*);
    friend void dump<T>(Stack<T>*, StackErrorType, std::ostream&);

    bool extend() {
        VALIDATE_STACK;
        _allocated_size *= 2;
        std::unique_ptr<T> new_buffer = nullptr;
        try {
            new_buffer = std::unique_ptr<T>(new T[_allocated_size]);
        } catch (const std::exception&) {
            dump(this, StackErrorType::ALLOC);
            return false;
        }
        try {
            for (size_t i = 0; i < _size; ++i) {
                new_buffer.get()[i] = std::move_if_noexcept(_buffer.get()[i]);
            }
        } catch (const std::exception&) {
            dump(this, StackErrorType::COPY);
            return false;
        }
        _buffer = std::move(new_buffer);
        _checksum = checksum();
        VALIDATE_STACK;
        return true;
    }

    size_t checksum() const {
        return (size_t)_buffer.get() + HASH_BASE * _size + HASH_BASE * HASH_BASE * _allocated_size;
    }
};
/// conditional compile: define do_debug(x) : x
/// define STACK_RETURN
/// define dump __FILE__ __PRETTY_FUNCTION__ __LINE__
