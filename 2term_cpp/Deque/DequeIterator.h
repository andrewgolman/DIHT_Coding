#ifndef INC_1113_DEQUE_DEQUEITERATOR_H
#define INC_1113_DEQUE_DEQUEITERATOR_H

#include <iterator>

typedef size_t diff_type;

template <class Type>
class Deque;

template <class Type, class TypePtr, class TypeRef>
struct DequeIterator : std::iterator<std::random_access_iterator_tag, Type, diff_type, TypePtr, TypeRef> {
    friend class Deque<Type>;
private:
    Deque<Type> const *_deque;
    size_t _pos;
public:
    DequeIterator(const Deque<Type> *deque, size_t pos) : _deque(deque), _pos(pos) {}
    DequeIterator(const DequeIterator &iter) : _deque(iter._deque), _pos(iter._pos) {}
    DequeIterator& operator = (const DequeIterator &iter) {
        _deque = iter._deque;
        _pos = iter._pos;
        return *this;
    }
    bool operator == (const DequeIterator &iter) const {
        return _pos == iter._pos;
    }
    bool operator != (const DequeIterator &iter) const {
        return _pos != iter._pos;
    }
    bool operator < (const DequeIterator &iter) const  {
        return _pos < iter._pos;
    }
    bool operator <= (const DequeIterator &iter) const  {
        return _pos <= iter._pos;
    }
    bool operator > (const DequeIterator &iter) const  {
        return _pos > iter._pos;
    }
    bool operator >= (const DequeIterator &iter) const  {
        return _pos >= iter._pos;
    }
    TypeRef operator * () const {
        return const_cast<TypeRef>((*_deque)[_pos]);
    }
    TypePtr operator -> () const {
        return this;
    }
    DequeIterator& operator ++ () {
        ++_pos;
        return *this;
    }
    DequeIterator operator ++ (int) {
        DequeIterator res(*this);
        ++_pos;
        return res;
    }
    DequeIterator& operator -- () {
        --_pos;
        return *this;
    }
    DequeIterator operator -- (int) {
        DequeIterator res(*this);
        --_pos;
        return res;
    }
    DequeIterator& operator += (size_t n) {
        _pos += n;
        return *this;
    }
    DequeIterator operator + (size_t n) const {
        DequeIterator res(*this);
        res += n;
        return res;
    }
    DequeIterator& operator -= (size_t n) {
        _pos -= n;
        return *this;
    }
    DequeIterator operator - (size_t n) const {
        DequeIterator res(*this);
        res -= n;
        return res;
    }
    size_t operator - (const DequeIterator &iter) const {
        return _pos - iter._pos;
    }
    TypeRef operator [] (size_t n) {
        return *(this + n);
    }
    TypeRef operator [] (size_t n) const {
        return *(this + n);
    }
};

template <class Type, class TypePtr, class TypeRef>
const DequeIterator<Type, TypePtr, TypeRef> operator + (diff_type k, DequeIterator<Type, TypePtr, TypeRef> iter) {
    return iter + k;
};

template <class Type, class TypePtr, class TypeRef>
const DequeIterator<Type, TypePtr, TypeRef> operator - (diff_type k, DequeIterator<Type, TypePtr, TypeRef> iter) {
    return iter - k;
};

#endif //INC_1113_DEQUE_DEQUEITERATOR_H

