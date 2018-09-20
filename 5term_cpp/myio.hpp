#pragma once

constexpr const char* BAD_INPUT_MSG = "ERROR: incorrect input";

void print_log(const char* c) {
    std::time_t timestamp;
    time(&timestamp);
    std::tm* timestamp_ptr = std::localtime(&timestamp);
    std::cerr << std::put_time(timestamp_ptr, "%c") << "\t" << c << std::endl;
}

void print_log(const std::string& s) {
    print_log(s.c_str());
}

template <typename Type>
Type read_type(bool write_log = false) {
    Type result;
    if (!(std::cin >> result)) {
        std::cout << BAD_INPUT_MSG << std::endl;
        if (write_log) {
            print_log(BAD_INPUT_MSG);
        }
        std::exit(1);
    }
    return result;
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
