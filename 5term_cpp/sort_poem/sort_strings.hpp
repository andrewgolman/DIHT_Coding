#pragma once

#include "../myio.hpp"

int cmp_char_iter(const IterChar& a, const IterChar& b) {
    if (*b == '\n') {
        return -1;
    } else if (*a == '\n') {
        return 1;
    } else if (*a < *b) {
        return 1;
    } else if (*b < *a) {
        return -1;
    }
    return 0;
}

bool is_utf8_head(char byte) {
    return (byte & 0b11000000) != 0b10000000;
}

bool compare_iter_strings(const IterChar& a, const IterChar& b) {
    auto pa = a, pb = b;
    while (true) {
        int cmp_res = cmp_char_iter(pa, pb);
        if (cmp_res == 1) {
            return true;
        } else if (cmp_res == -1) {
            return false;
        }
        ++pa, ++pb;
    }
}

bool cmp_utf8(const IterChar& a, const IterChar& b) {
    return compare_iter_strings(a, b);
}

bool compare_iter_strings_reverse(const IterChar& a, const IterChar& b) {
    auto pa = a, pb = b;
    while (true) {
        while (*pa != '\n' && !is_utf8_head(*pa)) {
            --pa;
        }
        while (*pb != '\n' && !is_utf8_head(*pb)) {
            --pb;
        }
        if (cmp_utf8(pa, pb)) {
            return true;
        } else if (cmp_utf8(pb, pa)) {
            return false;
        }
        --pa, --pb;
    }
}

void iter_move_to_end(IterChar& iter) {
    IterChar next = iter + 1;
    while (*next != '\n') {
        ++iter;
        next = iter + 1;
    }
}

void iter_move_to_begin(IterChar& iter) {
    IterChar next = iter - 1;
    while (*next != '\n') {
        --iter;
        next = iter - 1;
    }
}

void sort_file_strings(IterCharV& iter_array) {
    auto comparator = [](const IterChar& a, const IterChar& b) {
        return compare_iter_strings(a, b);
    };
    std::sort(iter_array.begin(), iter_array.end(), comparator);
}

void sort_file_reversed_strings(IterCharV& iter_array) {
    for (auto& iter : iter_array) {
        iter_move_to_end(iter);
    }
    auto comparator = [](const IterChar& a, const IterChar& b) {
        int res = compare_iter_strings_reverse(a, b);
        return res;
    };
    std::sort(iter_array.begin(), iter_array.end(), comparator);
    for (auto& iter : iter_array) {
        iter_move_to_begin(iter);
    }
}

template <class Compare>
void sort_file_strings(IterCharV& iter_array, Compare compare) {
    std::sort(iter_array.begin(), iter_array.end(), compare);
}

template <class Compare>
void sort_file_reversed_strings(IterCharV& iter_array, Compare compare) {
    for (auto& iter : iter_array) {
        iter_move_to_end(iter);
    }
    std::sort(iter_array.begin(), iter_array.end(), compare);
    for (auto& iter : iter_array) {
        iter_move_to_begin(iter);
    }
}
