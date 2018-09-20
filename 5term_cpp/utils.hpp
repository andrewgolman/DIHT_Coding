#pragma once

constexpr double UT_EPS = 0.0000001;

template <typename Type>
int compare_to_zero(Type val) {
    if (val > UT_EPS) {
        return 1;
    } else if (val < -UT_EPS) {
        return -1;
    } else {
        return 0;
    }
}
