#pragma once

//constexpr size_t SS_MAX_STRING_SIZE = 256;

std::vector<const char*> get_pointer_array_from_buffer(const char* buffer) {
    std::vector<const char*> ptr_array;
    while (true) {
        ptr_array.emplace_back(buffer);  // todo not skip \0
        while (*buffer != '\n') {
            if (*buffer == '\0') {
                return ptr_array;
            }
            ++buffer;
        }
        ++buffer;
    }
    return ptr_array;
}

bool compare_strings(const char* a, const char* b) {
    for (int i = 0; i < SS_MAX_STRING_SIZE; ++i) {
        if (a[i] == '\n') {
            return true;
        } else if (b[i] == '\n') {
            return false;
        } else if (a[i] < b[i]) {
            return true;
        } else if (b[i] < a[i]) {
            return false;
        }
    }
    return false;
}

std::vector<const char*> sort_poem(const char* buffer) {
    std::vector<const char*> ptr_array = get_pointer_array_from_buffer(buffer);
    std::sort(ptr_array, &compare_strings);
    return ptr_array;
}
