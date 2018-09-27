#include <iostream>


int main(int argc, const char* argv[]) { // prototype
    if (argv != 4) {
        return 1;
    }
    const char* input_buffer = read_file(argv[0]);
    sort_file_lines(buffer, argv[1]);
    sort_file_lines_reversed(buffer, argv[2]);
    buffer_to_file(buffer, argv[3]);
    delete input_buffer;
    return 0;
}
