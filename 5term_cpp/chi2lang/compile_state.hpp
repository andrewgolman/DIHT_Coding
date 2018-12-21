#pragma once

#include <map>

struct CompileState {
    std::map<int, int> var_values;
    int mark_count = 0;
    int var_count = 0;
    std::string LastMark() {
        return "$" + std::to_string(mark_count - 1);
    }
    std::string NewMark() {
        return "$" + std::to_string(mark_count++);
    }
    CompileState Copy() {
        return CompileState(*this);
    }
    void AddVar(int id) {
        var_values[var_count++] = id;
    }
};
