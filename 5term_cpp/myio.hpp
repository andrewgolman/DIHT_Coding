#pragma once

#include <ctime>
#include <iomanip>
#include <fstream>
#include <vector>

constexpr const char* BAD_INPUT_MSG = "ERROR: incorrect input";
using IterChar = std::vector<char>::const_iterator;
using IterCharV = std::vector<IterChar>;


void print_log(const char* c) {
    std::time_t timestamp = std::time(nullptr);
    time(&timestamp);
    std::tm timestamp_ptr = *std::localtime(&timestamp);
    std::cerr << std::put_time(&timestamp_ptr, "%c") << "\t" << c << std::endl;
}

void print_log(const std::string& s) {
    print_log(s.c_str());
}

double read_double() {
    double result;
    if (!(std::cin >> result)) {
        throw std::runtime_error(BAD_INPUT_MSG);
    }
    return result;
}

size_t file_size(const std::string& file_name) {
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    std::ifstream::pos_type size = ifs.tellg();
    if (size == -1) {
        throw std::runtime_error("Can't open file " + file_name);
    }
    return size;
}

std::vector<char> read_file_to_buffer(const std::string& file_name, bool add_endlines=false) {
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary);
    auto size = file_size(file_name) + 2 * add_endlines;
    std::vector<char> bytes(size);
    if (add_endlines) {
        bytes.front() = '\n';
        bytes.back() = '\n';
    }
    ifs.read(bytes.data() + add_endlines, size);
    return bytes;
}

IterCharV get_pointer_array_from_buffer(const std::vector<char>& buffer) {
    IterCharV iter_array;
    bool in_line = false;
    for (auto iter = buffer.begin(); iter != buffer.end(); ++iter) {
        if (!in_line && *iter != '\n') {
            in_line = true;
            iter_array.emplace_back(static_cast<IterChar>(iter));
        }
        if (*iter == '\n') {
            in_line = false;
        }
    }
    return iter_array;
}

void lines_from_iter_array_to_file(const IterCharV& ptr_array, const std::string& file_name) {
    std::ofstream ofs(file_name);
    for (auto ptr : ptr_array) {
        while (*ptr != '\n') {
            ofs << *ptr;
            ++ptr;
        }
        ofs << '\n';
    }
}

std::ostream myio_black_hole_stream(nullptr);

//------------------------------------------
//! Return std::cout if enable==1, else empty stream
//------------------------------------------
std::ostream& enable_stream(bool enable) {
    if (enable) {
        return std::cout;
    } else {
        return myio_black_hole_stream;
    }
}
