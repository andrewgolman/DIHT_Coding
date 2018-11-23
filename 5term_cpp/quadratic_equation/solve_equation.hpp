#pragma once

#include <vector>
#include <cmath>
#include "../utils.hpp"

template <typename Type>
const QERoots<Type> SE_INF_ROOTS = {3, 0, 0};

//------------------------------------------
//! Solves equation c == 0
//! @param[in]  c   coefficient c
//! @return   empty vector if c == 0 else SE_INF_ROOTS
//------------------------------------------
template <typename Type>
QERoots<Type> solve_constant_equation(Type c) {
    if (compare_to_zero(c) == 0) {
        return SE_INF_ROOTS<Type>;
    }
    else {
        return {0, 0, 0};
    }
}

//------------------------------------------
//! Solves equation k * x + b == 0
//! @param[in]  k   coefficient k
//! @param[in]  b   coefficient b
//! @return  vector of roots or SE_INF_ROOTS in case of infinite number of roots
//------------------------------------------
template <typename Type>
QERoots<Type> solve_linear_equation(Type k, Type b) {
    if (compare_to_zero(k) == 0) {
        return solve_constant_equation(b);
    } else {
        return {1, -b / k, 0};
    }
}

//------------------------------------------
//! Solves equation a^2 * x + b * x + c == 0
//! @param[in]  a   coefficient a
//! @param[in]  b   coefficient b
//! @param[in]  c   coefficient c
//! @return  vector of increasing roots or SE_INF_ROOTS in case of infinite number of roots
//------------------------------------------
template <typename Type>
QERoots<Type> solve_quadratic_equation(Type a, Type b, Type c) {
    if (compare_to_zero(a) == 0) {
        return solve_linear_equation(b, c);
    }
    auto discriminant = b * b - 4 * a * c;
    if (compare_to_zero(discriminant) == 1) {
        Type discriminant_sqrt = sqrt(discriminant);
        Type root_1 = (-b + discriminant_sqrt) / (2 * a);
        Type root_2 = (-b - discriminant_sqrt) / (2 * a);
        if (root_1 > root_2) {
            std::swap(root_1, root_2);
        }
        return {2, root_1, root_2};
    } else if (compare_to_zero(discriminant) == 0) {
        Type root = -b / (2 * a);
        return {1, root, 0};
    } else {
        return {0, 0, 0};
    }
}
