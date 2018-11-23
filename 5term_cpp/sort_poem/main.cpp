#include <iostream>
#include "../myio.hpp"
#include "sort_strings.hpp"


int main(int argc, const char* argv[]) {
    if (argc < 3 || argc > 4) {
        std::cout << "Usage: ./sort_poem input_file output_file [-r]" << std::endl;
        exit(1);
    }
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    bool sort_reversed = (argc == 4 && argv[3][0] == '-' && argv[3][1] == 'r');

    print_log("Start reading from " + input_file + ".");

    std::vector<char> buffer = read_file_to_buffer(input_file, true);

    auto iter_array = get_pointer_array_from_buffer(buffer);

    print_log("Collected lines from " + input_file + ".");
    print_log("Start sorting.");

    if (!sort_reversed) {
        sort_file_strings(iter_array);
    } else {
        sort_file_reversed_strings(iter_array);
    }

    print_log("Sorting completed. Writing to " + output_file + ".");

    lines_from_iter_array_to_file(iter_array, output_file);

    print_log("Done.");
    return 0;
}
